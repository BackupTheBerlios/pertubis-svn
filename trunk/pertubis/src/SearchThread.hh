
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SEARCH_THREAD_H
#define _PERTUBIS_ENTRY_PROTECTOR_SEARCH_THREAD_H 1

#include "ThreadBase.hh"
#include <QString>

namespace pertubis
{
    class Item;
    class DatabaseView;

    /*! \brief this thread is used in conjunction with SearchWindow. Returns a complete item tree.
    *
    */
    class SearchThread : public ThreadBase
    {
        Q_OBJECT

    public:

        SearchThread(QObject* parent,
                    DatabaseView* main);

        void start(const QString& str,bool name,bool desc);

    protected:

        void run();

    private:

        QString                                 m_query;
        bool                                    m_optName;
        bool                                    m_optDesc;

    signals:
        void itemResult(Item* root);
        void changeInCat(QString cat);
        void finished(int resultCount);
    };
}

#endif
