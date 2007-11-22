
/* Copyright (C) 2007 Stefan Koegl.
*
* This file is part of pertubis.
*
* pertubis is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* pertubis is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http:*www.gnu.org/licenses/>.
*/

#include "DatabaseView.hh"
#include "ThreadBase.hh"

#include <paludis/environment.hh>
#include <paludis/query.hh>

#include <paludis/util/wrapped_forward_iterator.hh>
#include <paludis/version_operator.hh>
#include <paludis/version_requirements.hh>
#include <paludis/package_id.hh>
#include <paludis/package_database.hh>
#include <paludis/util/sequence.hh>


pertubis::ThreadBase::ThreadBase(QObject* pobj,
                                DatabaseView* main) : QThread(pobj), m_main(main)
{
}

bool pertubis::ThreadBase::installed(const paludis::tr1::shared_ptr<const paludis::PackageID>& id)
{
    using namespace paludis;
    tr1::shared_ptr<VersionRequirements> req(new VersionRequirements());
    req->push_back(VersionRequirement(VersionOperator(vo_equal),id->version()));
    tr1::shared_ptr<const PackageIDSequence> ipacks(
        m_main->getEnv()->package_database()->query(
            query::InstalledAtRoot(m_main->getEnv()->root()) &
            query::Matches(PackageDepSpec(
                tr1::shared_ptr<QualifiedPackageName>(new QualifiedPackageName(id->name())),
                tr1::shared_ptr<CategoryNamePart>(),
                tr1::shared_ptr<PackageNamePart>(),
                req,
                vr_and,
                tr1::shared_ptr<SlotName>(new SlotName(id->slot())))),
            qo_order_by_version));
    return (ipacks->begin() != ipacks->end());
}

bool pertubis::ThreadBase::hasVersionChange(const paludis::tr1::shared_ptr<const paludis::PackageID>& id)
{
    using namespace paludis;
    tr1::shared_ptr<const PackageIDSequence> ci(
        m_main->getEnv()->package_database()->query(
            query::InstalledAtRoot(m_main->getEnv()->root()) &
            query::Matches(PackageDepSpec(
                tr1::shared_ptr<QualifiedPackageName>(new QualifiedPackageName(id->name())),
                tr1::shared_ptr<CategoryNamePart>(),
                tr1::shared_ptr<PackageNamePart>(),
                tr1::shared_ptr<VersionRequirements>(),
                vr_and,
                tr1::shared_ptr<SlotName>(new SlotName(id->slot())))),
            qo_order_by_version));

        tr1::shared_ptr<const PackageIDSequence> av(
            m_main->getEnv()->package_database()->query(
                query::SupportsAction<InstallAction>() &
                query::Matches(PackageDepSpec(
                    tr1::shared_ptr<QualifiedPackageName>(new QualifiedPackageName(id->name())),
                    tr1::shared_ptr<CategoryNamePart>(),
                    tr1::shared_ptr<PackageNamePart>(),
                    tr1::shared_ptr<VersionRequirements>(),
                    vr_and,
                    tr1::shared_ptr<SlotName>(new SlotName(id->slot())))) &
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

