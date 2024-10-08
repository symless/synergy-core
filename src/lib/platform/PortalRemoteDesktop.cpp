/*
 * Deskflow -- mouse and keyboard sharing utility
 * Copyright (C) 2022 Red Hat, Inc.
 * Copyright (C) 2024 Symless Ltd.
 *
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "platform/PortalRemoteDesktop.h"
#include "base/Log.h"
#include "base/TMethodJob.h"

#include <sys/socket.h> // for EIS fd hack, remove
#include <sys/un.h> // for EIS fd hack, remove

namespace deskflow {

PortalRemoteDesktop::PortalRemoteDesktop(EiScreen *screen, IEventQueue *events)
    : screen_(screen)
    , events_(events)
    , portal_(xdp_portal_new())
{
    glib_main_loop_ = g_main_loop_new(nullptr, true);
    glib_thread_ = new Thread(new TMethodJob<PortalRemoteDesktop>(this, &PortalRemoteDesktop::glib_thread));

    reconnect(0);
}

PortalRemoteDesktop::~PortalRemoteDesktop()
{
    if (g_main_loop_is_running(glib_main_loop_))
        g_main_loop_quit(glib_main_loop_);

    if (glib_thread_ != nullptr) {
        glib_thread_->cancel();
        glib_thread_->wait();
        delete glib_thread_;
        glib_thread_ = nullptr;

        g_main_loop_unref(glib_main_loop_);
        glib_main_loop_ = nullptr;
    }

    if (session_signal_id_)
        g_signal_handler_disconnect(session_, session_signal_id_);
    if (session_ != nullptr)
        g_object_unref(session_);
    g_object_unref(portal_);

    free(session_restore_token_);
}

gboolean PortalRemoteDesktop::timeout_handler()
{
    return true; // keep re-triggering
}

void PortalRemoteDesktop::reconnect(unsigned int timeout)
{
    auto init_cb = [](gpointer data) -> gboolean {
        return reinterpret_cast<PortalRemoteDesktop *>(data)->init_remote_desktop_session();
    };

    if (timeout > 0)
        g_timeout_add(timeout, init_cb, this);
    else
        g_idle_add(init_cb, this);
}

void PortalRemoteDesktop::cb_session_closed(XdpSession *session)
{
    LOG_ERR("portal remote desktop session was closed, reconnecting");
    g_signal_handler_disconnect(session, session_signal_id_);
    session_signal_id_ = 0;
    events_->addEvent(Event(events_->forEi().sessionClosed(), screen_->getEventTarget()));

    // gcc warning "Suspicious usage of 'sizeof(A*)'" can be ignored
    g_clear_object(&session_);

    reconnect(1000);
}

void PortalRemoteDesktop::cb_session_started(GObject *object, GAsyncResult *res)
{
    g_autoptr(GError) error = nullptr;
    auto session = XDP_SESSION(object);
    auto success = xdp_session_start_finish(session, res, &error);
    if (!success) {
        LOG_ERR("failed to start portal remote desktop session, quitting: %s", error->message);
        g_main_loop_quit(glib_main_loop_);
        events_->addEvent(Event::kQuit);
        return;
    }

    session_restore_token_ = xdp_session_get_restore_token(session);

    // ConnectToEIS requires version 2 of the xdg-desktop-portal (and the same
    // version in the impl.portal), i.e. you'll need an updated compositor on
    // top of everything...
    auto fd = -1;
#if HAVE_LIBPORTAL_SESSION_CONNECT_TO_EIS
    fd = xdp_session_connect_to_eis(session, &error);
#endif
    if (fd < 0) {
        g_main_loop_quit(glib_main_loop_);
        events_->addEvent(Event::kQuit);
        return;
    }

    // Socket ownership is transferred to the EiScreen
    events_->addEvent(Event(events_->forEi().connected(), screen_->getEventTarget(), EiScreen::EiConnectInfo::alloc(fd)));
}

void PortalRemoteDesktop::cb_init_remote_desktop_session(GObject *object, GAsyncResult *res)
{
    LOG_DEBUG("portal remote desktop session initialized");
    g_autoptr(GError) error = nullptr;

    auto session = xdp_portal_create_remote_desktop_session_finish(XDP_PORTAL(object), res, &error);
    if (!session) {
        LOG_ERR("failed to initialize remote desktop session: %s", error->message);
        // This was the first attempt to connect to the RD portal - quit if that
        // fails.
        if (session_iteration_ == 0) {
            g_main_loop_quit(glib_main_loop_);
            events_->addEvent(Event::kQuit);
        } else {
            this->reconnect(1000);
        }
        return;
    }

    session_ = session;
    ++session_iteration_;

    // FIXME: the lambda trick doesn't work here for unknown reasons, we need
    // the static function
    session_signal_id_ = g_signal_connect(G_OBJECT(session), "closed", G_CALLBACK(cb_session_closed_cb), this);

    LOG_DEBUG("portal remote desktop session starting");
    xdp_session_start(
        session,
        nullptr, // parent
        nullptr, // cancellable
        [](GObject *obj, GAsyncResult *res, gpointer data) {
            reinterpret_cast<PortalRemoteDesktop *>(data)->cb_session_started(obj, res);
        },
        this);
}

#if !defined(HAVE_LIBPORTAL_CREATE_REMOTE_DESKTOP_SESSION_FULL)
static inline void xdp_portal_create_remote_desktop_session_full(XdpPortal *portal,
                                                                 XdpDeviceType devices,
                                                                 XdpOutputType outputs,
                                                                 XdpRemoteDesktopFlags flags,
                                                                 XdpCursorMode cursor_mode,
                                                                 XdpPersistMode _unused1,
                                                                 const char *_unused2,
                                                                 GCancellable *cancellable,
                                                                 GAsyncReadyCallback callback,
                                                                 gpointer data)
{
    xdp_portal_create_remote_desktop_session(portal, devices, outputs, flags, cursor_mode, cancellable, callback, data);
}
#endif

gboolean PortalRemoteDesktop::init_remote_desktop_session()
{
    LOG_DEBUG("setting up remote desktop session with restore token %s", session_restore_token_);
    xdp_portal_create_remote_desktop_session_full(
        portal_,
        static_cast<XdpDeviceType>(XDP_DEVICE_POINTER | XDP_DEVICE_KEYBOARD),
        XDP_OUTPUT_NONE,
        XDP_REMOTE_DESKTOP_FLAG_NONE,
        XDP_CURSOR_MODE_HIDDEN,
        XDP_PERSIST_MODE_TRANSIENT,
        session_restore_token_,
        nullptr, // cancellable
        [](GObject *obj, GAsyncResult *res, gpointer data) {
            reinterpret_cast<PortalRemoteDesktop *>(data)->cb_init_remote_desktop_session(obj, res);
        },
        this);

    return false; // don't reschedule
}

void PortalRemoteDesktop::glib_thread(void *)
{
    auto context = g_main_loop_get_context(glib_main_loop_);

    while (g_main_loop_is_running(glib_main_loop_)) {
        Thread::testCancel();
        g_main_context_iteration(context, true);
    }
}

} // namespace deskflow
