
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

bool pertubis::ThreadBase::installed(paludis::tr1::shared_ptr<const paludis::PackageID> id)
{
    paludis::tr1::shared_ptr<paludis::VersionRequirements> req(new paludis::VersionRequirements());
    req->push_back(paludis::VersionRequirement(paludis::VersionOperator(paludis::vo_equal),id->version()));
    paludis::tr1::shared_ptr<const paludis::PackageIDSequence> ipacks(
            m_main->getEnv()->package_database()->query(
                           paludis::query::InstalledAtRoot(m_main->getEnv()->root()) &
            paludis::query::Matches(paludis::PackageDepSpec(
                                    paludis::tr1::shared_ptr<paludis::QualifiedPackageName>(new paludis::QualifiedPackageName(id->name())),
            paludis::tr1::shared_ptr<paludis::CategoryNamePart>(),
                    paludis::tr1::shared_ptr<paludis::PackageNamePart>(),
                            req,
                            paludis::vr_and,
                            paludis::tr1::shared_ptr<paludis::SlotName>(new paludis::SlotName(id->slot())))),
            paludis::qo_order_by_version));
    return (ipacks->begin() != ipacks->end());
}
