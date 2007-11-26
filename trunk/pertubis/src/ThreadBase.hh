
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_THREAD_BASE_H
#define _PERTUBIS_ENTRY_PROTECTOR_THREAD_BASE_H 1

#include <QThread>
#include <paludis/util/tr1_memory.hh>
#include <paludis/environment-fwd.hh>
#include <paludis/package_id-fwd.hh>

namespace pertubis
{
    class TaskBox;

    /*! \brief provides the paludis infrastructure we need for the threads
    *
    */
    class ThreadBase : public QThread
    {
        Q_OBJECT

    public:

        ThreadBase(QObject* parent,
                    const paludis::tr1::shared_ptr<paludis::Environment>&  env,
                    TaskBox* box);


    protected:

        bool installed(const paludis::tr1::shared_ptr<const paludis::PackageID>& id);
        bool hasVersionChange(const paludis::tr1::shared_ptr<const paludis::PackageID>& id);

        paludis::tr1::shared_ptr<paludis::Environment>  m_env;
        TaskBox*                                        m_taskbox;
    };
}

#endif
