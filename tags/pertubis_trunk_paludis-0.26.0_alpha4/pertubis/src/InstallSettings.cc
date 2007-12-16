
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
#include <QGroupBox>
#include <QLabel>
#include <QDebug>

#include <QSettings>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QGridLayout>
#include <sstream>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/visitor-impl.hh>
#include <paludis/dep_list.hh>


pertubis::InstallSettingsModel::InstallSettingsModel(QObject *pobj) : QObject(pobj),paludis::args::ArgsHandler(),
        m_debug(0),
        m_continueOnFailure(0),
        install_args(this, "Install, Uninstall options",
                        "Options which are relevant for --install, --uninstall or --uninstall-unused.")
{
    loadSettings();
}

std::string pertubis::InstallSettingsModel::app_name() const
{
    return "";
}

std::string pertubis::InstallSettingsModel::app_synopsis() const
{
    return "";
}

std::string pertubis::InstallSettingsModel::app_description() const
{
    return "";
}

pertubis::InstallSettingsModel::~InstallSettingsModel()
{
    saveSettings();
}

void pertubis::InstallSettingsModel::populate_install_task(const paludis::Environment *env, paludis::InstallTask &task) const
{
    install_args.populate_install_task(env,task);
}

QString pertubis::InstallSettingsModel::getDesc()
{
    std::ostringstream stream;
    paludis::args::ArgsHandler::dump_to_stream(stream);
    return QString::fromStdString(stream.str());
}

void pertubis::InstallSettingsModel::loadSettings()
{
    QSettings settings;
    settings.beginGroup( "InstallSettingsModel" );
    install_args.a_debug_build.set_argument(settings.value("debug_build","split").toString().toStdString());
    install_args.a_fetch.set_specified(settings.value("fetch",false).toBool());
    install_args.a_no_config_protection.set_specified(settings.value("no_config_protect",false).toBool());
    install_args.a_no_safe_resume.set_specified(settings.value("no_safe_resume",false).toBool());
    install_args.a_pretend.set_specified(settings.value("pretend",false).toBool());
    install_args.a_continue_on_failure.set_argument(settings.value("continue_on_failure","if-fetch-only").toString().toStdString());
    install_args.a_checks.set_argument(settings.value("checks","default").toString().toStdString());
    settings.endGroup();
}

void pertubis::InstallSettingsModel::saveSettings()
{
    QSettings settings;
    settings.beginGroup( "InstallSettingsModel" );
    settings.setValue("debug",install_args.a_debug_build.specified());
    settings.setValue("fetch",install_args.a_fetch.specified());
    settings.setValue("no_config_protect",install_args.a_no_config_protection.specified());
    settings.setValue("no_safe_resume",install_args.a_no_safe_resume.specified());
    settings.setValue("pretend",install_args.a_pretend.specified());
    settings.setValue("continue_on_failure",QString::fromStdString(install_args.a_continue_on_failure.argument()));
    settings.setValue("checks",QString::fromStdString(install_args.a_checks.argument()));
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
        m_checks(new QComboBox(pobj))
{
    createNormal(pobj);
}

void pertubis::InstallSettingsView::createNormal(QWidget* pobj)
{
    QGroupBox* group(new QGroupBox(tr("Installation Settings"),pobj));

    group->setToolTip(m_model->getDesc());
    m_debug->setToolTip( tr("debug build") );
    m_debug->addItem( tr("none") );
    m_debug->addItem( tr("split") );
    m_debug->addItem( tr("internal") );
    m_debug->setCurrentIndex(m_model->install_args.a_debug_build.specified());
    m_fetch->setToolTip( tr("Only fetch sources; don't install anything") );
    m_fetch->setChecked(m_model->install_args.a_fetch.specified());
    m_noConfigProtection->setToolTip(tr("Disable config file protection (dangerous)"));
    m_noConfigProtection->setChecked(m_model->install_args.a_no_config_protection.specified());
    m_noSafeResume->setToolTip( tr("Do not allow interrupted downloads to be resumed"));
    m_noSafeResume->setChecked(m_model->install_args.a_no_safe_resume.specified());
    m_pretend->setToolTip( tr("Pretend only") );
    m_pretend->setChecked(m_model->install_args.a_pretend.specified());
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
}

void pertubis::InstallSettingsView::createView()
{
//     QGroupBox* group = new QGroupBox(this);
//     group->setTitle(QString::fromStdString(m_model->install_args.name()));
//     group->setToolTip(m_model->m_tooltip);
//     ArgumentViewCreator creator(this,group);
//     qDebug() << creator.m_text;
//     std::for_each(indirect_iterator(m_model->install_args.begin()), indirect_iterator(m_model->install_args.end()), accept_visitor(creator));
}

pertubis::InstallSettingsView::~InstallSettingsView()
{
}
