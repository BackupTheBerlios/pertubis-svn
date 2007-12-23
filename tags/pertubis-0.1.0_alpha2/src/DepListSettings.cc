
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

#include <paludis/args/args_option.hh>
#include <paludis/dep_list.hh>
#include <paludis/install_task.hh>
#include <paludis/override_functions.hh>
#include <paludis/util/log.hh>
#include <paludis/util/sequence.hh>
#include <paludis/util/tr1_functional.hh>

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSettings>

static paludis::DepListDepsOption getOptionArg(int arg)
{
    if (arg ==  0)
        return paludis::dl_deps_pre;
    else if (arg ==  1)
        return paludis::dl_deps_pre_or_post;
    else if (arg ==  2)
        return paludis::dl_deps_post;
    else if (arg ==  3)
        return paludis::dl_deps_try_post;
    else if (arg ==  4)
        return paludis::dl_deps_discard;
    else
        throw;
}

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

void pertubis::DepListSettingsModel::changeDepsDefault(int state)
{
    if (m_dlDepsDefault != state)
        emit depsDefaultChanged(state);
    m_dlDepsDefault = state;
}

void pertubis::DepListSettingsModel::changeInstallDepsPre(int state)
{
    if (m_dlInstalledDepsPre != state)
        emit installDepsPreChanged(state);
    m_dlInstalledDepsPre = state;
}

void pertubis::DepListSettingsModel::changeInstallDepsRuntime(int state)
{
    if (m_dlInstalledDepsRuntime != state)
        emit installDepsRuntimeChanged(state);
    m_dlInstalledDepsRuntime = state;
}

void pertubis::DepListSettingsModel::changeInstallDepsPost(int state)
{
    if (m_dlInstalledDepsPost != state)
        emit installDepsPostChanged(state);
    m_dlInstalledDepsPost = state;
}

void pertubis::DepListSettingsModel::changeUninstallDepsPre(int state)
{
    if (m_dlUninstalledDepsPre != state)
        emit uninstallDepsPreChanged(state);
    m_dlUninstalledDepsPre = state;
}

void pertubis::DepListSettingsModel::changeUninstallDepsRuntime(int state)
{
    if (m_dlUninstalledDepsRuntime != state)
        emit uninstallDepsRuntimeChanged(state);
    m_dlUninstalledDepsRuntime = state;
}

void pertubis::DepListSettingsModel::changeUninstallDepsPost(int state)
{
    if (m_dlUninstalledDepsPost != state)
        emit uninstallDepsPostChanged(state);
    m_dlUninstalledDepsPost = state;
}

void pertubis::DepListSettingsModel::changeUninstallDepsSuggested(int state)
{
    if (m_dlUninstalledDepsSuggested != state)
        emit uninstallDepsSuggestedChanged(state);
    m_dlUninstalledDepsSuggested = state;
}

void pertubis::DepListSettingsModel::changeSuggested(int state)
{
    if (m_dlSuggested != state)
        emit suggestedChanged(state);
    m_dlSuggested = state;
}

void pertubis::DepListSettingsModel::changeBlocks(int state)
{
    if (m_dlBlocks != state)
        emit blocksChanged(state);
    m_dlBlocks = state;
}
void pertubis::DepListSettingsModel::changeCircular(int state)
{
    if (m_dlCircular != state)
        emit circularChanged(state);
    m_dlCircular = state;
}

void pertubis::DepListSettingsModel::changeOverride(QListWidgetItem * item )
{
    bool onHere(m_dlOverrideMasks.contains(item->text()));
    bool onThere(Qt::Checked == item->checkState());
    if (onHere && !onThere)
    {
        emit overrideChanged(item->text(),Qt::Unchecked);
        m_dlOverrideMasks.remove(item->text());
    }
    if (!onHere && onThere)
    {
        emit overrideChanged(item->text(),Qt::Checked);
        m_dlOverrideMasks.insert(item->text());
    }
}

void pertubis::DepListSettingsModel::changeFallback(int state)
{
    if (m_dlFallBack != state)
        emit fallbackChanged(state);
    m_dlFallBack = state;
}

void pertubis::DepListSettingsModel::changeDowngrade(int state)
{
    if (m_dlDowngrade != state)
        emit downgradeChanged(state);
    m_dlDowngrade = state;
}

