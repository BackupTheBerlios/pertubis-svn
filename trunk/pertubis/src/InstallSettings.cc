
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

// #include "ArgumentViewCreator.hh"
#include "InstallSettings.hh"
#include "FormatterUtils.hh"
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <sstream>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/visitor-impl.hh>
#include <paludis/install_task.hh>
#include <paludis/dep_list.hh>
#include <paludis/action-fwd.hh>

pertubis::InstallSettingsModel::InstallSettingsModel(QObject *pobj) :
        QObject(pobj)
{
    loadSettings();
}

pertubis::InstallSettingsModel::~InstallSettingsModel()
{
    saveSettings();
}

void pertubis::InstallSettingsModel::populate_install_task(const paludis::Environment * /*env*/, paludis::InstallTask &task) const
{
    task.set_no_config_protect(m_config);
    task.set_fetch_only(m_fetch);
    task.set_pretend(m_pretend);
    task.set_preserve_world(m_preserve);
    task.set_safe_resume(m_noSafeResume);

    if ( ! m_worldSpec.isEmpty() )
        task.set_add_to_world_spec(m_worldSpec.toStdString());

    if (m_debug == 0)
        task.set_debug_mode(paludis::iado_none);
    if (m_debug == 1)
        task.set_debug_mode(paludis::iado_split);
    if (m_debug == 2)
        task.set_debug_mode(paludis::iado_internal);
    if (m_checks == 0)
        task.set_checks_mode(paludis::iaco_none);
    if (m_checks == 0)
        task.set_checks_mode(paludis::iaco_default);
    if (m_checks == 0)
        task.set_checks_mode(paludis::iaco_always);

    switch (m_continueOnFailure)
    {
        case 0:
            task.set_continue_on_failure(paludis::itcof_if_fetch_only);
            break;
        case 1:
            task.set_continue_on_failure(paludis::itcof_never);
            break;
        case 2:
            task.set_continue_on_failure(paludis::itcof_if_satisfied);
            break;
        case 3:
            task.set_continue_on_failure(paludis::itcof_if_independent);
            break;
        case 4:
            task.set_continue_on_failure(paludis::itcof_always);
            break;
        default:
            throw;
    }
}

void pertubis::InstallSettingsModel::loadSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "InstallSettingsModel" );
    m_debug = settings.value("debug_build",1).toInt();
    m_fetch = settings.value("fetch",false).toBool();
    m_config = settings.value("no_config_protect",false).toBool();
    m_noSafeResume = settings.value("no_safe_resume",false).toBool();
    m_pretend = settings.value("pretend",false).toBool();
    m_continueOnFailure  = settings.value("continue_on_failure",0).toInt();
    m_checks = settings.value("checks",1).toInt();
    settings.endGroup();
}

void pertubis::InstallSettingsModel::saveSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "InstallSettingsModel" );
    settings.setValue("debug",m_debug);
    settings.setValue("fetch",m_fetch);
    settings.setValue("no_config_protect",m_config);
    settings.setValue("no_safe_resume",m_noSafeResume);
    settings.setValue("pretend",m_pretend);
    settings.setValue("continue_on_failure",m_continueOnFailure);
    settings.setValue("checks",m_checks);
    settings.endGroup();
}

