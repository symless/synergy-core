/*
 * Deskflow -- mouse and keyboard sharing utility
 * Copyright (C) 2012 Symless Ltd.
 * Copyright (C) 2008 Volker Lanz (vl@fidra.de)
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

#include "ScreenSettingsDialog.h"

#include "gui/config/Screen.h"
#include "gui/styles.h"
#include "gui/validators/AliasValidator.h"
#include "gui/validators/ScreenNameValidator.h"
#include "gui/validators/ValidationError.h"

#include <QMessageBox>
#include <QtCore>
#include <QtGui>

using namespace deskflow::gui;
using enum ScreenConfig::Modifier;
using enum ScreenConfig::SwitchCorner;
using enum ScreenConfig::Fix;

ScreenSettingsDialog::ScreenSettingsDialog(QWidget *parent, Screen *pScreen, const ScreenList *pScreens)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
    , Ui::ScreenSettingsDialogBase()
    , m_pScreen(pScreen)
{
    setupUi(this);

    m_pLabelAliasError->setStyleSheet(kStyleErrorActiveLabel);
    m_pLabelNameError->setStyleSheet(kStyleErrorActiveLabel);

    m_pLineEditName->setText(m_pScreen->name());
    m_pLineEditName->setValidator(new validators::ScreenNameValidator(m_pLineEditName, new validators::ValidationError(this, m_pLabelNameError), pScreens));
    m_pLineEditName->selectAll();

    m_pLineEditAlias->setValidator(new validators::AliasValidator(m_pLineEditAlias, new validators::ValidationError(this, m_pLabelAliasError)));

    for (int i = 0; i < m_pScreen->aliases().count(); i++)
        new QListWidgetItem(m_pScreen->aliases()[i], m_pListAliases);

    m_pComboBoxShift->setCurrentIndex(m_pScreen->modifier(static_cast<int>(Shift)));
    m_pComboBoxCtrl->setCurrentIndex(m_pScreen->modifier(static_cast<int>(Ctrl)));
    m_pComboBoxAlt->setCurrentIndex(m_pScreen->modifier(static_cast<int>(Alt)));
    m_pComboBoxMeta->setCurrentIndex(m_pScreen->modifier(static_cast<int>(Meta)));
    m_pComboBoxSuper->setCurrentIndex(m_pScreen->modifier(static_cast<int>(Super)));

    m_pCheckBoxCornerTopLeft->setChecked(m_pScreen->switchCorner(static_cast<int>(TopLeft)));
    m_pCheckBoxCornerTopRight->setChecked(m_pScreen->switchCorner(static_cast<int>(TopRight)));
    m_pCheckBoxCornerBottomLeft->setChecked(m_pScreen->switchCorner(static_cast<int>(BottomLeft)));
    m_pCheckBoxCornerBottomRight->setChecked(m_pScreen->switchCorner(static_cast<int>(BottomRight)));
    m_pSpinBoxSwitchCornerSize->setValue(m_pScreen->switchCornerSize());

    m_pCheckBoxCapsLock->setChecked(m_pScreen->fix(CapsLock));
    m_pCheckBoxNumLock->setChecked(m_pScreen->fix(NumLock));
    m_pCheckBoxScrollLock->setChecked(m_pScreen->fix(ScrollLock));
    m_pCheckBoxXTest->setChecked(m_pScreen->fix(XTest));
}

void ScreenSettingsDialog::accept()
{
    if (m_pLineEditName->text().isEmpty()) {
        QMessageBox::warning(this,
                             tr("Screen name is empty"),
                             tr("The screen name cannot be empty. "
                                "Please either fill in a name or cancel the dialog."));
        return;
    } else if (!m_pLabelNameError->text().isEmpty()) {
        return;
    }

    m_pScreen->init();

    m_pScreen->setName(m_pLineEditName->text());

    for (int i = 0; i < m_pListAliases->count(); i++) {
        QString alias(m_pListAliases->item(i)->text());
        if (alias == m_pLineEditName->text()) {
            QMessageBox::warning(this,
                                 tr("Screen name matches alias"),
                                 tr("The screen name cannot be the same as an alias. "
                                    "Please either remove the alias or change the screen name."));
            return;
        }
        m_pScreen->addAlias(alias);
    }

    m_pScreen->setModifier(static_cast<int>(Shift), m_pComboBoxShift->currentIndex());
    m_pScreen->setModifier(static_cast<int>(Ctrl), m_pComboBoxCtrl->currentIndex());
    m_pScreen->setModifier(static_cast<int>(Alt), m_pComboBoxAlt->currentIndex());
    m_pScreen->setModifier(static_cast<int>(Meta), m_pComboBoxMeta->currentIndex());
    m_pScreen->setModifier(static_cast<int>(Super), m_pComboBoxSuper->currentIndex());

    m_pScreen->setSwitchCorner(static_cast<int>(TopLeft), m_pCheckBoxCornerTopLeft->isChecked());
    m_pScreen->setSwitchCorner(static_cast<int>(TopRight), m_pCheckBoxCornerTopRight->isChecked());
    m_pScreen->setSwitchCorner(static_cast<int>(BottomLeft), m_pCheckBoxCornerBottomLeft->isChecked());
    m_pScreen->setSwitchCorner(static_cast<int>(BottomRight), m_pCheckBoxCornerBottomRight->isChecked());
    m_pScreen->setSwitchCornerSize(m_pSpinBoxSwitchCornerSize->value());

    m_pScreen->setFix(static_cast<int>(CapsLock), m_pCheckBoxCapsLock->isChecked());
    m_pScreen->setFix(static_cast<int>(NumLock), m_pCheckBoxNumLock->isChecked());
    m_pScreen->setFix(static_cast<int>(ScrollLock), m_pCheckBoxScrollLock->isChecked());
    m_pScreen->setFix(static_cast<int>(XTest), m_pCheckBoxXTest->isChecked());

    QDialog::accept();
}

void ScreenSettingsDialog::on_m_pButtonAddAlias_clicked()
{
    if (!m_pLineEditAlias->text().isEmpty() && m_pListAliases->findItems(m_pLineEditAlias->text(), Qt::MatchFixedString).isEmpty()) {
        new QListWidgetItem(m_pLineEditAlias->text(), m_pListAliases);
        m_pLineEditAlias->clear();
    }
}

void ScreenSettingsDialog::on_m_pLineEditAlias_textChanged(const QString &text)
{
    m_pButtonAddAlias->setEnabled(!text.isEmpty() && m_pLabelAliasError->text().isEmpty());
}

void ScreenSettingsDialog::on_m_pButtonRemoveAlias_clicked()
{
    QList<QListWidgetItem *> items = m_pListAliases->selectedItems();

    for (int i = 0; i < items.count(); i++)
        delete items[i];
}

void ScreenSettingsDialog::on_m_pListAliases_itemSelectionChanged()
{
    m_pButtonRemoveAlias->setEnabled(!m_pListAliases->selectedItems().isEmpty());
}
