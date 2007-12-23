
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

pertubis::UninstallSettingsModel::UninstallSettingsModel(QObject *pobj) :
        QObject(pobj),
        m_deps(false),
        m_unusedDeps(false),
        m_allVersions(false),
        m_unsafeUninstall(false)
{
    loadSettings();
}


pertubis::UninstallSettingsModel::~UninstallSettingsModel()
{
    saveSettings();
}

void pertubis::UninstallSettingsModel::loadSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "UninstallSettingsModel" );
    m_deps = settings.value("deps",false).toBool();
    m_unusedDeps = settings.value("unusedDeps",false).toBool();
    m_allVersions = settings.value("allVersions",false).toBool();
    m_unsafeUninstall = settings.value("unsafeUninstall",false).toBool();
    settings.endGroup();
}

void pertubis::UninstallSettingsModel::saveSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "UninstallSettingsModel" );
    settings.setValue("deps",m_deps);
    settings.setValue("unusedDeps",m_unusedDeps);
    settings.setValue("allVersions",m_allVersions);
    settings.setValue("unsafeUninstall",m_unsafeUninstall);
    settings.endGroup();
}

pertubis::UninstallSettingsView::UninstallSettingsView(QWidget *pobj,UninstallSettingsModel* model) :
        QWidget(pobj),
        m_model(model),
        m_deps(new QCheckBox(tr("with deps"),pobj)),
        m_unusedDeps(new QCheckBox(tr("with unused deps"),pobj)),
        m_allVersions(new QCheckBox(tr("all versions"),pobj)),
        m_unsafeUninstall(new QCheckBox(tr("ignore forward dependencies "),pobj))
{
    QGroupBox* group(new QGroupBox(tr("Deinstallation Settings"),pobj));
    setToolTip(tr("Options which are relevant for deinstallation."));
    m_deps->setToolTip( tr("Also uninstall packages that depend upon the target") );
    m_deps->setChecked(m_model->m_deps);
    m_unusedDeps->setToolTip( tr("Also uninstall any dependencies of the target that are no longer used") );
    m_unusedDeps->setChecked(m_model->m_unusedDeps);
    m_allVersions->setToolTip(tr("Uninstall all versions of a package"));
    m_allVersions->setChecked(m_model->m_allVersions);
    m_unsafeUninstall->setToolTip( tr("Allow deinstallation of packagess, which are dependencies of other packages"));
    m_unsafeUninstall->setChecked(m_model->m_unsafeUninstall);

    QGridLayout *groupLayout = new QGridLayout;
    groupLayout->addWidget(m_deps, 0, 0);
    groupLayout->addWidget(m_unusedDeps, 0, 1);
    groupLayout->addWidget(m_allVersions, 1, 0);
    groupLayout->addWidget(m_unsafeUninstall, 1, 1);
    group->setLayout(groupLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(group);
    mainLayout->addSpacing(12);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    connect(m_deps,
            SIGNAL(clicked(bool)),
            m_model,
            SLOT(onDepsChanged(bool)));

    connect(m_unusedDeps,
            SIGNAL(clicked(bool)),
            m_model,
            SLOT(onUnusedDepChanged(bool)));

    connect(m_allVersions,
            SIGNAL(clicked(bool)),
            m_model,
            SLOT(onAllVersionsChanged(bool)));

    connect(m_unsafeUninstall,
            SIGNAL(clicked(bool)),
            m_model,
            SLOT(onUnsafeUninstallChanged(bool)));

    connect(m_model,
            SIGNAL(depsChanged(bool)),
            m_deps,
            SLOT(setChecked(bool)));

    connect(m_model,
            SIGNAL(unusedDepsChanged(bool)),
            m_unusedDeps,
            SLOT(setChecked(bool)));

    connect(m_model,
            SIGNAL(allVersionsChanged(bool)),
            m_allVersions,
            SLOT(setChecked(bool)));

    connect(m_model,
            SIGNAL(unsafeUninstallChanged(bool)),
            m_unsafeUninstall,
            SLOT(setChecked(bool)));
}

pertubis::UninstallSettingsView::~UninstallSettingsView()
{
    qDebug("UninstallSettingsView::~UninstallSettingsView() - start");
    qDebug("UninstallSettingsView::~UninstallSettingsView() - done");
}
