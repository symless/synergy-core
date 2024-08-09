/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2020 Symless Ltd.
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

#include "ConfigScopes.h"
#include "Logger.h"
#include "common/constants.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QtCore/qfileinfo.h>
#include <QtCore/qlogging.h>
#include <memory>

const auto kSystemConfigFilename = "SystemConfig.ini";
const auto kLegacyOrgDomain = "http-symless-com";

#if defined(Q_OS_UNIX)
const auto kUnixSystemConfigPath = "/usr/local/etc/symless/";
#endif

namespace synergy::gui {

using namespace proxy;

QString getSystemSettingPath() {
  const QString settingFilename(kSystemConfigFilename);
#if defined(Q_OS_WIN)
  return QCoreApplication::applicationDirPath() + QDir::separator();
#elif defined(Q_OS_MAC)
  // it would be nice to use /Library dir, but qt has no elevate system.
  return kUnixSystemConfigPath + settingFilename;
#elif defined(Q_OS_LINUX)
  // qt already adds application and filename to the end of the path on linux.
  return kUnixSystemConfigPath;
#else
#error "unsupported platform"
#endif
}

void migrateLegacySystemSettings(QSettings &settings) {
  if (QFile(settings.fileName()).exists()) {
    qDebug("system settings already exist, skipping migration");
    return;
  }

  QSettings::setPath(
      QSettings::IniFormat, QSettings::SystemScope, kSystemConfigFilename);
  QSettings oldSystemSettings(
      QSettings::IniFormat, QSettings::SystemScope,
      QCoreApplication::organizationName(),
      QCoreApplication::applicationName());

  if (QFile(oldSystemSettings.fileName()).exists()) {
    for (const auto &key : oldSystemSettings.allKeys()) {
      settings.setValue(key, oldSystemSettings.value(key));
    }
  }

  QSettings::setPath(
      QSettings::IniFormat, QSettings::SystemScope, getSystemSettingPath());
}

void migrateLegacyUserSettings(QSettings &newSettings) {
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

  qDebug(
      "migrating legacy settings: '%s' -> '%s'", //
      qPrintable(oldFileInfo.fileName()), qPrintable(newFileInfo.fileName()));

  QStringList keys = oldSettings.allKeys();
  for (const QString &key : keys) {
    QVariant oldValue = oldSettings.value(key);
    newSettings.setValue(key, oldValue);
    logVerbose(
        QString("migrating setting '%1' = '%2'").arg(key, oldValue.toString()));
  }

  newSettings.sync();
}

ConfigScopes::ConfigScopes() {
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

  // default to user scope.
  // if we set the scope specifically then we also have to set the application
  // name and the organisation name which breaks backwards compatibility.
  m_pUserSettings = std::make_unique<QSettings>();
  m_userSettingsProxy.set(*m_pUserSettings);

#if defined(Q_OS_MAC)
  // on mac, we used to save settings to "com.http-symless-com.Synergy.plist"
  // because `setOrganizationName` was historically called using a url instead
  // of an actual domain (e.g. symless.com).
  migrateLegacyUserSettings(*m_pUserSettings);
#endif // Q_OS_MAC

  QSettings::setPath(
      QSettings::Format::IniFormat, QSettings::Scope::SystemScope,
      getSystemSettingPath());

  // Config will default to User settings if they exist,
  //  otherwise it will load System setting and save them to User settings
  m_pSystemSettings = std::make_unique<QSettings>(
      QSettings::Format::IniFormat, QSettings::Scope::SystemScope, orgName,
      appName);
  m_systemSettingsProxy.set(*m_pSystemSettings);

#if defined(Q_OS_WIN)
  migrateLegacySystemSettings(*m_pSystemSettings);
#endif
}

void ConfigScopes::signalReady() { emit ready(); }

void ConfigScopes::save() {
  qDebug("emitting config saving signal");
  emit saving();

  qDebug("writing config to filesystem");
  m_pUserSettings->sync();
  m_pSystemSettings->sync();
}

bool ConfigScopes::isActiveScopeWritable() const {
  return activeSettings().isWritable();
}

void ConfigScopes::setActiveScope(ConfigScopes::Scope scope) {
  m_currentScope = scope;
}

ConfigScopes::Scope ConfigScopes::activeScope() const { return m_currentScope; }

bool ConfigScopes::scopeContains(const QString &name, Scope scope) const {
  switch (scope) {
  case Scope::User:
    return m_pUserSettings->contains(name);
  case Scope::System:
    return m_pSystemSettings->contains(name);
  default:
    return activeSettings().contains(name);
  }
}

QSettingsProxy &ConfigScopes::activeSettings() {
  if (m_currentScope == Scope::User) {
    return m_userSettingsProxy;
  } else {
    return m_systemSettingsProxy;
  }
}

const QSettingsProxy &ConfigScopes::activeSettings() const {
  if (m_currentScope == Scope::User) {
    return m_userSettingsProxy;
  } else {
    return m_systemSettingsProxy;
  }
}

QString ConfigScopes::activeFilePath() const {
  return activeSettings().fileName();
}

QVariant ConfigScopes::getFromScope(
    const QString &name, const QVariant &defaultValue, Scope scope) const {
  switch (scope) {
  case Scope::User:
    return m_pUserSettings->value(name, defaultValue);
  case Scope::System:
    return m_pSystemSettings->value(name, defaultValue);
  default:
    return activeSettings().value(name, defaultValue);
  }
}

void ConfigScopes::setInScope(
    const QString &name, const QVariant &value, Scope scope) {
  switch (scope) {
  case Scope::User:
    m_pUserSettings->setValue(name, value);
    break;
  case Scope::System:
    m_pSystemSettings->setValue(name, value);
    break;
  default:
    activeSettings().setValue(name, value);
    break;
  }
}

} // namespace synergy::gui
