
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_DETAILS_THREAD_H
#define _PERTUBIS_ENTRY_PROTECTOR_DETAILS_THREAD_H 1

#include "ThreadBase.hh"

#include <QString>
#include <QVariantList>
#include <string>
#include <paludis/package_id-fwd.hh>

namespace pertubis
{
    class DatabaseView;

    /*! \brief this thread asks pertubis for package details and returns preformatted text.
     *
     * \ingroup PaludisAdapter
     * \ingroup Threads
     * \see TaskQueue, ThreadBase, CategoryThread, PackagesThread, SearchThread, SetThread
     */
    class DetailsThread : public ThreadBase
    {
        Q_OBJECT

    public:

        DetailsThread(QObject* pobj,
                      const paludis::tr1::shared_ptr<paludis::Environment>&  env);
        ~DetailsThread();

        void setup(paludis::tr1::shared_ptr<const paludis::PackageID> id);

        void appendOutput(const std::string& row) { m_text.append(QString::fromStdString(row)); }

        void run();

    private:

        paludis::tr1::shared_ptr<const paludis::PackageID> m_id;
        mutable QString    m_text;

    signals:
        void sendResult(QString);
    };
}

#endif


