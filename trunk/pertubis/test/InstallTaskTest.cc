
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

#include <paludis/util/sequence.hh>

#include <paludis/environment.hh>

#include <paludis/package_database.hh>
#include <paludis/package_id.hh>
#include <paludis/query.hh>
#include <paludis/util/log.hh>
#include <libwrapiter/libwrapiter_forward_iterator.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>
#include <paludis/dep_spec.hh>
#include <paludis/name.hh>
#include <paludis/environments/environment_maker.hh>
#include "InstallTaskTest.hh"
#include "../src/InstallTask.hh"
#include "../src/TaskBox.hh"

void pertubis::InstallTaskTest::getSetTaskid()
{
    InstallTask* install = new InstallTask();
//     install->setTaskid(0);
//     QCOMPARE(install->taskid(), 0);
    delete install;
}

void pertubis::InstallTaskTest::addEntry()
{
    paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    paludis::Log::get_instance()->set_program_name("test-ItemTest");
    paludis::tr1::shared_ptr<paludis::Environment> env(paludis::EnvironmentMaker::get_instance()->make_from_spec(""));
    paludis::tr1::shared_ptr<const paludis::PackageIDSequence> versions(
        env->package_database()->query(
            paludis::query::Matches(paludis::PackageDepSpec("sys-apps/paludis", paludis::pds_pm_eapi_0_strict)) &
            paludis::query::InstalledAtRoot(env->root()),
            paludis::qo_order_by_version));
    paludis::PackageIDSequence::Iterator vstart(versions->begin()),vend(versions->end());
    if (versions->empty() )
        QFAIL("no packages found");
    InstallTask* install = new InstallTask();
    install->changeEntry(*versions->begin(),true);
    QCOMPARE(install->hasEntry(*versions->begin()),true);
    install->changeEntry(*versions->begin(),false);
    QCOMPARE(install->hasEntry(*versions->begin()),false);
    install->changeEntry(*versions->begin(),true);
    versions = env->package_database()->query(
            paludis::query::Matches(paludis::PackageDepSpec("sys-apps/paludis", paludis::pds_pm_eapi_0_strict)) &
            paludis::query::InstalledAtRoot(env->root()),
            paludis::qo_order_by_version);

    if (versions->empty() )
        QFAIL("no packages found");

    QCOMPARE(install->hasEntry(*versions->begin()),true);
    delete install;
}

QTEST_MAIN(pertubis::InstallTaskTest)
