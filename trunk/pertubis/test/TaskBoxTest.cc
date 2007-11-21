
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
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/stringify.hh>
#include <paludis/dep_spec.hh>
#include <paludis/name.hh>
#include <paludis/environment_maker.hh>
#include "TaskBoxTest.hh"
#include "../src/TaskBox.hh"
#include "../src/InstallTask.hh"
#include "../src/Item.hh"

void pertubis::TaskBoxTest::addTask()
{
    paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    paludis::Log::get_instance()->set_program_name("TaskBoxTest");
    paludis::tr1::shared_ptr<paludis::Environment> env(paludis::EnvironmentMaker::get_instance()->make_from_spec(""));

    int index(-1);
    TaskBox* box = new TaskBox(this);
    InstallTask* install = new InstallTask();
    index = box->addTask(install);
    QCOMPARE(index,0);
    QCOMPARE(box->task(index),install);

    delete box;
    delete install;
}

void pertubis::TaskBoxTest::hasTask()
{
    paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    paludis::Log::get_instance()->set_program_name("TaskBoxTest");
    paludis::tr1::shared_ptr<paludis::Environment> env(paludis::EnvironmentMaker::get_instance()->make_from_spec(""));

    int index(-1);
    TaskBox* box = new TaskBox(this);
    InstallTask* install = new InstallTask();
    index = box->addTask(install);
    QCOMPARE(index,0);
    QCOMPARE(box->hasTask(index),true);

    delete box;
    delete install;
}

void pertubis::TaskBoxTest::tasks()
{
    paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    paludis::Log::get_instance()->set_program_name("TaskBoxTest");
    paludis::tr1::shared_ptr<paludis::Environment> env(paludis::EnvironmentMaker::get_instance()->make_from_spec(""));

    int index(-1);
    TaskBox* box = new TaskBox(this);
    InstallTask* install = new InstallTask();
    index = box->addTask(install);
    QVariantList list(box->tasks());

    QCOMPARE(list,QVariantList() << QVariant(false) );

    delete box;
    delete install;
}

void pertubis::TaskBoxTest::selectionData()
{
    paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    paludis::Log::get_instance()->set_program_name("TaskBoxTest");
    paludis::tr1::shared_ptr<paludis::Environment> env(paludis::EnvironmentMaker::get_instance()->make_from_spec(""));

    TaskBox* box = new TaskBox(this);
    InstallTask* install = new InstallTask();
    box->addTask(install);

    // first quering 2 id sequences
    paludis::tr1::shared_ptr<const paludis::PackageIDSequence> ids_1(
            env->package_database()->query(
            paludis::query::Matches(paludis::PackageDepSpec("sys-apps/paludis", paludis::pds_pm_eapi_0_strict)) &
            paludis::query::InstalledAtRoot(env->root()),
                                            paludis::qo_order_by_version));

    if (ids_1->empty() )
        QFAIL("no packages found");

    paludis::tr1::shared_ptr<const paludis::PackageIDSequence> ids_2(
            env->package_database()->query(
            paludis::query::Matches(paludis::PackageDepSpec("app-shells/bash", paludis::pds_pm_eapi_0_strict)) &
            paludis::query::InstalledAtRoot(env->root()),
                                            paludis::qo_order_by_version));

    if (ids_2->empty() )
        QFAIL("no packages found");

    install->addEntry(*ids_1->begin());
    install->addEntry(*ids_2->begin());

    QVariantList list_1 = box->selectionData(*ids_1->begin());
    QCOMPARE(list_1,QVariantList() << QVariant(2) );

    QVariantList list_2 = box->selectionData(*ids_2->begin());
    QCOMPARE(list_2,QVariantList() << QVariant(2) );

    install->deleteEntry(*ids_2->begin());
    QVariantList list_3 = box->selectionData(*ids_2->begin());
    QCOMPARE(list_3,QVariantList() << QVariant(0) );

    delete box;
    delete install;
}

void pertubis::TaskBoxTest::setItemTasks()
{
    paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    paludis::Log::get_instance()->set_program_name("TaskBoxTest");
    paludis::tr1::shared_ptr<paludis::Environment> env(paludis::EnvironmentMaker::get_instance()->make_from_spec(""));

    TaskBox* box = new TaskBox(this);
    InstallTask* install = new InstallTask();
    box->addTask(install);

    // init
    paludis::tr1::shared_ptr<const paludis::PackageIDSequence> ids_1(
            env->package_database()->query(
                                paludis::query::Matches(paludis::PackageDepSpec("sys-apps/paludis", paludis::pds_pm_eapi_0_strict)),
                                paludis::qo_order_by_version));

    if (ids_1->empty() )
        QFAIL("no packages found");

    install->addEntry(*ids_1->begin());
    Item* pitem = makePackageItem(*ids_1->begin(),
                                box->tasks(),
                                stringify( (*ids_1->begin())->name().package).c_str(),
                                stringify( (*ids_1->begin())->name().category).c_str(),
                                Qt::Unchecked,
                                Item::is_stable,
                                Item::ur_parent,
                                0,
                                "");
    for (paludis::PackageIDSequence::ConstIterator vstart(ids_1->begin()),vend(ids_1->end());
         vstart != vend;
         ++vstart)
    {
        Item* vitem = makeVersionItem(*ids_1->begin(),
                                    box->tasks(),
                                    stringify( (*ids_1->begin())->version()).c_str(),
                                    stringify( (*ids_1->begin())->repository()->name()).c_str(),
                                    Qt::Unchecked,
                                    Item::is_stable,
                                    Item::ur_parent,
                                    0,
                                    "");
        pitem->appendChild(vitem);
        pitem->setBestChild(vitem);
        box->setItemTasks(vitem);
        QVariantList list = vitem->data(Item::io_selected).toList();
        qDebug() << *vitem;
    }

    QCOMPARE(install->hasEntry(pitem->ID()),true);
    QCOMPARE(pitem->data(Item::io_selected).toList(),QVariantList() << QVariant(2) );
    QCOMPARE(pitem->bestChild()->data(Item::io_selected).toList(),QVariantList() << QVariant(2) );

    install->deleteEntry(pitem->ID());
    box->setItemTasks(pitem->bestChild());

    qDebug() << *pitem;
    qDebug() << *pitem->bestChild();
    QCOMPARE(install->hasEntry(pitem->ID()),false );
    QCOMPARE(pitem->bestChild()->data(Item::io_selected).toList(),QVariantList() << QVariant(Qt::Unchecked));
    QCOMPARE(pitem->data(Item::io_selected).toList(),QVariantList() << QVariant(Qt::PartiallyChecked) );

    delete box;
    delete install;
    delete pitem;
}

QTEST_MAIN(pertubis::TaskBoxTest)

