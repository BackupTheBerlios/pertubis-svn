
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_QUERY_SETTINGS_H
#define _PERTUBIS_ENTRY_PROTECTOR_QUERY_SETTINGS_H 1

#include <QWidget>
#include <QSet>
#include <QDebug>

class QCheckBox;
class QComboBox;
class QStandardItemModel;
class QItemSelectionModel;
class QTableView;
class QButtonGroup;


namespace pertubis
{

    class QuerySettingsModel : public QObject
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            QuerySettingsModel(QObject *parent);
            ///@}

            ~QuerySettingsModel();

            ///@name Content modification
            ///@{

            /// loads all settings for the gui
            void loadSettings();

            /// saves all settings for the gui
            void saveSettings();

            ///@}

            /// \name data
            ///@{


            int m_kindModel;
            int m_matcherModel;
            ///@}

        signals:
            void kindChanged(int value);
            void matcherChanged(int value);

        private slots:

            void onKindChanged(int value)
            {
                if (m_kindModel != value)
                    emit kindChanged(value);
                m_kindModel = value;
            }

            void onMatcherChanged(int value)
            {
                if (m_matcherModel != value)
                    emit matcherChanged(value);
                m_matcherModel = value;
            }
    };

    class QuerySettingsView : public QWidget
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            QuerySettingsView(QWidget *parent,QuerySettingsModel* model);
            ///@}

            ~QuerySettingsView();

            ///@name Content modification
            ///@{

            /// loads all settings for the gui
            void loadSettings();

            /// saves all settings for the gui
            void saveSettings();

            ///@}

            /// \name install options
            ///@{

            QuerySettingsModel* m_model;
            QButtonGroup*       m_kindGroup;
            QButtonGroup*       m_matcherGroup;
            QCheckBox*          m_nameExtractor;
            QCheckBox*          m_descExtractor;
            ///@}

        private slots:

            void setKind(int value);
            void setMatcher(int value);
    };
}

#endif

