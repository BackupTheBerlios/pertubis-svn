
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of the pertubis frontend for paludis package manager.
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_USEFLAG_EDITOR_H
#define _PERTUBIS_ENTRY_PROTECTOR_USEFLAG_EDITOR_H

#include <QTableWidget>
#include <QVariant>

#include <paludis/util/tr1_memory.hh>

#include <paludis/environment.hh>

class QToolBar;
class QSplitter;
class QCloseEvent;

namespace pertubis
{
    class UseFlagModel;

    /*! \brief Presents the information we find in UseFlagModel and allows the user to add/change/delete it.
    *
    */
    class UseFlagEditor : public QWidget
    {
        Q_OBJECT

        public:

            UseFlagEditor(QWidget* parent,
                          std::tr1::shared_ptr<paludis::Environment> env);
            ~UseFlagEditor();

            void saveSettings();
            void loadSettings();

            void createToolbar();
            void setModel(UseFlagModel* umodel);

            UseFlagModel* model() { return m_model;}

        private:

            QTableWidget*        m_setView;
            QTableWidget*        m_table;
            QSplitter*            m_hSplit;
            UseFlagModel*        m_model;
            QToolBar*            m_toolbar;

            std::tr1::shared_ptr<paludis::Environment> m_env;

        private slots:

            void slotUseflagsChanged(QTableWidgetItem * current, QTableWidgetItem * previous );
            void slotSetResult(QStringList list);
            void slotUseflagResult(QList<QVariantList> list);
            void slotToggleSetTable();
    };
};

#endif
