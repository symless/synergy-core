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

#include "synergy/Clipboard.h"

//
// Clipboard
//

Clipboard::Clipboard() :
	m_open(false),
	m_owner(false)
{
	open(0);
	empty();
	close();
}

Clipboard::~Clipboard()
{
	// do nothing
}

bool
Clipboard::empty()
{
	assert(m_open);

	// clear all data
	for (SInt32 index = 0; index < kNumFormats; ++index) {
		m_data[index]  = "";
		m_added[index] = false;
	}

	// save time
	m_timeOwned = m_time;

	// we're the owner now
	m_owner = true;

	return true;
}

void
Clipboard::add(EFormat format, const String& data)
{
	assert(m_open);
	assert(m_owner);

	m_data[format]  = data;
	m_added[format] = true;
}

bool
Clipboard::open(Time time) const
{
	assert(!m_open);

	m_open = true;
	m_time = time;

	return true;
}

void
Clipboard::close() const
{
	assert(m_open);

	m_open = false;
}

Clipboard::Time
Clipboard::getTime() const
{
	return m_timeOwned;
}

bool
Clipboard::has(EFormat format) const
{
	assert(m_open);
	return m_added[format];
}

String
Clipboard::get(EFormat format) const
{
	assert(m_open);
	return m_data[format];
}

void
Clipboard::unmarshall(const String& data, Time time)
{
	IClipboard::unmarshall(this, data, time);
}

String
Clipboard::marshall() const
{
	return IClipboard::marshall(this);
}
