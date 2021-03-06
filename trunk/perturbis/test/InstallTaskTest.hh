
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of perturbis
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_INSTALLTASK_TEST_H
#define _PERTUBIS_ENTRY_PROTECTOR_INSTALLTASK_TEST_H 1

#include <QtTest/QtTest>
#include <QtCore>
#include <QtGui>

namespace perturbis
{
    class TaskBox;

    class InstallTaskTest : public QObject
    {
        Q_OBJECT
        public:

        InstallTaskTest() {}
        ~InstallTaskTest() {}

    private slots:
        void getSetTaskid();
        void addEntry();
//         void deleteEntry(Item* item);
//         void setTaskid(int id);
//         void hasEntry(Item* item);
//         void name() const { return m_name;}
//         void processTaskState(Item* item,bool mystate);
    };
}

#endif
