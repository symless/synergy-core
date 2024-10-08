/*
 * Deskflow -- mouse and keyboard sharing utility
 * Copyright (C) 2012-2016 Symless Ltd.
 * Copyright (C) 2011 Nick Bolton
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

#include "deskflow/KeyState.h"

#include <gmock/gmock.h>

class MockKeyMap;
class MockEventQueue;

// NOTE: do not mock methods that are not pure virtual. this mock exists only
// to provide an implementation of the KeyState abstract class.
class MockKeyState : public KeyState
{
public:
    MockKeyState(const MockEventQueue &eventQueue)
        : KeyState((IEventQueue *)&eventQueue, {"en"}, true)
    {
    }

    MockKeyState(const MockEventQueue &eventQueue, const deskflow::KeyMap &keyMap)
        : KeyState((IEventQueue *)&eventQueue, (deskflow::KeyMap &)keyMap, {"en"}, true)
    {
    }

    MOCK_METHOD(SInt32, pollActiveGroup, (), (const, override));
    MOCK_METHOD(KeyModifierMask, pollActiveModifiers, (), (const, override));
    MOCK_METHOD(bool, fakeCtrlAltDel, (), (override));
    MOCK_METHOD(void, getKeyMap, (deskflow::KeyMap &), (override));
    MOCK_METHOD(void, fakeKey, (const Keystroke &), (override));
    MOCK_METHOD(bool, fakeMediaKey, (KeyID), (override));
    MOCK_METHOD(void, pollPressedKeys, (KeyButtonSet &), (const, override));
};

typedef ::testing::NiceMock<MockKeyState> KeyStateImpl;

typedef UInt32 KeyID;

typedef void (*ForeachKeyCallback)(KeyID, SInt32 group, deskflow::KeyMap::KeyItem &, void *userData);
