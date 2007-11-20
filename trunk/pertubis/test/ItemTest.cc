
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

void pertubis::ItemTest::appendChild()
{
    Item* pitem = new Item();
    Item* vitem = new Item();
    QCOMPARE(pitem->childCount(),0);
    QCOMPARE(vitem->childCount(),0);
    pitem->appendChild(vitem);
    QCOMPARE(pitem->childCount(),1);
    QCOMPARE(vitem->parent(),pitem);
    delete pitem;
}

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
                           Item::ur_parent,
                           pitem);
    pitem->setTaskState(0,Qt::Checked);
    vitem->setTaskState(1,Qt::Checked);
    int s( static_cast<int>(pitem->data(Item::io_selected).toList().value(0).toInt()));
    QCOMPARE(s,static_cast<int>(Qt::Checked));
    s =  static_cast<int>(vitem->data(Item::io_selected).toList().value(1).toInt());
    QCOMPARE(s,static_cast<int>(Qt::Checked));
    pitem->setTaskState(0,Qt::Unchecked);
    s = static_cast<int>(pitem->data(Item::io_selected).toList().value(0).toInt());
    QCOMPARE(s,static_cast<int>(Qt::Unchecked));
    vitem->setTaskState(1,Qt::Unchecked);
    s = static_cast<int>(pitem->data(Item::io_selected).toList().value(1).toInt());
    QCOMPARE(s,static_cast<int>(Qt::Unchecked));
}

QTEST_MAIN(pertubis::ItemTest)
