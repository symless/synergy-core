/*
 * Deskflow -- mouse and keyboard sharing utility
 * Copyright (C) 2012-2016 Symless Ltd.
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
 */

#pragma once

#include "base/IEventQueue.h"
#include "io/IStream.h"

//! A stream filter
/*!
This class wraps a stream.  Subclasses provide indirect access
to the wrapped stream, typically performing some filtering.
*/
class StreamFilter : public deskflow::IStream
{
public:
    /*!
    Create a wrapper around \c stream.  Iff \c adoptStream is true then
    this object takes ownership of the stream and will delete it in the
    d'tor.
    */
    StreamFilter(IEventQueue *events, deskflow::IStream *stream, bool adoptStream = true);
    StreamFilter(StreamFilter const &) = delete;
    StreamFilter(StreamFilter &&) = delete;
    virtual ~StreamFilter();

    StreamFilter &operator=(StreamFilter const &) = delete;
    StreamFilter &operator=(StreamFilter &&) = delete;

    // IStream overrides
    // These all just forward to the underlying stream except getEventTarget.
    // Override as necessary.  getEventTarget returns a pointer to this.
    virtual void close();
    virtual UInt32 read(void *buffer, UInt32 n);
    virtual void write(const void *buffer, UInt32 n);
    virtual void flush();
    virtual void shutdownInput();
    virtual void shutdownOutput();
    virtual void *getEventTarget() const;
    virtual bool isReady() const;
    virtual UInt32 getSize() const;

    //! Get the stream
    /*!
    Returns the stream passed to the c'tor.
    */
    deskflow::IStream *getStream() const;

protected:
    //! Handle events from source stream
    /*!
    Does the event filtering.  The default simply dispatches an event
    identical except using this object as the event target.
    */
    virtual void filterEvent(const Event &);

private:
    void handleUpstreamEvent(const Event &, void *);

private:
    deskflow::IStream *m_stream;
    bool m_adopted;
    IEventQueue *m_events;
};
