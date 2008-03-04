
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

#include "PaludisUtils.hh"

#include <paludis/environment.hh>
#include <paludis/mask.hh>
#include <paludis/metadata_key.hh>
#include <paludis/package_database.hh>
#include <paludis/package_id.hh>
#include <paludis/query.hh>
#include <paludis/util/sequence.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/wrapped_forward_iterator.hh>
#include <paludis/version_operator.hh>
#include <paludis/version_requirements.hh>

bool
perturbis::installed(const paludis::tr1::shared_ptr<paludis::Environment>&  env,
                         const paludis::tr1::shared_ptr<const paludis::PackageID>& id)
{
    using namespace paludis;
    tr1::shared_ptr<const PackageIDSequence> ipacks(
        env->package_database()->query(
            query::InstalledAtRoot(env->root()) &
            query::Matches(make_package_dep_spec()
                .package(id->name())
                .slot(id->slot())
                .version_requirement(VersionRequirement(VersionOperator(vo_equal), id->version()))),
            qo_order_by_version));
    return (ipacks->begin() != ipacks->end());
}

bool
perturbis::hasVersionChange(const paludis::tr1::shared_ptr<paludis::Environment>&  m_env,
                                const paludis::tr1::shared_ptr<const paludis::PackageID>& id)
{
    using namespace paludis;
    tr1::shared_ptr<const PackageIDSequence> ci(
        m_env->package_database()->query(
            query::InstalledAtRoot(m_env->root()) &
            query::Matches(make_package_dep_spec()
            .package(id->name())
            .slot(id->slot())),
            qo_order_by_version));

    tr1::shared_ptr<const PackageIDSequence> av(
        m_env->package_database()->query(
            query::SupportsAction<InstallAction>() &
            query::Matches(make_package_dep_spec()
                .package(id->name())
                .slot(id->slot())) &
            query::NotMasked(),
            qo_order_by_version));

    if (! ci->empty())
    {
        if (! av->empty())
        {
            if ((*av->last())->version() < (*ci->last())->version() ||
                  (*av->last())->version() > (*ci->last())->version())
            {
                return true;
            }
        }
    }
    return false;
}
