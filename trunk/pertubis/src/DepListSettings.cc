
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

#include "DepListSettings.hh"
#include "FormatterUtils.hh"
#include <QGroupBox>
#include <QLabel>
#include <QLabel>
#include <QSettings>
#include <QComboBox>
#include <QGridLayout>

#include <paludis/dep_list.hh>

pertubis::DepListSettingsModel::DepListSettingsModel(QObject *pobj) :
        QObject(pobj),
        m_dlDepsDefault(0),
        m_dlInstalledDepsPre(0),
        m_dlInstalledDepsRuntime(0),
        m_dlInstalledDepsPost(0),
        m_dlUninstalledDepsPre(0),
        m_dlUninstalledDepsRuntime(0),
        m_dlUninstalledDepsPost(0),
        m_dlUninstalledDepsSuggested(0),
        m_dlSuggested(0),
        m_dlBlocks(0),
        m_dlCircular(0),
        m_dlOverrideMasks(0),
        m_dlFallBack(0),
        m_dlDowngrade(0),
        m_dlNewSlots(0),
        m_dlReinstall(0),
        m_dlReinstallScm(0),
        m_dlReinstallTargets(0),
        m_dlUpgrade(0)
{
    loadSettings();
}
pertubis::DepListSettingsModel::~DepListSettingsModel()
{
    saveSettings();
}

void pertubis::DepListSettingsModel::loadSettings()
{
    QSettings settings;
    settings.beginGroup( "DepListSettings" );
        m_dlDepsDefault = settings.value("depsDefault",0).toInt();
        m_dlInstalledDepsPre = settings.value("idepsPre",0).toInt();
        m_dlInstalledDepsRuntime = settings.value("idepsRun",0).toInt();
        m_dlInstalledDepsPost = settings.value("idepsPost",0).toInt();
        m_dlUninstalledDepsPre = settings.value("udepsPre",0).toInt();
        m_dlUninstalledDepsRuntime = settings.value("udepsRun",0).toInt();
        m_dlUninstalledDepsPost = settings.value("udepsPost",0).toInt();
        m_dlUninstalledDepsSuggested = settings.value("udepsS",0).toInt();
        m_dlSuggested = settings.value("Suggested",0).toInt();
        m_dlBlocks = settings.value("Block",0).toInt();
        m_dlCircular = settings.value("Circular",0).toInt();
        m_dlOverrideMasks = settings.value("Override",0).toInt();
        m_dlFallBack = settings.value("Fallback",0).toInt();
        m_dlDowngrade = settings.value("Downgrade",0).toInt();
        m_dlNewSlots = settings.value("NewSlots",0).toInt();
        m_dlReinstall = settings.value("Reinstall",0).toInt();
        m_dlReinstallScm = settings.value("ReinstallScm",0).toInt();
        m_dlReinstallTargets = settings.value("ReinstallTarget",0).toInt();
        m_dlUpgrade = settings.value("Upgrade",0).toInt();
    settings.endGroup();
}

void pertubis::DepListSettingsModel::saveSettings()
{
    QSettings settings;
    settings.beginGroup( "DepListSettings" );
    settings.setValue("depsDefault",m_dlDepsDefault );
    settings.setValue("idepsPre",m_dlInstalledDepsPre);
    settings.setValue("idepsRun",m_dlInstalledDepsRuntime);
    settings.setValue("idepsPost",m_dlInstalledDepsPost);
    settings.setValue("udepsPre",m_dlUninstalledDepsPre );
    settings.setValue("udepsRun",m_dlUninstalledDepsRuntime );
    settings.setValue("udepsPost",m_dlUninstalledDepsPost);
    settings.setValue("udepsS",m_dlUninstalledDepsSuggested );
    settings.setValue("Suggested",m_dlSuggested);
    settings.setValue("Block",m_dlBlocks);
    settings.setValue("Circular",m_dlCircular);
    settings.setValue("Override",m_dlOverrideMasks);
    settings.setValue("Fallback",m_dlFallBack);
    settings.setValue("Downgrade",m_dlDowngrade);
    settings.setValue("NewSlots",m_dlNewSlots);
    settings.setValue("Reinstall",m_dlReinstall);
    settings.setValue("ReinstallScm",m_dlReinstallScm);
    settings.setValue("ReinstallTarget",m_dlReinstallTargets);
    settings.setValue("Upgrade",m_dlUpgrade);
    settings.endGroup();
}


