
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
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
#include "../src/Package.hh"

void pertubis::TaskBoxTest::addTask()
{
    paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    paludis::Log::get_instance()->set_program_name("TaskBoxTest");
    paludis::tr1::shared_ptr<paludis::Environment> env(paludis::EnvironmentMaker::get_instance()->make_from_spec(""));

    int index(-1);
    TaskBox* box = new TaskBox(this);
    InstallTask* install = new InstallTask(0,0,"install");
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
    InstallTask* install = new InstallTask(0,0,"install");
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
    InstallTask* install = new InstallTask(0,0,"install");
    index = box->addTask(install);
    QVariantList list(box->tasks());

    QCOMPARE(list,QVariantList() << QVariant(false) );

    delete box;
    delete install;
}

void pertubis::TaskBoxTest::selectionData()
{
    qDebug() << "1";
    paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    qDebug() << "2";
    paludis::Log::get_instance()->set_program_name("TaskBoxTest");
    qDebug() << "3";
    paludis::tr1::shared_ptr<paludis::Environment> env(paludis::EnvironmentMaker::get_instance()->make_from_spec(""));
    qDebug() << "4";
    TaskBox* box = new TaskBox(this);
    qDebug() << "5";
    InstallTask* install = new InstallTask(0,0,"install");
    box->addTask(install);


    // first quering 2 id sequences
    paludis::tr1::shared_ptr<const paludis::PackageIDSequence> packageIDs(
            env->package_database()->query(
            paludis::query::Matches(paludis::PackageDepSpec("sys-apps/paludis", paludis::pds_pm_eapi_0_strict)) &
            paludis::query::InstalledAtRoot(env->root()),
                                            paludis::qo_order_by_version));

    qDebug() << "3";
    if (packageIDs->empty() )
        QFAIL("no packages found");
    qDebug() << "4";
    paludis::tr1::shared_ptr<const paludis::PackageIDSequence> ids_2(
            env->package_database()->query(
            paludis::query::Matches(paludis::PackageDepSpec("app-shells/bash", paludis::pds_pm_eapi_0_strict)) &
            paludis::query::InstalledAtRoot(env->root()),
                                            paludis::qo_order_by_version));

    qDebug() << "5";
    if (ids_2->empty() )
        QFAIL("no packages found");

    install->addEntry(*packageIDs->last());
    install->addEntry(*ids_2->begin());

    QVariantList list_1 = box->selectionData(*packageIDs->last());
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
    InstallTask* install = new InstallTask(0,0,"install");
    box->addTask(install);

    // init
    paludis::tr1::shared_ptr<const paludis::PackageIDSequence> packageIDs(
            env->package_database()->query(
                                paludis::query::Matches(paludis::PackageDepSpec("sys-apps/paludis", paludis::pds_pm_eapi_0_strict)),
                                paludis::qo_order_by_version));

    if (packageIDs->empty() )
        QFAIL("no packages found");

    install->addEntry(*packageIDs->last());
    Package* pitem = makePackagePackage(*packageIDs->last(),
                                box->tasks(),
                                QString::fromStdString(paludis::stringify( (*packageIDs->last())->name().package)),
                                QString::fromStdString(paludis::stringify( (*packageIDs->last())->name().category)),
                                Qt::Unchecked,
                                Package::ps_stable,
                                0,
                                "");
    for (paludis::PackageIDSequence::ConstIterator vstart(packageIDs->begin()),vend(packageIDs->end());
         vstart != vend;
         ++vstart)
    {
        Package* vitem = makeVersionPackage(*vstart,
                                    box->tasks(),
                                    QString::fromStdString(paludis::stringify( (*vstart)->version())),
                                    QString::fromStdString(paludis::stringify( (*vstart)->repository()->name())),
                                    Qt::Unchecked,
                                    Package::ps_stable,
                                    0,
                                    "");
        pitem->appendChild(vitem);
        pitem->setBestChild(vitem);
        box->setTasksInPackage(vitem);
    }

    qDebug() << "pitem" << *pitem;
    qDebug() << "pitem->bestChild()" << *pitem->bestChild();
    QCOMPARE(install->hasEntry(pitem->ID()),true);
    QCOMPARE(pitem->data(Package::po_selected).toList(),QVariantList() << QVariant(Qt::PartiallyChecked) );
    QCOMPARE(pitem->bestChild()->data(Package::po_selected).toList(),QVariantList() << QVariant(Qt::Checked) );

    install->deleteEntry(pitem->ID());
    QCOMPARE(install->hasEntry(pitem->ID()),false );
    box->setTasksInPackage(pitem);
    QCOMPARE(pitem->bestChild()->data(Package::po_selected).toList(),QVariantList() << QVariant(Qt::Unchecked));
    QCOMPARE(pitem->data(Package::po_selected).toList(),QVariantList() << QVariant(Qt::Unchecked) );
    delete box;
    delete install;
    delete pitem;
}

QTEST_MAIN(pertubis::TaskBoxTest)

