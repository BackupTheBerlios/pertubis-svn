
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_THREAD_SHOW_SELECTIONS_THREAD_H
#define _PERTUBIS_ENTRY_PROTECTOR_THREAD_SHOW_SELECTIONS_THREAD_H 1

#include "ThreadBase.hh"

namespace pertubis
{
    class Item;

    /*! \brief gathering information from paludis showing the selected packages the user has selected for tasks
     * \ingroup PaludisAdapter
     * \ingroup Thread
    */
    class ShowSelectionsThread : public ThreadBase
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            ShowSelectionsThread(QObject* pobj,
                                 const paludis::tr1::shared_ptr<paludis::Environment>&  env,
                                 TaskBox* box) : ThreadBase(pobj,env,box)
            {
            }
            ///@}

        signals:

            /// sends an new Item to append
            void appendPackage(Item* item);

        protected:

            /// overloaded from QThread
            void run();

    };
}


#endif

