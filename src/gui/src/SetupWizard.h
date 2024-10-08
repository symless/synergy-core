/*
 * Deskflow -- mouse and keyboard sharing utility
 * Copyright (C) 2012 Symless Ltd.
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

#include "ui_SetupWizardBase.h"

#include "gui/config/AppConfig.h"

#include <QDialog>
#include <QObject>

class MainWindow;

class SetupWizard : public QDialog, public Ui::SetupWizardBase
{
    Q_OBJECT

public:
    explicit SetupWizard(AppConfig &appConfig);

protected:
    void accept();
    void reject();

private:
    AppConfig &m_appConfig;

public slots:
    void onLineEditNameChanged(const QString &error);
};
