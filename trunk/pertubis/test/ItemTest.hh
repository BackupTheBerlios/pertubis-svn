
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


#ifndef _PERTUBIS_ENTRY_PROTECTOR_ITEM_TEST_H
#define _PERTUBIS_ENTRY_PROTECTOR_ITEM_TEST_H 1

#include <QtTest/QtTest>
#include <QtCore>
#include <QtGui>

namespace pertubis
{
	class TaskBox;

    /*! \brief Test cases for Item classes
     *
     */
	class ItemTest : public QObject
	{
		Q_OBJECT
	public:

		ItemTest() {}
		~ItemTest() {}

	private slots:

        /*! \brief Combined simple hierachy test with PackageItem and VersionItem as aprent and child
        *
        */
		void appendChild();
        void setTaskState();
	};
}

#endif
