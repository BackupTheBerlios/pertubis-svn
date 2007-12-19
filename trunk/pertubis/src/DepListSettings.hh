
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_DEP_LIST_SETTINGS_H
#define _PERTUBIS_ENTRY_PROTECTOR_DEP_LIST_SETTINGS_H 1

#include <QWidget>
#include <paludis/environment-fwd.hh>
#include <paludis/dep_list-fwd.hh>
#include <QSet>
#include <QString>

class QListWidget;
class QComboBox;
class QListWidgetItem;

namespace paludis
{
    class InstallTask;
}

namespace pertubis
{

    class DepListSettingsModel : public QObject
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            DepListSettingsModel(QObject *parent);
            ///@}

            virtual ~DepListSettingsModel();

            ///@name Content modification
            ///@{

            /// loads all settings for the gui
            void loadSettings();

            /// saves all settings for the gui
            void saveSettings();
            ///@}

            /// \name deplist options
            ///@{

            QSet<QString> m_dlOverrideMasks;
            int m_dlDepsDefault;
            int m_dlInstalledDepsPre;
            int m_dlInstalledDepsRuntime;
            int m_dlInstalledDepsPost;
            int m_dlUninstalledDepsPre;
            int m_dlUninstalledDepsRuntime;
            int m_dlUninstalledDepsPost;
            int m_dlUninstalledDepsSuggested;
            int m_dlSuggested;
            int m_dlBlocks;
            int m_dlCircular;
            int m_dlFallBack;
            int m_dlDowngrade;
            int m_dlNewSlots;
            int m_dlReinstall;
            int m_dlReinstallScm;
            int m_dlReinstallTargets;
            int m_dlUpgrade;

        signals:

            void depsDefaultChanged(int);
            void installDepsPreChanged(int);
            void installDepsRuntimeChanged(int);
            void installDepsPostChanged(int);

            void uninstallDepsPreChanged(int);
            void uninstallDepsRuntimeChanged(int);
            void uninstallDepsPostChanged(int);
            void uninstallDepsSuggestedChanged(int);

            void suggestedChanged(int);
            void blocksChanged(int);
            void circularChanged(int);
            void overrideChanged(QString name,Qt::CheckState state);
            void fallbackChanged(int);
            void downgradeChanged(int);
            void newSlotsChanged(int);
            void reinstallChanged(int);
            void reinstallScmChanged(int);
            void reinstallTargetChanged(int);
            void upgradeChanged(int);

        public slots:

            void changeDepsDefault(int state);
            void changeInstallDepsPre(int state);
            void changeInstallDepsRuntime(int state);
            void changeInstallDepsPost(int state);
            void changeUninstallDepsPre(int state);
            void changeUninstallDepsRuntime(int state);
            void changeUninstallDepsPost(int state);
            void changeUninstallDepsSuggested(int state);
            void changeSuggested(int state);
            void changeBlocks(int state);
            void changeCircular(int state);
            void changeOverride(QListWidgetItem * item );
            void changeFallback(int state);
            void changeDowngrade(int state);
            void changeNewSlots(int state);
            void changeReinstall(int state);
            void changeReinstallScm(int state);
            void changeReinstallTarget(int state);
            void changeUpgrade(int state);
    };

    class DepListSettingsView : public QWidget
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            DepListSettingsView(QWidget *parent, DepListSettingsModel * model);


            virtual ~DepListSettingsView() {}

            void populate_dep_list_options(const paludis::Environment * env, paludis::DepListOptions & );
            void populate_install_task(const paludis::Environment *, paludis::InstallTask & task) const;

            DepListSettingsModel* m_model;
            QComboBox* m_dlDepsDefault;
            QComboBox* m_dlInstalledDepsPre;
            QComboBox* m_dlInstalledDepsRuntime;
            QComboBox* m_dlInstalledDepsPost;
            QComboBox* m_dlUninstalledDepsPre;
            QComboBox* m_dlUninstalledDepsRuntime;
            QComboBox* m_dlUninstalledDepsPost;
            QComboBox* m_dlUninstalledDepsSuggested;
            QComboBox* m_dlSuggested;
            QComboBox* m_dlBlocks;
            QComboBox* m_dlCircular;
            QListWidget* m_dlOverrideMasks;
            QComboBox* m_dlFallBack;
            QComboBox* m_dlDowngrade;
            QComboBox* m_dlNewSlots;
            QComboBox* m_dlReinstall;
            QComboBox* m_dlReinstallScm;
            QComboBox* m_dlReinstallTargets;
            QComboBox* m_dlUpgrade;

        private slots:

            void setCurrentIndex(QString name,Qt::CheckState state);
    };
}

#endif
