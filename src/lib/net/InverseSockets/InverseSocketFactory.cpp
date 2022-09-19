/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2012-2022 Symless Ltd.
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
#include "InverseSocketFactory.h"
#include "net/TCPSocket.h"
#include "net/TCPListenSocket.h"
#include "net/SecureSocket.h"
#include "net/SecureListenSocket.h"

//
// InverseSocketFactory
//

InverseSocketFactory::InverseSocketFactory(IEventQueue* events, SocketMultiplexer* socketMultiplexer) :
    m_events(events),
    m_socketMultiplexer(socketMultiplexer)
{
}

IDataSocket*
InverseSocketFactory::create(bool secure, IArchNetwork::EAddressFamily family) const
{
    if (secure) {
        SecureSocket* secureSocket = new SecureSocket(m_events, m_socketMultiplexer, family);
        secureSocket->initSsl (false);
        return secureSocket;
    }
    else {
        return new TCPSocket(m_events, m_socketMultiplexer, family);
    }
}

IListenSocket*
InverseSocketFactory::createListen(bool secure, IArchNetwork::EAddressFamily family) const
{
    IListenSocket* socket = nullptr;

    if (secure) {
        socket = new SecureListenSocket(m_events, m_socketMultiplexer, family);
    }
    else {
        socket = new TCPListenSocket(m_events, m_socketMultiplexer, family);
    }

    return socket;
}
