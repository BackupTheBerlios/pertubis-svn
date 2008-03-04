
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_MAKE_PACKAGE_H
#define _PERTUBIS_ENTRY_PROTECTOR_MAKE_PACKAGE_H 1

#include "Package-fwd.hh"
#include <Qt>
#include <paludis/package_id-fwd.hh>
#include <paludis/util/tr1_memory.hh>

namespace perturbis
{

    /** \brief creates a node to diplay a package, can have subnodes
     * \ingroup PackageModelClass
     *
     */
    Package* makePackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                         Qt::CheckState install,
                         Qt::CheckState deinstall,
                         QString pack,
                         QString cat,
                         Qt::CheckState isInstalled,
                         PackageState mystate,
                         Package* myparent,
                         QString mask_reasons);

    /** \brief creates a subnode to display a version
     * \ingroup PackageModelClass
     *
     */
    Package* makeVersionPackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                                Qt::CheckState install,
                                Qt::CheckState deinstall,
                                QString version,
                                QString rep,
                                Qt::CheckState isInstalled,
                                PackageState mystate,
                                Package* pitem,
                                QString mask_reasons);

    /** \brief creates a simple root Package instance
     * \ingroup PackageModelClass
     *
     */
    Package* makeRootPackage();
}

#endif
