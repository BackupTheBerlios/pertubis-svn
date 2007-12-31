
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_INSTALL_TASK_H
#define _PERTUBIS_ENTRY_PROTECTOR_INSTALL_TASK_H 1

#include "Selections.hh"

namespace pertubis
{
    /*! \brief selection storage for package installation
    * \ingroup Selection
    */
    class InstallSelections :
        public Selections
    {
        Q_OBJECT

    public:

        /// std constructor
        InstallSelections(QObject* pobject,
                          QAction* myaction,
                          QString tname,
                          PackageOrder pos);

        virtual ~InstallSelections() {}

        /// @name Content information
        ///@{

            /// returns if its not masked
        bool available(Package* item) const;

        ///@}

        /// @name Content modification
        ///@{

        /*! @brief makes the actual changes in this object and in the Package object when a user visually changes a selection state
         * @param item the Package to be changed
         * @param mystate the new selection state for this Selections
         */
        bool changeStates(Package* item, int mystate);

        ///@}

    signals:

        /// the paludis install task successfully finished
        void finished();

        void depListResult(QString);
    };
}

#endif
