
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_FWD_H
#define _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_FWD_H 1

namespace perturbis
{
    class Package;
    class PartiallyMadePackage;
    class PartiallyMadePackageData;

    enum PackageState
    {
        ps_stable,
        ps_unstable,
        ps_masked,
        ps_last
    };

    enum PackageHeaderOrder
    {
        pho_install,
        pho_deinstall,
        pho_installed,
        pho_package,
        pho_category,
        pho_repository,
        pho_mask_reasons,
        pho_last
    };

    enum PackageType
    {
        pt_nothing,
        pt_parent,
        pt_child,
        pt_node_full,
        pt_node_only,
        pt_last
    };
}

#endif
