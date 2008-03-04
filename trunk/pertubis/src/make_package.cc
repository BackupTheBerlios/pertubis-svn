
/* Copyright (C) 2007-2008 Stefan Koegl
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


#include "make_package.hh"
#include "Package.hh"

perturbis::Package*
perturbis::makePackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                                        Qt::CheckState install,
                                        Qt::CheckState deinstall,
                                        QString package,
                                        QString cat,
                                        Qt::CheckState isInstalled,
                                        PackageState mystate,
                                        Package* pitem,
                                        QString mask_reasons)
{
    Package* pack(new Package(id,QVector<QVariant>(pho_last),mystate,pt_parent,pitem));
    pack->setData(pho_install,install);
    pack->setData(pho_deinstall,deinstall);
    pack->setData(pho_package,package);
    pack->setData(pho_category,cat);
    pack->setData(pho_repository, "");
    pack->setData(pho_installed,QVariant(static_cast<int>(isInstalled)));
    pack->setData(pho_mask_reasons,mask_reasons);
    return pack;
}

perturbis::Package*
perturbis::makeRootPackage()
{
    return new Package(paludis::tr1::shared_ptr<const paludis::PackageID>(),
                       QVector<QVariant>(pho_last),
                       ps_stable,
                       pt_parent,
                       0);
}

perturbis::Package*
perturbis::makeVersionPackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
        Qt::CheckState install,
        Qt::CheckState deinstall,
        QString version,
        QString rep,
        Qt::CheckState isInstalled,
        PackageState mystate,
        Package* pitem,
        QString mask_reasons)
{
    Package* pack(new Package(id,QVector<QVariant>(pho_last),mystate,pt_child,pitem));
    pack->setData(pho_install,install);
    pack->setData(pho_deinstall,deinstall);
    pack->setData(pho_package,version);
    pack->setData(pho_category,"");
    pack->setData(pho_repository, rep);
    pack->setData(pho_installed,QVariant(static_cast<int>(isInstalled)));
    pack->setData(pho_mask_reasons,mask_reasons);
    return pack;
}
