
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

#include "UninstallSettings.hh"
#include "FormatterUtils.hh"
#include <QLabel>
#include <QLabel>
#include <QSettings>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QGridLayout>

#include <paludis/dep_list.hh>

pertubis::UninstallSettings::UninstallSettings(QWidget *pobj) :
        m_deps(new QCheckBox(tr("with deps"),pobj)),
        m_unusedDeps(new QCheckBox(tr("with unused deps"),pobj)),
        m_allVersions(new QCheckBox(tr("all versions"),pobj)),
        m_unsafeUninstall(new QCheckBox(tr("ignore forward dependencies "),pobj))
{
    QGroupBox* group(new QGroupBox(tr("Deinstallation Settings"),pobj));
    setToolTip(tr("Options which are relevant for deinstallation."));
    m_deps->setToolTip( tr("Also uninstall packages that depend upon the target") );
    m_unusedDeps->setToolTip( tr("Also uninstall any dependencies of the target that are no longer used") );
    m_allVersions->setToolTip(tr("Uninstall all versions of a package"));
    m_unsafeUninstall->setToolTip( tr("Allow deinstallation of packagess, which are dependencies of other packages"));

    QGridLayout *installSettings = new QGridLayout;
    installSettings->addWidget(m_deps, 0, 0);
    installSettings->addWidget(m_unusedDeps, 0, 1);
    installSettings->addWidget(m_allVersions, 1, 0);
    installSettings->addWidget(m_unsafeUninstall, 1, 1);
    group->setLayout(installSettings);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(group);
    mainLayout->addSpacing(12);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
    loadSettings();
}

pertubis::UninstallSettings::~UninstallSettings()
{
    qDebug("UninstallSettings::~UninstallSettings() - start");
    saveSettings();
    qDebug("UninstallSettings::~UninstallSettings() - done");
}

void pertubis::UninstallSettings::setDefaults()
{
    m_deps->setChecked(true);
    m_unusedDeps->setChecked(true);
    m_allVersions->setChecked(true);
    m_unsafeUninstall->setChecked(true);
}

void pertubis::UninstallSettings::loadSettings()
{
    QSettings settings;
    settings.beginGroup( "UninstallSettings" );
        settings.setValue("deps",m_deps->checkState());
        settings.setValue("unusedDeps",m_unusedDeps->checkState());
        settings.setValue("allVersions",m_allVersions->checkState());
        settings.setValue("unsafeUninstall",m_unsafeUninstall->checkState());
    settings.endGroup();
}

void pertubis::UninstallSettings::saveSettings()
{
    QSettings settings;
    settings.beginGroup( "UninstallSettings" );
        m_deps->setChecked(settings.value("deps").toInt());
        m_unusedDeps->setChecked(settings.value("unusedDeps").toInt());
        m_allVersions->setChecked(settings.value("allVersions").toInt());
        m_unsafeUninstall->setChecked(settings.value("unsafeUninstall").toInt());
    settings.endGroup();
}
