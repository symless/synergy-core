/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2012 Synergy Si Ltd.
 * Copyright (C) 2004 Chris Schoeneman
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

#include "platform/OSXClipboard.h"

//! Convert to/from some text encoding
class OSXClipboardAnyTextConverter : public IOSXClipboardConverter {
public:
	OSXClipboardAnyTextConverter();
	virtual ~OSXClipboardAnyTextConverter();

	// IOSXClipboardConverter overrides
	virtual IClipboard::EFormat
						getFormat() const;
	virtual CFStringRef
						getOSXFormat() const = 0;
	virtual String		fromIClipboard(const String &) const;
	virtual String		toIClipboard(const String &) const;

protected:
	//! Convert from IClipboard format
	/*!
	Do UTF-8 conversion and linefeed conversion.
	*/
	virtual String		doFromIClipboard(const String&) const = 0;

	//! Convert to IClipboard format
	/*!
	Do UTF-8 conversion and Linefeed conversion.
	*/
	virtual String		doToIClipboard(const String&) const = 0;

private:
	static String		convertLinefeedToMacOS(const String&);
	static String		convertLinefeedToUnix(const String&);
};
