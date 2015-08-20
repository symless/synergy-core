/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2015 Synergy Si, Std.
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

#include "FileSysClient.h"

#include "EditionType.h"
#include "QUtility.h"

#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QCoreApplication>
#include <stdexcept>

void FileSysClient::queryPluginList()
{
	try {
		isDone(false);
		QString extension = "*" + Plugin::getOsSpecificExt();
		QStringList nameFilter(extension);

		QString installDir(m_CoreInterface.getInstalledDir()
							.append(QDir::separator())
							.append(Plugin::getOsSpecificInstallerLocation()));

		QString searchDirectory(installDir);
		QDir directory(searchDirectory);
		m_PluginList = directory.entryList(nameFilter);
		isDone(true);
	}
	catch (std::exception& e)
	{
		isDone(true);
		emit error(tr(	"An error occurred while trying to load the "
						"plugin list. Please contact the help desk, and "
						"provide the following details.\n\n%1").arg(e.what()));
	}
	emit queryPluginDone();
	return;
}
