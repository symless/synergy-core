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

#include "synergy/IClipboard.h"

//! Memory buffer clipboard
/*!
This class implements a clipboard that stores data in memory.
*/
class Clipboard : public IClipboard {
public:
	Clipboard();
	virtual ~Clipboard();

	//! @name manipulators
	//@{

	//! Unmarshall clipboard data
	/*!
	Extract marshalled clipboard data and store it in this clipboard.
	Sets the clipboard time to \c time.
	*/
	void				unmarshall(const String& data, Time time);

	//@}
	//! @name accessors
	//@{

	//! Marshall clipboard data
	/*!
	Merge this clipboard's data into a single buffer that can be later
	unmarshalled to restore the clipboard and return the buffer.
	*/
	String				marshall() const;

	//@}

	// IClipboard overrides
	virtual bool		empty();
	virtual void		add(EFormat, const String& data);
	virtual bool		open(Time) const;
	virtual void		close() const;
	virtual Time		getTime() const;
	virtual bool		has(EFormat) const;
	virtual String		get(EFormat) const;

private:
	mutable bool		m_open;
	mutable Time		m_time;
	bool				m_owner;
	Time				m_timeOwned;
	bool				m_added[kNumFormats];
	String				m_data[kNumFormats];
};
