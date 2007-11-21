
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

#include "ItemTest.hh"
#include "../src/Item.hh"

void pertubis::ItemTest::setTaskState()
{
    QVariantList selections;
    selections << Qt::Unchecked << Qt::Unchecked;
    QVariantList data;
    data << QVariant(selections) << "" << "" << "" << Qt::Unchecked << "" << "";
    Item* pitem = new Item(paludis::tr1::shared_ptr<const paludis::PackageID>(),
                          data,
                          Item::is_stable,
                          Item::ur_parent,
                          0);
    Item* vitem = new Item(paludis::tr1::shared_ptr<const paludis::PackageID>(),
                           data,
                           Item::is_stable,
                           Item::ur_child,
                           pitem);
    pitem->setTaskState(0,Qt::Checked);
    vitem->setTaskState(1,Qt::Checked);
    int s( static_cast<int>(pitem->data(Item::io_selected).toList().value(0).toInt()));
    QCOMPARE(s,static_cast<int>(Qt::Checked));
    s =  static_cast<int>(vitem->data(Item::io_selected).toList().value(1).toInt());
    QCOMPARE(s,static_cast<int>(Qt::Checked));
    pitem->setTaskState(0,Qt::Unchecked);
    s = pitem->data(Item::io_selected).toList().value(0).toInt();
    QCOMPARE(s,static_cast<int>(Qt::Unchecked));
    vitem->setTaskState(1,Qt::Unchecked);
    s = static_cast<int>(pitem->data(Item::io_selected).toList().value(1).toInt());
    QCOMPARE(s,static_cast<int>(Qt::Unchecked));
    delete pitem;
}

void pertubis::ItemTest::setState()
{
    Item* pitem = new Item(paludis::tr1::shared_ptr<const paludis::PackageID>(),
                           QVariantList(),
                           Item::is_stable,
                           Item::ur_parent,
                           0);
    pitem->setState(Item::is_unstable);
    QCOMPARE(pitem->state(),Item::is_unstable);
    pitem->setState(Item::is_masked);
    QCOMPARE(pitem->state(),Item::is_masked);
    delete pitem;
}

void pertubis::ItemTest::available()
{
    Item* pitem = new Item(paludis::tr1::shared_ptr<const paludis::PackageID>(),
                           QVariantList(),
                            Item::is_unstable,
                            Item::ur_parent,
                            0);
    QCOMPARE(pitem->available(),true);
    pitem->setState(Item::is_stable);
    QCOMPARE(pitem->available(),true);
    pitem->setState(Item::is_masked);
    QCOMPARE(pitem->available(),false);
    delete pitem;
}

void pertubis::ItemTest::appendChild()
{
    Item* pitem = new Item();
    Item* vitem1 = new Item();
    Item* vitem2 = new Item();
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
    Item* pitem = new Item();
    Item* vitem1 = new Item();
    Item* vitem2 = new Item();
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
    Item* pitem = new Item();
    Item* vitem1 = new Item();
    Item* vitem2 = new Item();
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
    Item* pitem = new Item();
    paludis::tr1::shared_ptr<const paludis::PackageID> id;
    pitem->setID(id);
    QCOMPARE(pitem->ID(),id);
    delete pitem;
}

QTEST_MAIN(pertubis::ItemTest)
