/*
 * Deskflow -- mouse and keyboard sharing utility
 * Copyright (C) 2021 Symless Ltd.
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

#include "ServerConnection.h"

#include "ServerMessage.h"
#include "gui/config/ServerConfigDialogState.h"
#include "messages.h"

#include <QMessageBox>
#include <QPushButton>

namespace deskflow::gui {

//
// ServerConnection::Deps
//

messages::NewClientPromptResult ServerConnection::Deps::showNewClientPrompt(QWidget *parent, const QString &clientName) const
{
    return messages::showNewClientPrompt(parent, clientName);
}

//
// ServerConnection
//

ServerConnection::ServerConnection(QWidget *parent,
                                   IAppConfig &appConfig,
                                   IServerConfig &serverConfig,
                                   const config::ServerConfigDialogState &serverConfigDialogState,
                                   std::shared_ptr<Deps> deps)
    : m_pParent(parent)
    , m_appConfig(appConfig)
    , m_serverConfig(serverConfig)
    , m_serverConfigDialogState(serverConfigDialogState)
    , m_pDeps(deps)
{
}

void ServerConnection::handleLogLine(const QString &logLine)
{
    ServerMessage message(logLine);

    if (!message.isNewClientMessage()) {
        return;
    }

    if (m_messageShowing) {
        qDebug("new client message already shown, skipping for now");
        return;
    }

    if (m_serverConfigDialogState.isVisible()) {
        qDebug("server config dialog visible, skipping new client prompt");
        return;
    }

    if (m_appConfig.useExternalConfig()) {
        qDebug("external config enabled, skipping new client prompt");
        return;
    }

    const auto client = message.getClientName();

    if (m_receivedClients.contains(client)) {
        qDebug("already got request, skipping new client prompt for: %s", qPrintable(client));
        return;
    }

    handleNewClient(message.getClientName());
}

void ServerConnection::handleNewClient(const QString &clientName)
{
    using enum messages::NewClientPromptResult;

    m_receivedClients.append(clientName);

    if (m_serverConfig.isFull()) {
        qDebug("server config full, skipping new client prompt for: %s", qPrintable(clientName));
        return;
    }

    if (m_serverConfig.screenExists(clientName)) {
        qDebug("client already added, skipping new client prompt for: %s", qPrintable(clientName));
        return;
    }

    emit messageShowing();

    m_messageShowing = true;
    const auto result = m_pDeps->showNewClientPrompt(m_pParent, clientName);
    m_messageShowing = false;

    if (result == Add) {
        qDebug("accepted dialog, adding client: %s", qPrintable(clientName));
        emit configureClient(clientName);
    } else if (result == Ignore) {
        qDebug("declined dialog, ignoring client: %s", qPrintable(clientName));
    } else {
        qFatal("unexpected add client result");
    }
}

} // namespace deskflow::gui
