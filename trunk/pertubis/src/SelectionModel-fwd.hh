
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SELECTION_MODEL_FWD_H
#define _PERTUBIS_ENTRY_PROTECTOR_SELECTION_MODEL_FWD_H 1

namespace pertubis
{
    class Package;
    class TaskBox;

    enum SelectionPackageHeaderOrder
    {
        spho_install,
        spho_deinstall,
        spho_installed,
        spho_package,
        spho_category,
        spho_old_version,
        spho_change_sign,
        spho_new_version,
        spho_repository,
        spho_last
    };


}

#endif
