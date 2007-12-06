
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of pertubis
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "InstallSettings.hh"
#include "FormatterUtils.hh"
#include <QGroupBox>
#include <QLabel>
#include <QLabel>
#include <QSettings>
#include <QComboBox>
#include <QCheckBox>
#include <QGridLayout>

#include <paludis/dep_list.hh>

pertubis::InstallSettings::InstallSettings(QWidget *pobj) :
        QWidget(pobj),
        m_debug(new QCheckBox(tr("debug"),pobj)),
        m_fetch(new QCheckBox(tr("fetch"),pobj)),
        m_noConfigProtection(new QCheckBox(tr("no-config-protection"),pobj)),
        m_noSafeResume(new QCheckBox(tr("no-safe-resume"),pobj)),
        m_pretend(new QCheckBox(tr("pretend"),pobj)),
        m_continueOnFailure(new QComboBox(pobj))
{
    QGroupBox* group(new QGroupBox(tr("Installation Settings"),pobj));
    setToolTip(tr("Options which are relevant for installation and uninstallation"));
    m_debug->setToolTip( tr("debug build") );
    m_fetch->setToolTip( tr("Only fetch sources; don't install anything") );
    m_noConfigProtection->setToolTip(tr("Disable config file protection (dangerous)"));
    m_noSafeResume->setToolTip( tr("Do not allow interrupted downloads to be resumed"));
    m_pretend->setToolTip( tr("Pretend only") );
    m_continueOnFailure->setToolTip(tr("Whether to continue after a fetch or install error"));

    m_continueOnFailure->addItem(tr("If fetching only"));
    m_continueOnFailure->addItem(tr("Never"));
    m_continueOnFailure->addItem(tr("If remaining packages' dependencies are satisfied"));
    m_continueOnFailure->addItem(tr("If independent of failed and skipped packages"));
    m_continueOnFailure->addItem(tr("Always (UNSAFE)"));
    m_continueOnFailure->setCurrentIndex(0);

    QGridLayout *groupLayout = new QGridLayout;
    groupLayout->addWidget(m_debug, 0, 0);
    groupLayout->addWidget(m_fetch, 0, 1);
    groupLayout->addWidget(m_noConfigProtection, 1, 0);
    groupLayout->addWidget(m_noSafeResume, 1, 1);
    groupLayout->addWidget(m_pretend, 2, 0);
    groupLayout->addWidget(new QLabel(tr("continue on failure"),this), 3, 0);
    groupLayout->addWidget(m_continueOnFailure, 3, 1);
    group->setLayout(groupLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(group);
    mainLayout->addSpacing(12);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
    loadSettings();
}

pertubis::InstallSettings::~InstallSettings()
{
    qDebug("InstallSettings::~InstallSettings() - start");
    saveSettings();
    qDebug("InstallSettings::~InstallSettings() - done");
}

void pertubis::InstallSettings::loadSettings()
{
    QSettings settings;
    settings.beginGroup( "InstallSettings" );
    settings.endGroup();
}

void pertubis::InstallSettings::saveSettings()
{
    QSettings settings;
    settings.beginGroup( "InstallSettings" );
    settings.endGroup();
}
