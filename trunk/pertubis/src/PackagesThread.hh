
/* Copyright (C) 2007 Stefan Koegl
*
* This file is part of perturbis
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


#ifndef _PERTUBIS_ENTRY_PROTECTOR_PACKAGES_THREAD_H
#define _PERTUBIS_ENTRY_PROTECTOR_PACKAGES_THREAD_H 1

#include "ThreadBase.hh"

#include <QString>


namespace perturbis
{
    class Package;
    class Selections;

    /*! \brief this thread fetches all packages in a specified category. Returns a complete item tree.
     * \ingroup PaludisAdapter
     * \ingroup Threads
     * \see TaskQueue, ThreadBase, CategoryThread, DetailsThread, SearchThread, SetThread
     */
    class PackagesThread : public ThreadBase
    {
        Q_OBJECT

        public:

            PackagesThread(QObject* parent,
                        const paludis::tr1::shared_ptr<paludis::Environment>&  myenv,
                        Selections* install,
                        Selections* deinstall);

            ~PackagesThread();

            void run();

            void setup(QString str);

        signals:

            void addPackage(Package* package);
            void changeInCat(QString);
            void resultCount(int count);

        private:

            QString                                    m_query;
            Selections*                                m_install;
            Selections*                                m_deinstall;
    };
}

#endif
