
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_DETAILS_THREAD_H
#define _PERTUBIS_ENTRY_PROTECTOR_DETAILS_THREAD_H 1

#include "ThreadBase.hh"

#include <paludis/util/tr1_memory.hh>

#include <QString>
#include <string>

namespace paludis
{
    class PackageID;
}

namespace pertubis
{
    class DatabaseView;

    /*! \brief this thread asks pertubis for package details and returns preformatted text.
    *
    */
    class DetailsThread : public ThreadBase
    {
        Q_OBJECT

    public:

        DetailsThread(QObject* pobj,
                           pertubis::DatabaseView* main);
        void start(paludis::tr1::shared_ptr<const paludis::PackageID> id);
        void appendOutput(std::string text);

        void run();

    private:

        paludis::tr1::shared_ptr<const paludis::PackageID> m_id;
        mutable QString    m_output;

    signals:
        void detailsResult(QString output);
    };
}

#endif


