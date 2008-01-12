
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_GENERAL_SETTINGS_H
#define _PERTUBIS_ENTRY_PROTECTOR_GENERAL_SETTINGS_H 1

#include <QWidget>
#include <QString>
#include <QMap>

class QSpinBox;
class QFontComboBox;

namespace pertubis
{
    class GeneralSettingsModel : public QObject
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            GeneralSettingsModel(QObject *parent);
            ///@}

            ~GeneralSettingsModel();

            ///@name Content modification
            ///@{

            /// loads all settings for the gui
            void loadSettings();

            /// saves all settings for the gui
            void saveSettings();

            ///@}

            /// \name "real" option data
            ///@{

            /// maps tr() language name to translation file
            QMap<QString,QString>  m_languageToTranslation;

            QString     m_currentLanguage;

            ///@}
    };

    class GeneralSettingsView : public QWidget
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            GeneralSettingsView(QWidget *parent, GeneralSettingsModel* model);
            ///@}

            ~GeneralSettingsView();

            ///@name Content modification
            ///@{

            ///@}

            GeneralSettingsModel*   m_model;
            QFontComboBox*          m_fonts;
            QSpinBox*               m_fontSize;

        private slots:

            void languageChanged(const QString& language);
    };
}

#endif
