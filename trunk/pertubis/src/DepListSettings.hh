
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

    class DepListSettings : public QWidget
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            DepListSettings(QWidget *parent);
            ///@}

            ~DepListSettings();

            ///@name Content modification
            ///@{

            /// loads all settings for the gui
            void loadSettings();

            /// saves all settings for the gui
            void saveSettings();
            ///@}

            /// \name deplist options
            ///@{

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
