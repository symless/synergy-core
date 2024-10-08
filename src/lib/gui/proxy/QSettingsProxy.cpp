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

#include "QSettingsProxy.h"

#include "common/constants.h"
#include "gui/Logger.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <memory>

namespace deskflow::gui::proxy {

const auto kLegacyOrgDomain = "http-symless-com";
const auto kLegacySystemConfigFilename = "SystemConfig.ini";

#if defined(Q_OS_UNIX)
const auto kUnixSystemConfigPath = "/usr/local/etc/";
#endif

//
// Free functions
//

/**
 * @brief The base dir for the system settings file.
 *
 * Important: Qt will append the org name as a dir, and the app name as the
 * settings filename, i.e.: `{base-dir}/Deskflow/Deskflow.ini`
 */
QString getSystemSettingsBaseDir()
{
#if defined(Q_OS_WIN)
    return QCoreApplication::applicationDirPath();
#elif defined(Q_OS_UNIX)
    // Qt already adds application and filename to the end of the path.
    // On macOS, it would be nice to use /Library dir, but qt has no elevate
    // system.
    return kUnixSystemConfigPath;
#else
#error "unsupported platform"
#endif
}

void migrateLegacySystemSettings(QSettings &settings)
{
    if (QFile(settings.fileName()).exists()) {
        qDebug("system settings already exist, skipping migration");
        return;
    }

    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, kLegacySystemConfigFilename);
    QSettings oldSystemSettings(QSettings::IniFormat, QSettings::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());

    if (QFile(oldSystemSettings.fileName()).exists()) {
        for (const auto &key : oldSystemSettings.allKeys()) {
            settings.setValue(key, oldSystemSettings.value(key));
        }
    }

    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, getSystemSettingsBaseDir());
}

void migrateLegacyUserSettings(QSettings &newSettings)
{
    QString newPath = newSettings.fileName();
    QFile newFile(newPath);
    if (newFile.exists()) {
        qInfo("user settings already exist, skipping migration");
        return;
    }

    QSettings oldSettings(kLegacyOrgDomain, kAppName);
    QString oldPath = oldSettings.fileName();

    if (oldPath.isEmpty()) {
        qInfo("no legacy settings to migrate, filename empty");
        return;
    }

    if (!QFile(oldPath).exists()) {
        qInfo("no legacy settings to migrate, file does not exist");
        return;
    }

    QFileInfo oldFileInfo(oldPath);
    QFileInfo newFileInfo(newPath);

    qDebug("migrating legacy settings: '%s' -> '%s'", //
           qPrintable(oldFileInfo.fileName()),
           qPrintable(newFileInfo.fileName()));

    QStringList keys = oldSettings.allKeys();
    for (const QString &key : keys) {
        QVariant oldValue = oldSettings.value(key);
        newSettings.setValue(key, oldValue);
        logVerbose(QString("migrating setting '%1' = '%2'").arg(key, oldValue.toString()));
    }

    newSettings.sync();
}

//
// QSettingsProxy
//

void QSettingsProxy::loadUser()
{
    m_pSettings = std::make_unique<QSettings>();

#if defined(Q_OS_MAC)
    // on mac, we used to save settings to "com.http-symless-com.Deskflow.plist"
    // because `setOrganizationName` was historically called using a url instead
    // of an actual domain (e.g. deskflow.org).
    migrateLegacyUserSettings(*m_pSettings);
#endif // Q_OS_MAC
}

void QSettingsProxy::loadSystem()
{
    auto orgName = QCoreApplication::organizationName();
    if (orgName.isEmpty()) {
        qFatal("unable to load config, organization name is empty");
        return;
    } else {
        qDebug() << "org name for config:" << orgName;
    }

    auto appName = QCoreApplication::applicationName();
    if (appName.isEmpty()) {
        qFatal("unable to load config, application name is empty");
        return;
    } else {
        qDebug() << "app name for config:" << appName;
    }

    QSettings::setPath(QSettings::Format::IniFormat, QSettings::Scope::SystemScope, getSystemSettingsBaseDir());

    m_pSettings = std::make_unique<QSettings>(QSettings::Format::IniFormat, QSettings::Scope::SystemScope, orgName, appName);

#if defined(Q_OS_WIN)
    migrateLegacySystemSettings(*m_pSettings);
#endif // Q_OS_WIN
}

int QSettingsProxy::beginReadArray(const QString &prefix)
{
    return m_pSettings->beginReadArray(prefix);
}

void QSettingsProxy::setArrayIndex(int i)
{
    m_pSettings->setArrayIndex(i);
}

QVariant QSettingsProxy::value(const QString &key) const
{
    return m_pSettings->value(key);
}

QVariant QSettingsProxy::value(const QString &key, const QVariant &defaultValue) const
{
    return m_pSettings->value(key, defaultValue);
}

void QSettingsProxy::endArray()
{
    m_pSettings->endArray();
}

void QSettingsProxy::beginWriteArray(const QString &prefix)
{
    m_pSettings->beginWriteArray(prefix);
}

void QSettingsProxy::setValue(const QString &key, const QVariant &value)
{
    m_pSettings->setValue(key, value);
}

void QSettingsProxy::beginGroup(const QString &prefix)
{
    m_pSettings->beginGroup(prefix);
}

void QSettingsProxy::remove(const QString &key)
{
    m_pSettings->remove(key);
}

void QSettingsProxy::endGroup()
{
    m_pSettings->endGroup();
}

bool QSettingsProxy::isWritable() const
{
    return m_pSettings->isWritable();
}

bool QSettingsProxy::contains(const QString &key) const
{
    return m_pSettings->contains(key);
}

} // namespace deskflow::gui::proxy
