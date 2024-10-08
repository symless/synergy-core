/*
 * Deskflow -- mouse and keyboard sharing utility
 * Copyright (C) 2024 Symless Ltd.
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

#include "Config.h"

#include "base/Log.h"

#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#if HAVE_TOMLPLUSPLUS
#include <toml++/toml.hpp>
#endif

namespace deskflow {

Config::Config(const std::string &filename, const std::string &section)
    : m_filename(filename)
    , m_section(section)
{
}

const char *const *Config::argv() const
{
    return m_argv.data();
}

int Config::argc() const
{
    return static_cast<int>(m_argv.size());
}

bool Config::load(const std::string &firstArg)
{
#if HAVE_TOMLPLUSPLUS
    if (!firstArg.empty()) {
        m_args.push_back(firstArg);
    }

    if (m_filename.empty()) {
        throw NoConfigFilenameError();
    }

    if (!std::filesystem::exists(m_filename)) {
        LOG((CLOG_ERR "config file not found: %s", m_filename.c_str()));
        return false;
    }

    toml::table configTable;
    try {
        LOG((CLOG_INFO "loading config file: %s", m_filename.c_str()));
        configTable = toml::parse_file(m_filename);

    } catch (const toml::parse_error &err) {
        LOG((CLOG_ERR "toml parse error: %s", err.what()));
        throw ParseError();
    }

    if (!configTable.contains(m_section)) {
        LOG((CLOG_WARN "no %s section found in config file", m_section.c_str()));
        return false;
    }

    const auto &section = configTable[m_section];
    const auto args = section["args"];
    if (!args.is_table()) {
        LOG((CLOG_WARN "no args table found in config file"));
        return false;
    }

    std::string specialLastArg = "";
    const auto &table = *(args.as_table());
    for (const auto &pair : table) {
        const auto &key = pair.first;
        if (key.str() == "_last") {
            specialLastArg = pair.second.as_string()->get();
            continue;
        }

        m_args.push_back("--" + std::string(key.str()));

        if (pair.second.is_string()) {
            const auto value = pair.second.as_string()->get();
            m_args.push_back(value);
        }
    }

    if (!specialLastArg.empty()) {
        m_args.push_back(specialLastArg);
    }

    if (m_args.empty()) {
        LOG((CLOG_WARN "no args loaded from config file"));
        return false;
    }

    for (const auto &arg : m_args) {
        m_argv.push_back(arg.c_str());
    }

    return true;
#else
    LOG((CLOG_ERR "toml++ not available, config file not loaded"));
    return false;
#endif // HAVE_TOMLPLUSPLUS
}

} // namespace deskflow
