
/* Copyright (C) 2007 Stefan Koegl
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

class QComboBox;
class QButtonGroup;

namespace pertubis
{
    class QuerySettingsModel : public QObject
    {
        Q_OBJECT

        public:


            /// std constructor
            QuerySettingsModel(QObject *parent);

            /// destructor
            virtual ~QuerySettingsModel();

            /// \name data
            ///@{

            int m_kindModel;
            int m_matcherModel;
            ///@}

        signals:
            void kindChanged(int value);
            void matcherChanged(int value);

        private slots:

            void onKindChanged(int value);
            void onMatcherChanged(int value);
    };

    class QuerySettingsView : public QWidget
    {
        Q_OBJECT

        public:

            /// std constructor
            QuerySettingsView(QWidget *parent,QuerySettingsModel* model);

            /// destructor
            virtual ~QuerySettingsView() {}

            QuerySettingsModel* m_model;

        private slots:

            void setKind(int value);
            void setMatcher(int value);

        private:

            QButtonGroup*       m_kindButtonGroup;
            QButtonGroup*       m_matcherButtonGroup;
    };
}

#endif
