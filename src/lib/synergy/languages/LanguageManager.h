/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2014-2021 Symless Ltd.
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
#ifndef SYNERGY_LANGUAGESLANGUAGEMANAGER_H
#define SYNERGY_LANGUAGESLANGUAGEMANAGER_H

#include "base/String.h"
#include <vector>

namespace synergy {

namespace languages {

class LanguageManager
{
    std::vector<String> m_remoteLanguages;
    std::vector<String> m_localLanguages;

public:

    /**
     * @brief setRemoteLanguages sets remote languages
     * @param remoteLanguages is a string with sericalized languages
     */
    void setRemoteLanguages(const String& remoteLanguages);

    /**
     * @brief getRemoteLanguages getter for remote languages
     * @return vector of remote languages
     */
    const std::vector<String>& getRemoteLanguages() const;

    /**
     * @brief setLocalLanguages setter for local languages
     * @param localLanguages is a vector of local languages
     */
    void setLocalLanguages(const std::vector<String>& localLanguages);

    /**
     * @brief getLocalLanguages getter for local languages
     * @return vector of local languages
     */
    const std::vector<String>& getLocalLanguages() const;

    /**
     * @brief getMissedLanguages getter for missed languages on local machine
     * @return difference between remote and local languages as a coma separated string
     */
    String getMissedLanguages() const;

    /**
     * @brief getSerializedLocalLanguages getter for local serialized languages
     * @return serialized local languages as a string
     */
    String getSerializedLocalLanguages() const;
};

} //namespace languages

} //namespace synergy

#endif // SYNERGY_LANGUAGESLANGUAGEMANAGER_H
