/*
 * Deskflow -- mouse and keyboard sharing utility
 * Copyright (C) 2012 Symless Ltd.
 * Copyright (C) 2008 Volker Lanz (vl@fidra.de)
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

#include "DeskflowApplication.h"
#include "MainWindow.h"
#include "SetupWizard.h"
#include "common/constants.h"
#include "common/version.h"
#include "gui/Logger.h"
#include "gui/config/AppConfig.h"
#include "gui/config/ConfigScopes.h"
#include "gui/constants.h"
#include "gui/diagnostic.h"
#include "gui/dotenv.h"
#include "gui/messages.h"
#include "gui/string_utils.h"
#include "gui_config.h" // IWYU pragma: keep

#ifdef DESKFLOW_GUI_HOOK_HEADER
#include DESKFLOW_GUI_HOOK_HEADER
#endif

#include <QApplication>
#include <QDebug>
#include <QGuiApplication>
#include <QMessageBox>
#include <QObject>
#include <QtCore>
#include <QtGlobal>
#include <QtGui>

#if defined(Q_OS_MAC)
#include <Carbon/Carbon.h>
#include <cstdlib>
#endif

using namespace deskflow::gui;

class QThreadImpl : public QThread
{
public:
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
};

#if defined(Q_OS_MAC)
bool checkMacAssistiveDevices();
#endif

bool hasArg(const QString &arg, const QStringList &args)
{
    return std::ranges::any_of(args, [&arg](const QString &a) {
        return a == arg;
    });
}

int main(int argc, char *argv[])
{
#if defined(Q_OS_MAC)
    /* Workaround for QTBUG-40332 - "High ping when QNetworkAccessManager is
     * instantiated" */
    ::setenv("QT_BEARER_POLL_TIMEOUT", "-1", 1);
#endif

    QCoreApplication::setApplicationName(kAppName);
    QCoreApplication::setOrganizationName(kAppName);

    // used as a prefix for settings paths, and must not be a url.
    QCoreApplication::setOrganizationDomain(kOrgDomain);

    DeskflowApplication app(argc, argv);

    qInstallMessageHandler(deskflow::gui::messages::messageHandler);
    QString version = QString::fromStdString(deskflow::version());
    qInfo(DESKFLOW_APP_NAME " v%s", qPrintable(version));

    dotenv();
    Logger::instance().loadEnvVars();

#if defined(Q_OS_MAC)

    if (app.applicationDirPath().startsWith("/Volumes/")) {
        QMessageBox::information(NULL,
                                 DESKFLOW_APP_NAME,
                                 "Please drag " DESKFLOW_APP_NAME
                                 " to the Applications folder, "
                                 "and open it from there.");
        return 1;
    }

    if (!checkMacAssistiveDevices()) {
        return 1;
    }
#endif

    ConfigScopes configScopes;

    // --no-reset
    QStringList arguments = QCoreApplication::arguments();
    const auto noReset = hasArg("--no-reset", arguments);
    const auto resetEnvVar = strToTrue(qEnvironmentVariable("DESKFLOW_RESET_ALL"));
    if (resetEnvVar && !noReset) {
        diagnostic::clearSettings(configScopes, false);
    }

    AppConfig appConfig(configScopes);

    QObject::connect(
        &configScopes,
        &ConfigScopes::saving,
        &appConfig,
        [&appConfig]() {
            appConfig.commit();
        },
        Qt::DirectConnection);

    if (appConfig.wizardShouldRun()) {
        SetupWizard wizard(appConfig);
        auto result = wizard.exec();
        if (result != QDialog::Accepted) {
            qInfo("wizard cancelled, exiting");
            return 0;
        }

        configScopes.save();
    }

    MainWindow mainWindow(configScopes, appConfig);

    QObject::connect(&app, &DeskflowApplication::aboutToQuit, &mainWindow, &MainWindow::onAppAboutToQuit);

    mainWindow.open();

#ifdef DESKFLOW_GUI_HOOK_START
    DESKFLOW_GUI_HOOK_START
#endif

    return DeskflowApplication::exec();
}

#if defined(Q_OS_MAC)
bool checkMacAssistiveDevices()
{
#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1090 // mavericks

    // new in mavericks, applications are trusted individually
    // with use of the accessibility api. this call will show a
    // prompt which can show the security/privacy/accessibility
    // tab, with a list of allowed applications. deskflow should
    // show up there automatically, but will be unchecked.

    if (AXIsProcessTrusted()) {
        return true;
    }

    const void *keys[] = {kAXTrustedCheckOptionPrompt};
    const void *trueValue[] = {kCFBooleanTrue};
    CFDictionaryRef options = CFDictionaryCreate(NULL, keys, trueValue, 1, NULL, NULL);

    bool result = AXIsProcessTrustedWithOptions(options);
    CFRelease(options);
    return result;

#else

    // now deprecated in mavericks.
    bool result = AXAPIEnabled();
    if (!result) {
        QMessageBox::information(NULL,
                                 DESKFLOW_APP_NAME,
                                 "Please enable access to assistive devices "
                                 "System Preferences -> Security & Privacy -> "
                                 "Privacy -> Accessibility, then re-open " DESKFLOW_APP_NAME ".");
    }
    return result;

#endif
}
#endif
