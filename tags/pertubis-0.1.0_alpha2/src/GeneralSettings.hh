
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

class QCheckBox;
class QLineEdit;
class QComboBox;

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

             /// maps language to translation file
            QMap<QString,QString>  m_languageToTranslation;
            QString     m_currentLanguage;
            int         m_toolbarDisplayMode;
            int         m_categoryDockPosition;
            int         m_repositoryDockPosition;
            int         m_setDockPosition;
            int         m_glsaDockPosition;

            ///@}

        signals:

            void toolbarDisplayModeChanged(int state);
            void glsaDockChanged(int state);
            void repositoryDockChanged(int state);
            void setDockChanged(int state);
            void categoryDockChanged(int state);


        public slots:

            void changeToolbarDisplayMode(int state)
            {
                if (m_toolbarDisplayMode != state)
                    emit toolbarDisplayModeChanged(state);
                m_toolbarDisplayMode = state;
            }

            void changeCategoryDockPosition(int state)
            {
                if (m_categoryDockPosition != state)
                    emit categoryDockChanged(state);
                m_categoryDockPosition = state;
            }

            void changeGlsaDockPosition(int state)
            {
                if (m_glsaDockPosition != state)
                    emit glsaDockChanged(state);
                m_glsaDockPosition = state;
            }

            void changeSetDockPosition(int state)
            {
                if (m_setDockPosition != state)
                    emit setDockChanged(state);
                m_setDockPosition = state;
            }

            void changeRepositoryDockPosition(int state)
            {
                if (m_repositoryDockPosition != state)
                    emit repositoryDockChanged(state);
                m_repositoryDockPosition = state;
            }


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

            GeneralSettingsModel* m_model;

        private:

            /// \name install options
            ///@{

            QComboBox* m_toolbarDisplayMode;
            QComboBox* m_categoryDockPosition;
            QComboBox* m_repositoryDockPosition;
            QComboBox* m_setDockPosition;
            QComboBox* m_glsaDockPosition;
            ///@}

        private slots:

            void languageChanged(const QString& language);
    };
}

#endif
