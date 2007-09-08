
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
#include "../src/TaskBox.hh"

#include "../src/PackageItem.hh"
#include "../src/VersionItem.hh"

void pertubis::ItemTest::appendChild()
{
    Item* pitem = new PackageItem();
    Item* vitem = new VersionItem();
    QCOMPARE(pitem->childCount(),0);
    QCOMPARE(vitem->childCount(),0);
    pitem->appendChild(vitem);
    QCOMPARE(pitem->childCount(),1);
    QCOMPARE(vitem->parent(),pitem);
    delete pitem;
}

QTEST_MAIN(pertubis::ItemTest)