void pertubis::DepListSettingsModel::changeNewSlots(int state)
{
    if (m_dlNewSlots != state)
        emit newSlotsChanged(state);
    m_dlNewSlots = state;
}

void pertubis::DepListSettingsModel::changeReinstall(int state)
{
    if (m_dlSuggested != state)
        emit reinstallChanged(state);
    m_dlReinstall = state;
}

void pertubis::DepListSettingsModel::changeReinstallScm(int state)
{
    if (m_dlReinstallScm != state)
        emit reinstallScmChanged(state);
    m_dlReinstallScm = state;
}

void pertubis::DepListSettingsModel::changeReinstallTarget(int state)
{
    if (m_dlReinstallTargets != state)
        emit reinstallTargetChanged(state);
    m_dlReinstallTargets = state;
}

void pertubis::DepListSettingsModel::changeUpgrade(int state)
{
    if (m_dlUpgrade != state)
        emit upgradeChanged(state);
    m_dlUpgrade = state;
}

void pertubis::DepListSettingsView::populate_dep_list_options(const paludis::Environment *env, paludis::DepListOptions & options)
{
    if (m_model->m_dlReinstall == 0)
        options.reinstall = paludis::dl_reinstall_never;
    else if (m_model->m_dlReinstall == 1)
        options.reinstall = paludis::dl_reinstall_always;
    else if (m_model->m_dlReinstall == 2)
        options.reinstall = paludis::dl_reinstall_if_use_changed;

    if (m_model->m_dlReinstallScm == 0)
        options.reinstall_scm = paludis::dl_reinstall_scm_never;
    else if (m_model->m_dlReinstallScm == 1)
        options.reinstall_scm = paludis::dl_reinstall_scm_always;
    else if (m_model->m_dlReinstallScm == 2)
        options.reinstall_scm = paludis::dl_reinstall_scm_daily;
    else if (m_model->m_dlReinstallScm == 3)
        options.reinstall_scm = paludis::dl_reinstall_scm_weekly;

    if (m_model->m_dlUpgrade == 0)
        options.upgrade = paludis::dl_upgrade_as_needed;
    else if (m_model->m_dlUpgrade == 1)
        options.upgrade = paludis::dl_upgrade_always;

    if (m_model->m_dlNewSlots == 0)
        options.new_slots = paludis::dl_new_slots_as_needed;
    else if (m_model->m_dlNewSlots == 1)
        options.new_slots = paludis::dl_new_slots_always;

    if (m_model->m_dlDowngrade == 0)
        options.downgrade = paludis::dl_downgrade_as_needed;
    else if (m_model->m_dlDowngrade == 1)
        options.downgrade = paludis::dl_downgrade_warning;
    else if (m_model->m_dlDowngrade == 2)
        options.downgrade = paludis::dl_downgrade_error;

    if (m_model->m_dlCircular == 0)
        options.circular = paludis::dl_circular_discard;
    else if (m_model->m_dlCircular == 1)
        options.circular = paludis::dl_circular_error;


    if (m_model->m_dlSuggested == 0)
        options.suggested = paludis::dl_suggested_show;
    else if (m_model->m_dlSuggested == 1)
        options.suggested = paludis::dl_suggested_discard;
    else if (m_model->m_dlSuggested == 2)
        options.suggested = paludis::dl_suggested_install;

    if (m_model->m_dlBlocks == 0)
        options.blocks = paludis::dl_blocks_discard;
    else if (m_model->m_dlBlocks == 1)
        options.blocks = paludis::dl_blocks_error;
    else if (m_model->m_dlBlocks == 2)
        options.blocks = paludis::dl_blocks_accumulate;

    if (! options.override_masks)
        options.override_masks.reset(new paludis::DepListOverrideMasksFunctions);
    options.override_masks->push_back(paludis::tr1::bind(&paludis::override_tilde_keywords, env, paludis::tr1::placeholders::_1, paludis::tr1::placeholders::_2));
    options.override_masks->push_back(paludis::tr1::bind(&paludis::override_license, paludis::tr1::placeholders::_2));

    if ( ! m_model->m_dlOverrideMasks.isEmpty())
    {
        for (QSet<QString>::const_iterator a(m_model->m_dlOverrideMasks.constBegin()),
             a_end(m_model->m_dlOverrideMasks.constEnd()) ; a != a_end ; ++a)
            if (*a == "none")
                options.override_masks.reset(new paludis::DepListOverrideMasksFunctions);

        for (QSet<QString>::const_iterator a(m_model->m_dlOverrideMasks.constBegin()),
             a_end(m_model->m_dlOverrideMasks.constEnd()) ; a != a_end ; ++a)
        {
            if (*a == "tilde-keyword")
                options.override_masks->push_back(paludis::tr1::bind(&paludis::override_tilde_keywords, env, paludis::tr1::placeholders::_1, paludis::tr1::placeholders::_2));
            else if (*a == "unkeyworded")
                options.override_masks->push_back(paludis::tr1::bind(&paludis::override_unkeyworded, env, paludis::tr1::placeholders::_1,paludis::tr1::placeholders::_2));
            else if (*a == "repository")
                options.override_masks->push_back(paludis::tr1::bind(&paludis::override_repository_masks, paludis::tr1::placeholders::_2));
            else if (*a == "license")
                options.override_masks->push_back(paludis::tr1::bind(&paludis::override_license, paludis::tr1::placeholders::_2));
            else if (*a == "profile")
            {
                paludis::Log::get_instance()->message(paludis::ll_warning, paludis::lc_no_context) <<
                        "--dl-override-masks profile is deprecated, use --dl-override-masks repository";
                options.override_masks->push_back(paludis::tr1::bind(&paludis::override_repository_masks, paludis::tr1::placeholders::_2));
            }
            else if (*a == "none")
            {
            }
        }
    }

    if (m_model->m_dlFallBack == 0)
        options.fall_back = paludis::dl_fall_back_as_needed_except_targets;
    else if (m_model->m_dlFallBack == 1)
        options.fall_back = paludis::dl_fall_back_as_needed;
    else if (m_model->m_dlFallBack == 2)
        options.fall_back = paludis::dl_fall_back_never;


    if (m_model->m_dlDepsDefault != -1)
    {
        paludis::DepListDepsOption op( getOptionArg(m_model->m_dlDepsDefault) );
        options.installed_deps_pre = op;
        options.installed_deps_post = op;
        options.installed_deps_runtime = op;
        options.uninstalled_deps_pre = op;
        options.uninstalled_deps_post = op;
        options.uninstalled_deps_runtime = op;
        options.uninstalled_deps_suggested = op;
    }

    if (m_model->m_dlInstalledDepsPre || ! m_model->m_dlDepsDefault)
        options.installed_deps_pre = getOptionArg(m_model->m_dlInstalledDepsPre);
    if (m_model->m_dlInstalledDepsRuntime || ! m_model->m_dlDepsDefault )
        options.installed_deps_runtime = getOptionArg(m_model->m_dlInstalledDepsRuntime);
    if (m_model->m_dlInstalledDepsPost || ! m_model->m_dlDepsDefault )
        options.installed_deps_post = getOptionArg(m_model->m_dlInstalledDepsPost);

    if (m_model->m_dlUninstalledDepsPre || ! m_model->m_dlDepsDefault)
        options.uninstalled_deps_pre = getOptionArg(m_model->m_dlUninstalledDepsPre);
    if (m_model->m_dlUninstalledDepsRuntime || ! m_model->m_dlDepsDefault)
        options.uninstalled_deps_runtime = getOptionArg(m_model->m_dlUninstalledDepsRuntime);
    if (m_model->m_dlUninstalledDepsPost || ! m_model->m_dlDepsDefault)
        options.uninstalled_deps_post = getOptionArg(m_model->m_dlUninstalledDepsPost);
    if (m_model->m_dlUninstalledDepsSuggested || ! m_model->m_dlDepsDefault)
        options.uninstalled_deps_suggested = getOptionArg(m_model->m_dlUninstalledDepsSuggested);
}

