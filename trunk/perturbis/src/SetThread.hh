
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SET_THREAD_H
#define _PERTUBIS_ENTRY_PROTECTOR_SET_THREAD_H 1

#include "ThreadBase.hh"
#include <QList>
#include <QMap>
#include <QSet>
#include <QString>

namespace perturbis
{
    class CategoryItem;

    /*! \brief thread for fetching categories from repositories and returns a list of names
     *
     * \ingroup PaludisAdapter
     * \ingroup Threads
     * \see TaskQueue, ThreadBase, CategoryThread, DetailsThread, PacackagesThread, SearchThread, SetThread
     */
    class SetThread : public ThreadBase
    {
        Q_OBJECT

        public:

            ///\name Constructors
            ///\{

            /// constructs a SetThread object
            SetThread(QObject* parent,
                           const paludis::tr1::shared_ptr<paludis::Environment>&  env);
            ///@}

        protected:

            /// overloaded from QThread
            void run();

        signals:

            /// sends data for a new Set to create
            void sendSet(QMap<QString, QSet<QString> >);
    };
}

#endif
