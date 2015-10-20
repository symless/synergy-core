/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2012 Synergy Si Ltd.
 * Copyright (C) 2012 Nick Bolton
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
 * 
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the OpenSSL
 * library.
 * You must obey the GNU General Public License in all respects for all of
 * the code used other than OpenSSL. If you modify file(s) with this
 * exception, you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete
 * this exception statement from your version. If you delete this exception
 * statement from all source files in the program, then also delete it here.
 */

#pragma once

#include "ipc/Ipc.h"
#include "arch/IArchMultithread.h"
#include "base/EventTypes.h"
#include "base/Event.h"

namespace synergy { class IStream; }
class IpcMessage;
class IpcCommandMessage;
class IpcHelloMessage;
class IEventQueue;

class IpcClientProxy {
	friend class IpcServer;

public:
	IpcClientProxy(synergy::IStream& stream, IEventQueue* events);
	virtual ~IpcClientProxy();

private:
	void				send(const IpcMessage& message);
	void				handleData(const Event&, void*);
	void				handleDisconnect(const Event&, void*);
	void				handleWriteError(const Event&, void*);
	IpcHelloMessage*	parseHello();
	IpcCommandMessage*	parseCommand();
	void				disconnect();
	
private:
	synergy::IStream&	m_stream;
	EIpcClientType		m_clientType;
	bool				m_disconnecting;
	ArchMutex			m_readMutex;
	ArchMutex			m_writeMutex;
	IEventQueue*		m_events;
};
