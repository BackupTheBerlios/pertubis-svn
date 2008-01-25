
/* Copyright (C) 2007-2008 Stefan Koegl <hotshelf@users.berlios.de>
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
    qDebug() << "pertubis::InstallSettingsModel::~InstallSettingsModel()";
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
    m_preserve = settings.value("preserve",false).toBool();
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
    settings.setValue("preserve",m_preserve);
    settings.setValue("continue_on_failure",m_continueOnFailure);
    settings.setValue("checks",m_checks);
    settings.endGroup();
}

pertubis::InstallSettingsView::InstallSettingsView(QWidget *pobj,InstallSettingsModel* model) :
        QWidget(pobj),
        m_model(model)
{
    QCheckBox* fetch(new QCheckBox(tr("fetch"),pobj));
    fetch->setToolTip( tr("Only fetch sources; don't install anything") );
    fetch->setChecked(model->m_fetch);

    QCheckBox* noConfigProtection(new QCheckBox(tr("no-config-protection"),pobj));
    noConfigProtection->setToolTip(tr("Disable config file protection (dangerous)"));
    noConfigProtection->setChecked(model->m_config);
    QCheckBox* noSafeResume(new QCheckBox(tr("no-safe-resume"),pobj));
    noSafeResume->setToolTip( tr("Do not allow interrupted downloads to be resumed"));
    noSafeResume->setChecked(model->m_noSafeResume);

    QCheckBox* pretend(new QCheckBox(tr("pretend"),pobj));
    pretend->setToolTip( tr("Pretend only") );
    pretend->setChecked(model->m_pretend);

    QCheckBox* preserveWorld(new QCheckBox(tr("preserve world"),pobj));
    preserveWorld->setToolTip( tr("Whether to preserve the world file when (un)installing packages") );
    preserveWorld->setChecked(model->m_preserve);

    QComboBox* checks(new QComboBox(pobj));
    checks->addItem("none");
    checks->addItem("default");
    checks->addItem("always");

    QComboBox* debug(new QComboBox(pobj));
    debug->setToolTip( tr("debug build") );
    debug->addItem( tr("none") );
    debug->addItem( tr("split") );
    debug->addItem( tr("internal") );
    debug->setCurrentIndex(model->m_debug);

    QComboBox* continueOnFailure(new QComboBox(pobj));
    continueOnFailure->setToolTip(tr("Whether to continue after a fetch or install error"));
    continueOnFailure->addItem("if-fetch-only");
    continueOnFailure->addItem("never");
    continueOnFailure->addItem("if-satisfied");
    continueOnFailure->addItem("if-independent");
    continueOnFailure->addItem("always");
    continueOnFailure->setCurrentIndex(model->m_continueOnFailure);

    QLineEdit* worldSpec(new QLineEdit(pobj));

    QGridLayout *groupLayout = new QGridLayout;
    groupLayout->addWidget(pretend, 0, 0);
    groupLayout->addWidget(fetch, 1, 0);
    groupLayout->addWidget(noConfigProtection, 2, 0);
    groupLayout->addWidget(noSafeResume, 3, 0);
    groupLayout->addWidget(preserveWorld, 4, 0);
    groupLayout->addWidget(new QLabel(tr("debug"),debug), 5, 0);
    groupLayout->addWidget(debug, 5, 1);
    groupLayout->addWidget(new QLabel(tr("continue on failure"),continueOnFailure), 6, 0);
    groupLayout->addWidget(continueOnFailure, 6, 1);
    groupLayout->addWidget(new QLabel(tr("checks"),checks), 7, 0);
    groupLayout->addWidget(checks, 7, 1);
    groupLayout->addWidget(new QLabel(tr("world spec"),checks), 8, 0);
    groupLayout->addWidget(worldSpec, 8, 1);

    QGroupBox* group(new QGroupBox(tr("Installation Settings"),pobj));
    group->setLayout(groupLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(group);
    mainLayout->addSpacing(12);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    connect(debug,
            SIGNAL(currentIndexChanged(int)),
            model,
            SLOT(changeDebug(int)));

    connect(fetch,
            SIGNAL(clicked(bool)),
            model,
            SLOT(changeFetch(bool)));

    connect(noConfigProtection,
            SIGNAL(clicked(bool)),
            model,
            SLOT(changeNoConfig(bool)));

    connect(noSafeResume,
            SIGNAL(clicked(bool)),
            model,
            SLOT(changeNoSafe(bool)));

    connect(pretend,
            SIGNAL(clicked(bool)),
            model,
            SLOT(changePretend(bool)));

    connect(continueOnFailure,
            SIGNAL(currentIndexChanged(int)),
            model,
            SLOT(changeContinue(int)));

    connect(worldSpec,
            SIGNAL(textChanged(QString)),
            model,
            SLOT(changeWorldSpec(QString)));

    connect(model,
            SIGNAL(debugChanged(int)),
            debug,
            SLOT(setCurrentIndex(int)));

    connect(model,
            SIGNAL(fetchChanged(bool)),
            fetch,
            SLOT(setChecked(bool)));

    connect(model,
            SIGNAL(noConfigChanged(bool)),
            noConfigProtection,
            SLOT(setChecked(bool)));

    connect(model,
            SIGNAL(noSafeChanged(bool)),
            noSafeResume,
            SLOT(setChecked(bool)));

    connect(model,
            SIGNAL(pretendChanged(bool)),
            pretend,
            SLOT(setChecked(bool)));

    connect(model,
            SIGNAL(continueChanged(int)),
            continueOnFailure,
            SLOT(setCurrentIndex(int)));

    connect(model,
            SIGNAL(worldSpecChanged(const QString &)),
            worldSpec,
            SLOT(setText(const QString &)));
}

pertubis::InstallSettingsView::~InstallSettingsView()
{
}
