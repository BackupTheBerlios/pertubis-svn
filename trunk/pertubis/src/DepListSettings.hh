
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


class QComboBox;

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

            ~DepListSettingsModel();

            ///@name Content modification
            ///@{

            /// loads all settings for the gui
            void loadSettings();

            /// saves all settings for the gui
            void saveSettings();
            ///@}

            /// \name deplist options
            ///@{

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
           int m_dlOverrideMasks;
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
            void overrideChanged(int);
            void fallbackChanged(int);
            void downgradeChanged(int);
            void newSlotsChanged(int);
            void reinstallChanged(int);
            void reinstallScmChanged(int);
            void reinstallTargetChanged(int);
            void upgradeChanged(int);

        public slots:

            void changeDepsDefault(int state)
            {
                if (m_dlDepsDefault != state)
                    emit depsDefaultChanged(state);
                m_dlDepsDefault = state;
            }

            void changeInstallDepsPre(int state)
            {
                if (m_dlInstalledDepsPre != state)
                    emit installDepsPreChanged(state);
                m_dlInstalledDepsPre = state;
            }

            void changeInstallDepsRuntime(int state)
            {
                if (m_dlInstalledDepsRuntime != state)
                    emit installDepsRuntimeChanged(state);
                m_dlInstalledDepsRuntime = state;
            }

            void changeInstallDepsPost(int state)
            {
                if (m_dlInstalledDepsPost != state)
                    emit installDepsPostChanged(state);
                m_dlInstalledDepsPost = state;
            }

            void changeUninstallDepsPre(int state)
            {
                if (m_dlUninstalledDepsPre != state)
                    emit uninstallDepsPreChanged(state);
                m_dlUninstalledDepsPre = state;
            }

            void changeUninstallDepsRuntime(int state)
            {
                if (m_dlUninstalledDepsRuntime != state)
                    emit uninstallDepsRuntimeChanged(state);
                m_dlUninstalledDepsRuntime = state;
            }

            void changeUninstallDepsPost(int state)
            {
                if (m_dlUninstalledDepsPost != state)
                    emit uninstallDepsPostChanged(state);
                m_dlUninstalledDepsPost = state;
            }

            void changeUninstallDepsSuggested(int state)
            {
                if (m_dlUninstalledDepsSuggested != state)
                    emit uninstallDepsSuggestedChanged(state);
                m_dlUninstalledDepsSuggested = state;
            }

            void changeSuggested(int state)
            {
                if (m_dlSuggested != state)
                    emit suggestedChanged(state);
                m_dlSuggested = state;
            }

            void changeBlocks(int state)
            {
                if (m_dlBlocks != state)
                    emit blocksChanged(state);
                m_dlBlocks = state;
            }
            void changeCircular(int state)
            {
                if (m_dlCircular != state)
                    emit circularChanged(state);
                m_dlCircular = state;
            }
            void changeOverride(int state)
            {
                if (m_dlOverrideMasks != state)
                    emit overrideChanged(state);
                m_dlOverrideMasks = state;
            }

            void changeFallback(int state)
            {
                if (m_dlFallBack != state)
                    emit fallbackChanged(state);
                m_dlFallBack = state;
            }

            void changeDowngrade(int state)
            {
                if (m_dlDowngrade != state)
                    emit downgradeChanged(state);
                m_dlDowngrade = state;
            }

            void changeNewSlots(int state)
            {
                if (m_dlNewSlots != state)
                    emit newSlotsChanged(state);
                m_dlNewSlots = state;
            }

            void changeReinstall(int state)
            {
                if (m_dlSuggested != state)
                    emit reinstallChanged(state);
                m_dlReinstall = state;
            }

            void changeReinstallScm(int state)
            {
                if (m_dlReinstallScm != state)
                    emit reinstallScmChanged(state);
                m_dlReinstallScm = state;
            }

            void changeReinstallTarget(int state)
            {
                if (m_dlReinstallTargets != state)
                    emit reinstallTargetChanged(state);
                m_dlReinstallTargets = state;
            }

            void changeUpgrade(int state)
            {
                if (m_dlUpgrade != state)
                    emit upgradeChanged(state);
                m_dlUpgrade = state;
            }
    };

    class DepListSettingsView : public QWidget
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            DepListSettingsView(QWidget *parent, DepListSettingsModel* model);
            ///@}

            ~DepListSettingsView();

            ///@name Content modification
            ///@{


            /// \name deplist options
            ///@{

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
            QComboBox* m_dlOverrideMasks;
            QComboBox* m_dlFallBack;
            QComboBox* m_dlDowngrade;
            QComboBox* m_dlNewSlots;
            QComboBox* m_dlReinstall;
            QComboBox* m_dlReinstallScm;
            QComboBox* m_dlReinstallTargets;
            QComboBox* m_dlUpgrade;
    };
}

#endif
