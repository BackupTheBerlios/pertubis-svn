
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_THREAD_FETCH_PACKAGES_H
#define _PERTUBIS_ENTRY_PROTECTOR_THREAD_FETCH_PACKAGES_H

#include "ThreadBase.hh"


namespace paludis
{
    class Environment;
}

class QMenu;

#include <QStringList>
#include <set>

namespace pertubis
{
    class ThreadKeywordManager;
    class ThreadFetchVersions;
    class Item;
    class TaskBox;

    /*! \brief this thread fetches all packages in a specified category. Returns a complete item tree.
    *
    */
    class ThreadFetchPackages : public ThreadBase
    {
        Q_OBJECT

        public:

            ThreadFetchPackages(QObject* parent,
                                paludis::tr1::shared_ptr<paludis::Environment> env,
                                ThreadKeywordManager* keywords,
                            TaskBox* box);

            ~ThreadFetchPackages();

            void run();

            void makePackageItems() const;
            void searchPackages(QString str);

        private:

            void fetchPackages();

            QString                                    m_query;
            ThreadKeywordManager*                    m_keywords;
            TaskBox*                                m_box;

        signals:
            void packagesResult(Item* root);
    };
}
#endif


