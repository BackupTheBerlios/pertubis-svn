
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_INSTALL_SETTINGS_H
#define _PERTUBIS_ENTRY_PROTECTOR_INSTALL_SETTINGS_H 1

#include <QWidget>

class QCheckBox;
class QComboBox;

namespace pertubis
{

    class InstallSettings : public QWidget
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            InstallSettings(QWidget *parent);
            ///@}

            ~InstallSettings();

            ///@name Content modification
            ///@{

            /// loads all settings for the gui
            void loadSettings();

            /// saves all settings for the gui
            void saveSettings();

            ///@}

        private:

            /// \name install options
            ///@{
            QCheckBox* m_debug;
            QCheckBox* m_fetch;
            QCheckBox* m_noConfigProtection;
            QCheckBox* m_noSafeResume;
            QCheckBox* m_pretend;
            QComboBox* m_continueOnFailure;
            ///@}

    };
}

#endif
