/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2022 Symless Ltd.
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

#include <string>
#include "SerialKeyType.h"
#include "SerialKeyEdition.h"

/**
 * @brief The SerialKeyData struct
 * This is DTO which stores key data
 */
struct SerialKeyData {
    std::string           m_key;
    std::string           m_name;
    std::string           m_email;
    std::string           m_company;
    SerialKeyEdition      m_edition;
    SerialKeyType         m_keyType;
    unsigned              m_userLimit  = 1;
    unsigned long long    m_warnTime   = 0;
    unsigned long long    m_expireTime = 0;

    SerialKeyData(const std::string& key) :
        m_key(key)
    {
    }
};