void pertubis::DepListSettingsView::populate_install_task(const paludis::Environment *, paludis::InstallTask & task) const
{
    if (m_model->m_dlReinstallTargets != -1)
    {
        if (m_model->m_dlReinstallTargets == 0)
        {
        }
        else if (m_model->m_dlReinstallTargets == 2)
            task.override_target_type(paludis::dl_target_package);
        else if (m_model->m_dlReinstallTargets == 1)
            task.override_target_type(paludis::dl_target_set);
    }
}

void pertubis::DepListSettingsModel::loadSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
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
        QVariantList overrides(settings.value("Override",QVariantList() << "tilde-keyword" << "license").toList());
        for (QVariantList::const_iterator it(overrides.constBegin()),iEnd(overrides.constEnd());
             it != iEnd;++it)
        {
            m_dlOverrideMasks.insert( (*it).toString());
        }
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
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
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
    QVariantList overrides;
    for (QSet<QString>::const_iterator it(m_dlOverrideMasks.constBegin()),iEnd(m_dlOverrideMasks.constEnd());
         it != iEnd;++it)
    {
        overrides.push_back(*it);
    }
    settings.setValue("Override",overrides);
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
        m_dlBlocks(new QComboBox(this)),
        m_dlCircular(new QComboBox(this)),
        m_dlDepsDefault(new QComboBox(this)),
        m_dlDowngrade(new QComboBox(this)),
        m_dlFallBack(new QComboBox(this)),
        m_dlInstalledDepsPost(new QComboBox(this)),
        m_dlInstalledDepsPre(new QComboBox(this)),
        m_dlInstalledDepsRuntime(new QComboBox(this)),
        m_dlNewSlots(new QComboBox(this)),
        m_dlReinstall(new QComboBox(this)),
        m_dlReinstallScm(new QComboBox(this)),
        m_dlReinstallTargets(new QComboBox(this)),
        m_dlSuggested(new QComboBox(this)),
        m_dlUninstalledDepsPost(new QComboBox(this)),
        m_dlUninstalledDepsPre(new QComboBox(this)),
        m_dlUninstalledDepsRuntime(new QComboBox(this)),
        m_dlUninstalledDepsSuggested(new QComboBox(this)),
        m_dlUpgrade(new QComboBox(this)),
        m_dlOverrideMasks(new QListWidget(this))
{
    QGroupBox* group(new QGroupBox(tr("Dependency List Settings"),pobj));
//     group->setWhatsThis("");
    QStringList commonItems;
    commonItems << tr("pre") << tr("pre-or-post") << tr("post") << tr("try-post") << tr("discard");

    QString commonHelp(QString("<table border=\"0\" summary=\"\" width=\"100%\" height=\"100%\" cellpadding=\"0\"><colgroup><col width=\"50%\"><col width=\"50%\"></colgroup>%2%3%4%5%6</table>").arg(
                    make_row(tr("discard"),tr("Discard"))).arg(
                    make_row(tr("pre"),tr("As pre dependencies"))).arg(
                    make_row(tr("pre-or-post"),tr("As pre dependencies, or post dependencies where needed"))).arg(
                    make_row(tr("post"),tr("As pre dependencies, or post dependencies where needed"))).arg(
                    make_row(tr("try-post"),tr("As post dependencies"))));

//     m_dlDepsDefault->setToolTip(QString::fromStdString(m_model->m_depListArgs.dl_deps_default.description()));
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

    QListWidgetItem* a0(new QListWidgetItem("none"));
    QListWidgetItem* a1(new QListWidgetItem("tilde-keyword"));
    QListWidgetItem* a2(new QListWidgetItem("unkeyworded"));
    QListWidgetItem* a3(new QListWidgetItem("repository"));
    QListWidgetItem* a4(new QListWidgetItem("profile"));
    QListWidgetItem* a5(new QListWidgetItem("license"));
    a0->setCheckState(m_model->m_dlOverrideMasks.contains("none") ? Qt::Checked : Qt::Unchecked);
    a1->setCheckState(m_model->m_dlOverrideMasks.contains("tilde-keyword") ? Qt::Checked : Qt::Unchecked);
    a2->setCheckState(m_model->m_dlOverrideMasks.contains("unkeyworded") ? Qt::Checked : Qt::Unchecked);
    a3->setCheckState(m_model->m_dlOverrideMasks.contains("repository") ? Qt::Checked : Qt::Unchecked);
    a4->setCheckState(m_model->m_dlOverrideMasks.contains("profile") ? Qt::Checked : Qt::Unchecked);
    a5->setCheckState(m_model->m_dlOverrideMasks.contains("license") ? Qt::Checked : Qt::Unchecked);
    m_dlOverrideMasks->addItem(a0);
    m_dlOverrideMasks->addItem(a1);
    m_dlOverrideMasks->addItem(a2);
    m_dlOverrideMasks->addItem(a3);
    m_dlOverrideMasks->addItem(a4);
    m_dlOverrideMasks->addItem(a5);

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
    groupLayout->addWidget(new QLabel(tr("Blocks Dependencies"),m_dlBlocks), 0, 0);
    groupLayout->addWidget(m_dlBlocks,0,1);

    groupLayout->addWidget(new QLabel(tr("Circular Dependencies"),m_dlCircular), 1, 0);
    groupLayout->addWidget(m_dlCircular,1,1);

    groupLayout->addWidget(new QLabel(tr("Default Dependencies"),m_dlDepsDefault), 2, 0);
    groupLayout->addWidget(m_dlDepsDefault,2,1);
    groupLayout->addWidget(new QLabel(tr("Downgrade"),m_dlDowngrade), 3, 0);
    groupLayout->addWidget(m_dlDowngrade,3,1);
    groupLayout->addWidget(new QLabel(tr("Fallback"),m_dlFallBack), 4, 0);
    groupLayout->addWidget(m_dlFallBack,4,1);
    groupLayout->addWidget(new QLabel(tr("Installed Dependencies Post"),m_dlInstalledDepsPost), 5, 0);
    groupLayout->addWidget(m_dlInstalledDepsPost,5,1);
    groupLayout->addWidget(new QLabel(tr("Installed Dependencies Pre"),m_dlInstalledDepsPre), 6, 0);
    groupLayout->addWidget(m_dlInstalledDepsPre,6,1);
    groupLayout->addWidget(new QLabel(tr("Installed Dependencies Runtime"),m_dlInstalledDepsRuntime), 7, 0);
    groupLayout->addWidget(m_dlInstalledDepsRuntime,7,1);
    groupLayout->addWidget(new QLabel(tr("New Slots"),m_dlNewSlots), 8, 0);
    groupLayout->addWidget(m_dlNewSlots,8,1);
    groupLayout->addWidget(new QLabel(tr("Reinstall"),m_dlReinstall), 9, 0);
    groupLayout->addWidget(m_dlReinstall,9,1);
    groupLayout->addWidget(new QLabel(tr("Reinstall scm"),m_dlReinstallScm), 10, 0);
    groupLayout->addWidget(m_dlReinstallScm,10,1);
    groupLayout->addWidget(new QLabel(tr("Reinstall Target"),m_dlReinstallTargets), 11, 0);
    groupLayout->addWidget(m_dlReinstallTargets,11,1);
    groupLayout->addWidget(new QLabel(tr("Suggested Dependencies"),m_dlSuggested), 12, 0);
    groupLayout->addWidget(m_dlSuggested,12,1);
    groupLayout->addWidget(new QLabel(tr("Uninstalled Dependencies Post"),m_dlUninstalledDepsPost), 13, 0);
    groupLayout->addWidget(m_dlUninstalledDepsPost,13,1);
    groupLayout->addWidget(new QLabel(tr("Unistalled Dependencies Pre"),m_dlUninstalledDepsPre), 14, 0);
    groupLayout->addWidget(m_dlUninstalledDepsPre,14,1);
    groupLayout->addWidget(new QLabel(tr("Uninstalled Dependencies Runtime"),m_dlUninstalledDepsRuntime), 15, 0);
    groupLayout->addWidget(m_dlUninstalledDepsRuntime,15,1);
    groupLayout->addWidget(new QLabel(tr("Uninstalled Dependencies Suggested"),m_dlUninstalledDepsSuggested), 16, 0);
    groupLayout->addWidget(m_dlUninstalledDepsSuggested,16,1);
    groupLayout->addWidget(new QLabel(tr("Upgrade"),m_dlUpgrade), 17, 0);
    groupLayout->addWidget(m_dlUpgrade,17,1);
    groupLayout->addWidget(new QLabel(tr("Override Masks"),m_dlOverrideMasks), 18, 0);
    groupLayout->addWidget(m_dlOverrideMasks,18,1);

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
            SIGNAL(overrideChanged(QString,Qt::CheckState)),
            this,
            SLOT(setCurrentIndex(QString,Qt::CheckState)));

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
            SIGNAL(itemClicked(QListWidgetItem*)),
            m_model,
            SLOT(changeOverride(QListWidgetItem*)));

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

void pertubis::DepListSettingsView::setCurrentIndex(QString name,Qt::CheckState state)
{
    QList<QListWidgetItem*> list(m_dlOverrideMasks->findItems(name,Qt::MatchExactly | Qt::MatchCaseSensitive));
    if (list.count() == 1)
        (*list.begin())->setCheckState(state);
}
