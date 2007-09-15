
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_VIEW_H
#define _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_VIEW_H

#include <QTableView>

namespace pertubis
{

    class RepositoryModel;

    /*! \brief not finished
    *
    */
    class RepositoryView : public QTableView
    {
        Q_OBJECT

    public:

        RepositoryView(QWidget* parent);
    //     ~RepositoryView(QObject* parent)
    private:
        RepositoryModel*   m_model;
    };
}

#endif

