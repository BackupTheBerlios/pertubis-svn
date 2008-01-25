
/* Copyright (C) 2007-2008 Stefan Koegl <hotshelf@users.berlios.de>
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_DEINSTALL_TASK_H
#define _PERTUBIS_ENTRY_PROTECTOR_DEINSTALL_TASK_H 1

#include "Selections.hh"

namespace pertubis
{
    /*! \brief selection storage for package deinstallation
     * \ingroup Selection
     * Here we store the packages the user wishes to deinstall
     */

    class DeinstallSelections : public Selections
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            DeinstallSelections(QObject* pobject,
                        QString tname) : Selections(pobject,tname) {}

            virtual ~DeinstallSelections() {}

            ///@}

            /// @name Content information
            ///@{

            /** \brief returns true if the Package can be uninstalled
             *
             * \todo why we really need this crap?
             */
            bool available(Package* item, int column) const;

            /*! @brief makes the actual changes in this object and in the Package object when a user visually changes the selection state
             * @param item the Package to be changed
             * @param mystate the new selection state for this Selections
             */
            bool changeStates(Package* item, int mystate, int column);
            ///@}
    };
}

#endif
