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

#include "synergy/ArgsBase.h"

ArgsBase::ArgsBase() :
#if SYSAPI_WIN32
m_daemon(false), // daemon mode not supported on windows (use --service)
m_debugServiceWait(false),
m_pauseOnExit(false),
m_stopOnDeskSwitch(false),
#else
m_daemon(true), // backward compatibility for unix (daemon by default)
#endif
#if WINAPI_XWINDOWS
m_disableXInitThreads(false),
#endif
m_backend(false),
m_restartable(true),
m_noHooks(false),
m_pname(NULL),
m_logFilter(NULL),
m_logFile(NULL),
m_display(NULL),
m_disableTray(false),
m_enableIpc(false),
m_enableDragDrop(false),
m_shouldExit(false),
m_synergyAddress(),
m_enableCrypto(false),
m_profileDirectory(""),
m_pluginDirectory("")
{
}

ArgsBase::~ArgsBase()
{
}