pertubis::DepListSettingsView::DepListSettingsView(QWidget *pobj, DepListSettingsModel* model) :
        QWidget(pobj),
        m_model(model),
        m_dlDepsDefault(new QComboBox(this)),
        m_dlInstalledDepsPre(new QComboBox(this)),
        m_dlInstalledDepsRuntime(new QComboBox(this)),
        m_dlInstalledDepsPost(new QComboBox(this)),
        m_dlUninstalledDepsPre(new QComboBox(this)),
        m_dlUninstalledDepsRuntime(new QComboBox(this)),
        m_dlUninstalledDepsPost(new QComboBox(this)),
        m_dlUninstalledDepsSuggested(new QComboBox(this)),
        m_dlSuggested(new QComboBox(this)),
        m_dlBlocks(new QComboBox(this)),
        m_dlCircular(new QComboBox(this)),
        m_dlOverrideMasks(new QComboBox(this)),
        m_dlFallBack(new QComboBox(this)),
        m_dlDowngrade(new QComboBox(this)),
        m_dlNewSlots(new QComboBox(this)),
        m_dlReinstall(new QComboBox(this)),
        m_dlReinstallScm(new QComboBox(this)),
        m_dlReinstallTargets(new QComboBox(this)),
        m_dlUpgrade(new QComboBox(this))
{
    QGroupBox* group(new QGroupBox(tr("Dependency List Settings"),pobj));
    group->setToolTip(tr("Modify dependency list generation behaviour. Use with caution."));
    QStringList commonItems;
    commonItems << tr("pre") << tr("pre-or-post") << tr("post") << tr("try-post") << tr("discard");

    QString commonHelp(QString("<table border=\"0\" summary=\"\" width=\"100%\" height=\"100%\" cellpadding=\"0\"><colgroup><col width=\"50%\"><col width=\"50%\"></colgroup>%2%3%4%5%6</table>").arg(
                    make_row(tr("discard"),tr("Discard"))).arg(
                    make_row(tr("pre"),tr("As pre dependencies"))).arg(
                    make_row(tr("pre-or-post"),tr("As pre dependencies, or post dependencies where needed"))).arg(
                    make_row(tr("post"),tr("As pre dependencies, or post dependencies where needed"))).arg(
                    make_row(tr("try-post"),tr("As post dependencies"))));

    m_dlDepsDefault->addItems(commonItems);
    m_dlDepsDefault->setToolTip(QString("<html><body><p>%1</p><p>%2</p></body></html>").arg(tr("Override default behaviour for all dependency classes")).arg(commonHelp));
    m_dlDepsDefault->setCurrentIndex(m_model->m_dlDepsDefault);

    m_dlInstalledDepsPre->addItems(commonItems);
    m_dlInstalledDepsPre->setToolTip(QString("<html><body><p>%1</p><p>%2</p></body></html>").arg(tr("Override default behaviour for all dependency classes")).arg(commonHelp));
    m_dlInstalledDepsPre->setCurrentIndex(m_model->m_dlInstalledDepsPre);

    m_dlInstalledDepsRuntime->addItems(commonItems);
    m_dlInstalledDepsRuntime->setToolTip(QString("<html><body><p>%1</p><p>%2</p></body></html>").arg(tr("How to handle runtime dependencies for installed packages")).arg(commonHelp));
    m_dlInstalledDepsRuntime->setCurrentIndex(m_model->m_dlInstalledDepsRuntime);

    m_dlInstalledDepsPost->addItems(commonItems);
    m_dlInstalledDepsPost->setToolTip(QString("<html><body><p>%1</p><p>%2</p></body></html>").arg(tr("How to handle post dependencies for installed packages")).arg(commonHelp));
    m_dlInstalledDepsPost->setCurrentIndex(m_model->m_dlInstalledDepsPost);

    m_dlUninstalledDepsPre->addItems(commonItems);
    m_dlUninstalledDepsPre->setToolTip(QString("<html><body><p>%1</p><p>%2</p></body></html>").arg(tr("How to handle pre dependencies for uninstalled packagess")).arg(commonHelp));
    m_dlUninstalledDepsPre->setCurrentIndex(m_model->m_dlUninstalledDepsPre);

    m_dlUninstalledDepsRuntime->addItems(commonItems);
    m_dlUninstalledDepsRuntime->setToolTip(QString("<html><body><p>%1</p><p>%2</p></body></html>").arg(tr("How to handle runtime dependencies for uninstalled packages")).arg(commonHelp));
    m_dlUninstalledDepsRuntime->setCurrentIndex(m_model->m_dlUninstalledDepsRuntime);

    m_dlUninstalledDepsPost->addItems(commonItems);
    m_dlUninstalledDepsPost->setToolTip(QString("<html><body><p>%1</p><p>%2</p></body></html>").arg(tr("How to handle post dependencies for uninstalled packages")).arg(commonHelp));
    m_dlUninstalledDepsPost->setCurrentIndex(m_model->m_dlUninstalledDepsPost);

    m_dlUninstalledDepsSuggested->addItems(commonItems);
    m_dlUninstalledDepsSuggested->setToolTip(QString("<html><body><p>%1</p><p>%2</p></body></html>").arg(tr("How to handle suggested dependencies for uninstalled packages (only with --dl-suggested install)")).arg(commonHelp));
    m_dlUninstalledDepsSuggested->setCurrentIndex(m_model->m_dlUninstalledDepsSuggested);

    m_dlSuggested->setToolTip(tr("How to handle suggested dependencies"));
    m_dlSuggested->addItem(tr("show"));
    m_dlSuggested->addItem(tr("install"));
    m_dlSuggested->addItem(tr("discard"));
    m_dlSuggested->setCurrentIndex(m_model->m_dlSuggested);

    m_dlCircular->setToolTip(tr("How to handle circular dependencies"));
    m_dlCircular->addItem(tr("error"));
    m_dlCircular->addItem(tr("discard"));
    m_dlCircular->setCurrentIndex(m_model->m_dlCircular);

    m_dlBlocks->setToolTip(tr("How to handle blocks"));
    m_dlBlocks->addItem(tr("accumulate"));
    m_dlBlocks->addItem(tr("error"));
    m_dlBlocks->addItem(tr("discard"));
    m_dlBlocks->setCurrentIndex(m_model->m_dlBlocks);

    m_dlOverrideMasks->setToolTip(tr("Zero or more mask kinds that can be overridden as necessary (default: tilde-keyword and license)"));
    m_dlOverrideMasks->addItem(tr("none"));
    m_dlOverrideMasks->addItem(tr("tilde-keyword"));
    m_dlOverrideMasks->addItem(tr("unkeyworded"));
    m_dlOverrideMasks->addItem(tr("repository"));
    m_dlOverrideMasks->addItem(tr("profile"));
    m_dlOverrideMasks->setCurrentIndex(m_model->m_dlOverrideMasks);

    m_dlFallBack->setToolTip(tr("When to fall back to installed packages"));
    m_dlFallBack->addItem(tr("as-needed-except-targets"));
    m_dlFallBack->addItem(tr("as-needed"));
    m_dlFallBack->addItem(tr("never"));
    m_dlFallBack->setCurrentIndex(m_model->m_dlFallBack);

    m_dlReinstall->setToolTip(tr("When to reinstall packages"));
    m_dlReinstall->addItem(tr("never"));
    m_dlReinstall->addItem(tr("always"));
    m_dlReinstall->addItem(tr("if-use-changed"));
    m_dlReinstall->setCurrentIndex(m_model->m_dlReinstall);

    m_dlReinstallScm->setToolTip(tr("When to reinstall scm packages"));
    m_dlReinstallScm->addItem(tr("never"));
    m_dlReinstallScm->addItem(tr("always"));
    m_dlReinstallScm->addItem(tr("daily"));
    m_dlReinstallScm->addItem(tr("weekly"));
    m_dlReinstallScm->setCurrentIndex(m_model->m_dlReinstallScm);

    m_dlReinstallTargets->setToolTip(tr("Whether to reinstall targets"));
    m_dlReinstallTargets->addItem(tr("auto"));
    m_dlReinstallTargets->addItem(tr("never"));
    m_dlReinstallTargets->addItem(tr("always"));
    m_dlReinstallTargets->setCurrentIndex(m_model->m_dlReinstallTargets);

    m_dlUpgrade->setToolTip(tr("When to upgrade packages"));
    m_dlUpgrade->addItem(tr("always"));
    m_dlUpgrade->addItem(tr("as-needed"));
    m_dlUpgrade->setCurrentIndex(m_model->m_dlUpgrade);

    m_dlDowngrade->setToolTip(tr("When to downgrade packages"));
    m_dlDowngrade->addItem(tr("as-needed"));
    m_dlDowngrade->addItem(tr("warning"));
    m_dlDowngrade->addItem(tr("error"));
    m_dlDowngrade->setCurrentIndex(m_model->m_dlDowngrade);

    m_dlNewSlots->setToolTip(tr("When to downgrade packages"));
    m_dlNewSlots->addItem(tr("always"));
    m_dlNewSlots->addItem(tr("as-needed"));
    m_dlNewSlots->setCurrentIndex(m_model->m_dlNewSlots);

    QGridLayout *groupLayout = new QGridLayout;
    groupLayout->addWidget(new QLabel(tr("Default Dependencies"),m_dlDepsDefault), 0, 0);
    groupLayout->addWidget(new QLabel(tr("Installed Dependencies Pre"),m_dlInstalledDepsPre), 1, 0);
    groupLayout->addWidget(new QLabel(tr("Installed Dependencies Runtime"),m_dlInstalledDepsRuntime), 2, 0);
    groupLayout->addWidget(new QLabel(tr("Installed Dependencies Post"),m_dlInstalledDepsPost), 3, 0);
    groupLayout->addWidget(new QLabel(tr("Unistalled Dependencies Pre"),m_dlUninstalledDepsPre), 4, 0);
    groupLayout->addWidget(new QLabel(tr("Uninstalled Dependencies Runtime"),m_dlUninstalledDepsRuntime), 5, 0);
    groupLayout->addWidget(new QLabel(tr("Uninstalled Dependencies Post"),m_dlUninstalledDepsPost), 6, 0);
    groupLayout->addWidget(new QLabel(tr("Uninstalled Dependencies Suggested"),m_dlUninstalledDepsSuggested), 7, 0);
    groupLayout->addWidget(new QLabel(tr("Suggested Dependencies"),m_dlSuggested), 8, 0);
    groupLayout->addWidget(new QLabel(tr("Blocks Dependencies"),m_dlBlocks), 9, 0);
    groupLayout->addWidget(new QLabel(tr("Circular Dependencies"),m_dlCircular), 10, 0);
    groupLayout->addWidget(new QLabel(tr("Override Masks"),m_dlOverrideMasks), 11, 0);
    groupLayout->addWidget(new QLabel(tr("Fallback"),m_dlFallBack), 12, 0);
    groupLayout->addWidget(new QLabel(tr("Downgrade"),m_dlDowngrade), 13, 0);
    groupLayout->addWidget(new QLabel(tr("New Slots"),m_dlNewSlots), 14, 0);
    groupLayout->addWidget(new QLabel(tr("Reinstall"),m_dlReinstall), 15, 0);
    groupLayout->addWidget(new QLabel(tr("Reinstall scm"),m_dlReinstallScm), 16, 0);
    groupLayout->addWidget(new QLabel(tr("Reinstall Target"),m_dlReinstallTargets), 17, 0);
    groupLayout->addWidget(new QLabel(tr("Upgrade"),m_dlUpgrade), 18, 0);
    groupLayout->addWidget(m_dlDepsDefault,0,1);
    groupLayout->addWidget(m_dlInstalledDepsPre,1,1);
    groupLayout->addWidget(m_dlInstalledDepsRuntime,2,1);
    groupLayout->addWidget(m_dlInstalledDepsPost,3,1);
    groupLayout->addWidget(m_dlUninstalledDepsPre,4,1);
    groupLayout->addWidget(m_dlUninstalledDepsRuntime,5,1);
    groupLayout->addWidget(m_dlUninstalledDepsPost,6,1);
    groupLayout->addWidget(m_dlUninstalledDepsSuggested,7,1);
    groupLayout->addWidget(m_dlSuggested,8,1);
    groupLayout->addWidget(m_dlBlocks,9,1);
    groupLayout->addWidget(m_dlCircular,10,1);
    groupLayout->addWidget(m_dlOverrideMasks,11,1);
    groupLayout->addWidget(m_dlFallBack,12,1);
    groupLayout->addWidget(m_dlDowngrade,13,1);
    groupLayout->addWidget(m_dlNewSlots,14,1);
    groupLayout->addWidget(m_dlReinstall,15,1);
    groupLayout->addWidget(m_dlReinstallScm,16,1);
    groupLayout->addWidget(m_dlReinstallTargets,17,1);
    groupLayout->addWidget(m_dlUpgrade,18,1);

    connect(m_model,
            SIGNAL(depsDefaultChanged(int)),
            m_dlDepsDefault,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(installDepsPreChanged(int)),
            m_dlInstalledDepsPre,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(installDepsRuntimeChanged(int)),
            m_dlInstalledDepsRuntime,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(installDepsPostChanged(int)),
            m_dlInstalledDepsPost,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(uninstallDepsPreChanged(int)),
            m_dlUninstalledDepsPre,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(uninstallDepsRuntimeChanged(int)),
            m_dlUninstalledDepsRuntime,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(uninstallDepsPostChanged(int)),
            m_dlUninstalledDepsPost,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(uninstallDepsSuggestedChanged(int)),
            m_dlUninstalledDepsSuggested,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(suggestedChanged(int)),
            m_dlSuggested,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(blocksChanged(int)),
            m_dlBlocks,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(circularChanged(int)),
            m_dlCircular,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(overrideChanged(int)),
            m_dlOverrideMasks,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(fallbackChanged(int)),
            m_dlFallBack,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(downgradeChanged(int)),
            m_dlDowngrade,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(newSlotsChanged(int)),
            m_dlNewSlots,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(reinstallChanged(int)),
            m_dlReinstall,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(reinstallScmChanged(int)),
            m_dlReinstallScm,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(reinstallTargetChanged(int)),
            m_dlReinstallTargets,
            SLOT(setCurrentIndex(int)));

    connect(m_model,
            SIGNAL(upgradeChanged(int)),
            m_dlUpgrade,
            SLOT(setCurrentIndex(int)));

    // ########################################

    connect(m_dlDepsDefault,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeDepsDefault(int)));

    connect(m_dlInstalledDepsPre,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeInstallDepsPre(int)));

    connect(m_dlInstalledDepsRuntime,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeInstallDepsRuntime(int)));

    connect(m_dlInstalledDepsPost,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeInstallDepsPost(int)));

    connect(m_dlUninstalledDepsPre,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeUninstallDepsPre(int)));

    connect(m_dlUninstalledDepsRuntime,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeUninstallDepsRuntime(int)));

    connect(m_dlUninstalledDepsPost,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeUninstallDepsPost(int)));

    connect(m_dlUninstalledDepsSuggested,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeUninstallDepsSuggested(int)));

    connect(m_dlSuggested,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeSuggested(int)));

    connect(m_dlBlocks,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeBlocks(int)));

    connect(m_dlCircular,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeCircular(int)));

    connect(m_dlOverrideMasks,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeOverride(int)));

    connect(m_dlFallBack,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeFallback(int)));

    connect(m_dlDowngrade,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeDowngrade(int)));

    connect(m_dlNewSlots,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeNewSlots(int)));

    connect(m_dlReinstall,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeReinstall(int)));

    connect(m_dlReinstallScm,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeReinstallScm(int)));

    connect(m_dlReinstallTargets,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeReinstallTarget(int)));

    connect(m_dlUpgrade,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeUpgrade(int)));

    group->setLayout(groupLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(group);
    mainLayout->addSpacing(12);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

pertubis::DepListSettingsView::~DepListSettingsView()
{
}

