/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2015 Symless Ltd.
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

#include "gui/CoreInterface.h"

#include <QObject>

class TlsCertificate : public QObject {
  Q_OBJECT

public:
  explicit TlsCertificate(QObject *parent = nullptr);

  void generateCertificate(
      const QString &path = QString(), const QString &keyLength = "2048",
      bool forceGen = false);
  QString getCertKeyLength(const QString &path);

private:
  bool runTool(const QStringList &args);
  void generateFingerprint(const QString &certificateFilename);

private:
  QString m_profileDir;
  QString m_toolStdout;
  CoreInterface m_coreInterface;
};
