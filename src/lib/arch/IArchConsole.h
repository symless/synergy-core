/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2012 Synergy Si Ltd.
 * Copyright (C) 2002 Chris Schoeneman
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

#include "common/IInterface.h"
#include "base/ELevel.h"

//! Interface for architecture dependent console output
/*!
This interface defines the console operations required by
synergy.  Each architecture must implement this interface.
*/
class IArchConsole : public IInterface {
public:
	//! @name manipulators
	//@{

	//! Open the console
	/*!
	Opens the console for writing.  The console is opened automatically
	on the first write so calling this method is optional.  Uses \c title
	for the console's title if appropriate for the architecture.  Calling
	this method on an already open console must have no effect.
	*/
	virtual void		openConsole(const char* title) = 0;

	//! Close the console
	/*!
	Close the console.  Calling this method on an already closed console
	must have no effect.
	*/
	virtual void		closeConsole() = 0;

	//! Show the console
	/*!
	Causes the console to become visible.  This generally only makes sense
	for a console in a graphical user interface.  Other implementations
	will do nothing.  Iff \p showIfEmpty is \c false then the implementation
	may optionally only show the console if it's not empty.
	*/
	virtual void		showConsole(bool showIfEmpty) = 0;

	//! Write to the console
	/*!
	Writes the given string to the console, opening it if necessary.
	*/
	virtual void		writeConsole(ELevel, const char*) = 0;

	//@}
};
