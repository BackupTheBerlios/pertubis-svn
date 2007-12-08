
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

#include "ItemTest.hh"
#include "../src/Package.hh"

void pertubis::ItemTest::setTaskState()
{
    QVariantList selections;
    selections << Qt::Unchecked << Qt::Unchecked;
    QVariantList data;
    data << QVariant(selections) << "" << "" << "" << Qt::Unchecked << "" << "";
    Package* pitem = new Package(paludis::tr1::shared_ptr<const paludis::PackageID>(),
                          data,
                          Package::ps_stable,
                          Package::pt_parent,
                          0);
    Package* vitem = new Package(paludis::tr1::shared_ptr<const paludis::PackageID>(),
                           data,
                           Package::ps_stable,
                           Package::pt_child,
                           pitem);
    pitem->setTaskState(0,Qt::Checked);
    vitem->setTaskState(1,Qt::Checked);
    int s( static_cast<int>(pitem->data(Package::po_selected).toList().value(0).toInt()));
    QCOMPARE(s,static_cast<int>(Qt::Checked));
    s =  static_cast<int>(vitem->data(Package::po_selected).toList().value(1).toInt());
    QCOMPARE(s,static_cast<int>(Qt::Checked));
    pitem->setTaskState(0,Qt::Unchecked);
    s = pitem->data(Package::po_selected).toList().value(0).toInt();
    QCOMPARE(s,static_cast<int>(Qt::Unchecked));
    vitem->setTaskState(1,Qt::Unchecked);
    s = static_cast<int>(pitem->data(Package::po_selected).toList().value(1).toInt());
    QCOMPARE(s,static_cast<int>(Qt::Unchecked));
    delete pitem;
}

void pertubis::ItemTest::setState()
{
    Package* pitem = new Package(paludis::tr1::shared_ptr<const paludis::PackageID>(),
                           QVariantList(),
                           Package::ps_stable,
                           Package::pt_parent,
                           0);
    pitem->setPackageState(Package::ps_unstable);
    QCOMPARE(pitem->state(),Package::ps_unstable);
    pitem->setPackageState(Package::ps_masked);
    QCOMPARE(pitem->state(),Package::ps_masked);
    delete pitem;
}

void pertubis::ItemTest::available()
{
    Package* pitem = new Package(paludis::tr1::shared_ptr<const paludis::PackageID>(),
                           QVariantList(),
                            Package::ps_unstable,
                            Package::pt_parent,
                            0);
    QCOMPARE(pitem->available(),true);
    pitem->setPackageState(Package::ps_stable);
    QCOMPARE(pitem->available(),true);
    pitem->setPackageState(Package::ps_masked);
    QCOMPARE(pitem->available(),false);
    delete pitem;
}

void pertubis::ItemTest::appendChild()
{
    Package* pitem = new Package();
    Package* vitem1 = new Package();
    Package* vitem2 = new Package();
    QCOMPARE(pitem->childCount(),0);
    QCOMPARE(vitem1->childCount(),0);
    QCOMPARE(vitem2->childCount(),0);
    pitem->appendChild(vitem1);
    pitem->appendChild(vitem2);
    QCOMPARE(pitem->childCount(),2);
    QCOMPARE(vitem1->parent(),pitem);
    QCOMPARE(vitem2->parent(),pitem);
    QCOMPARE(pitem->child(0),vitem1);
    QCOMPARE(pitem->child(1),vitem2);
    delete pitem;
}

void pertubis::ItemTest::prependChild()
{
    Package* pitem = new Package();
    Package* vitem1 = new Package();
    Package* vitem2 = new Package();
    QCOMPARE(pitem->childCount(),0);
    QCOMPARE(vitem1->childCount(),0);
    QCOMPARE(vitem2->childCount(),0);
    pitem->prependChild(vitem1);
    pitem->prependChild(vitem2);
    QCOMPARE(pitem->childCount(),2);
    QCOMPARE(vitem1->parent(),pitem);
    QCOMPARE(vitem2->parent(),pitem);
    QCOMPARE(pitem->child(0),vitem2);
    QCOMPARE(pitem->child(1),vitem1);
    delete pitem;
}

void pertubis::ItemTest::setBestChild()
{
    Package* pitem = new Package();
    Package* vitem1 = new Package();
    Package* vitem2 = new Package();
    pitem->appendChild(vitem1);
    pitem->appendChild(vitem2);
    pitem->setBestChild(vitem2);
    QCOMPARE(pitem->bestChild(),vitem2);
    pitem->setBestChild(vitem1);
    QCOMPARE(pitem->bestChild(),vitem1);
    delete pitem;
}

void pertubis::ItemTest::setID()
{
    Package* pitem = new Package();
    paludis::tr1::shared_ptr<const paludis::PackageID> id;
    pitem->setID(id);
    QCOMPARE(pitem->ID(),id);
    delete pitem;
}

QTEST_MAIN(pertubis::ItemTest)