pertubis::InstallSettingsView::InstallSettingsView(QWidget *pobj,InstallSettingsModel* model) :
        QWidget(pobj),
        m_model(model),
        m_debug(new QComboBox(pobj)),
        m_fetch(new QCheckBox(tr("fetch"),pobj)),
        m_noConfigProtection(new QCheckBox(tr("no-config-protection"),pobj)),
        m_noSafeResume(new QCheckBox(tr("no-safe-resume"),pobj)),
        m_pretend(new QCheckBox(tr("pretend"),pobj)),
        m_continueOnFailure(new QComboBox(pobj)),
        m_checks(new QComboBox(pobj)),
        m_worldSpec(new QLineEdit(pobj))
{
    QGroupBox* group(new QGroupBox(tr("Installation Settings"),pobj));

    m_debug->setToolTip( tr("debug build") );
    m_debug->addItem( tr("none") );
    m_debug->addItem( tr("split") );
    m_debug->addItem( tr("internal") );
    m_debug->setCurrentIndex(m_model->m_debug);
    m_fetch->setToolTip( tr("Only fetch sources; don't install anything") );
    m_fetch->setChecked(m_model->m_fetch);
    m_noConfigProtection->setToolTip(tr("Disable config file protection (dangerous)"));
    m_noConfigProtection->setChecked(m_model->m_config);
    m_noSafeResume->setToolTip( tr("Do not allow interrupted downloads to be resumed"));
    m_noSafeResume->setChecked(m_model->m_noSafeResume);
    m_pretend->setToolTip( tr("Pretend only") );
    m_pretend->setChecked(m_model->m_pretend);
    m_continueOnFailure->setToolTip(tr("Whether to continue after a fetch or install error"));

    m_checks->addItem("none");
    m_checks->addItem("default");
    m_checks->addItem("always");

    m_continueOnFailure->addItem("if-fetch-only");
    m_continueOnFailure->addItem("never");
    m_continueOnFailure->addItem("if-satisfied");
    m_continueOnFailure->addItem("if-independent");
    m_continueOnFailure->addItem("always");
    m_continueOnFailure->setCurrentIndex(m_model->m_continueOnFailure);

    QGridLayout *groupLayout = new QGridLayout;
    groupLayout->addWidget(m_pretend, 0, 0);
    groupLayout->addWidget(m_fetch, 1, 0);
    groupLayout->addWidget(m_noConfigProtection, 2, 0);
    groupLayout->addWidget(m_noSafeResume, 3, 0);
    groupLayout->addWidget(new QLabel(tr("debug"),m_debug), 4, 0);
    groupLayout->addWidget(m_debug, 4, 1);
    groupLayout->addWidget(new QLabel(tr("continue on failure"),m_continueOnFailure), 5, 0);
    groupLayout->addWidget(m_continueOnFailure, 5, 1);
    groupLayout->addWidget(new QLabel(tr("checks"),m_checks), 6, 0);
    groupLayout->addWidget(m_checks, 6, 1);
    groupLayout->addWidget(new QLabel(tr("world spec"),m_checks), 7, 0);
    groupLayout->addWidget(m_worldSpec, 7, 1);

    group->setLayout(groupLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(group);
    mainLayout->addSpacing(12);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    connect(m_debug,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeDebug(int)));

    connect(m_fetch,
            SIGNAL(clicked(bool)),
            m_model,
            SLOT(changeFetch(bool)));

    connect(m_noConfigProtection,
            SIGNAL(clicked(bool)),
            m_model,
            SLOT(changeNoConfig(bool)));

    connect(m_noSafeResume,
            SIGNAL(clicked(bool)),
            m_model,
            SLOT(changeNoSafe(bool)));

    connect(m_pretend,
            SIGNAL(clicked(bool)),
            m_model,
            SLOT(changePretend(bool)));

    connect(m_continueOnFailure,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeContinue(int)));

    connect(m_worldSpec,
            SIGNAL(textChanged(QString)),
            m_model,
            SLOT(changeWorldSpec(QString)));

    connect(m_model,
            SIGNAL(debugChanged(int)),
            m_debug,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(fetchChanged(bool)),
            m_fetch,
            SLOT(setChecked(bool)));

    connect(m_model,
            SIGNAL(noConfigChanged(bool)),
            m_noConfigProtection,
            SLOT(setChecked(bool)));

    connect(m_model,
            SIGNAL(noSafeChanged(bool)),
            m_noSafeResume,
            SLOT(setChecked(bool)));

    connect(m_model,
            SIGNAL(pretendChanged(bool)),
            m_pretend,
            SLOT(setChecked(bool)));

    connect(m_model,
            SIGNAL(continueChanged(int)),
            m_continueOnFailure,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(worldSpecChanged(const QString &)),
            m_worldSpec,
            SLOT(setText(const QString &)));
}

pertubis::InstallSettingsView::~InstallSettingsView()
{
}
