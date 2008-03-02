
/* Copyright (C) 2007-2008 Stefan Koegl
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

#include "DeinstallSelections.hh"
#include "Package.hh"

#include <paludis/package_id.hh>
#include <paludis/name.hh>

#include <QDebug>

using namespace pertubis;

bool
DeinstallSelections::available(Package* item, int column) const
{
    // TODO what about the other packages' header order?
    bool res(item->available() && item->data(column).toInt() == Qt::Checked);
    qDebug() << "DeinstallSelections::available()" << res;
    return res;
}

bool
DeinstallSelections::changeStates(Package* item, int newState, int position)
{
    qDebug() << "DeinstallSelections::changeStates()" << *item << newState;
    Package::PackageIterator iStart;
    Package::PackageIterator iEnd;
    Package::PackageIterator piStart;
    Package::PackageIterator piEnd;
    int i=0;
    switch (item->itemType())
    {
        case pt_parent:
            qDebug() << "DeinstallSelections::changeStates() 1 parent";
            iStart = item->childBegin();
            iEnd = item->childEnd();
            switch (newState)
            {
                case Qt::Unchecked:
                    qDebug() << "DeinstallSelections::changeStates() 1 parent unchecked";
                    item->setData(position,Qt::Unchecked);
                    while(iStart != iEnd)
                    {
                        changeEntry((*iStart)->ID(),false);
                        (*iStart)->setData(position,Qt::Unchecked);
                        ++iStart;
                    }
                    break;
                case Qt::PartiallyChecked:
                case Qt::Checked:
                    qDebug() << "DeinstallSelections::changeStates() 1 parent checked";
                    changeEntry(item->ID(),true);
                    item->setData(position,Qt::Checked);
                    if (item->bestChild() != 0)
                        item->bestChild()->setData(position,Qt::Checked);
                    break;
                default:
                    break;
            }
            break;
        case pt_child:
            qDebug() << "DeinstallSelections::changeStates() 1 child";
            if (item->data(pho_installed).toInt() == Qt::Unchecked)
                return false;
            piStart = item->parent()->childBegin();
            piEnd = item->parent()->childEnd();
            switch (newState)
            {
                case Qt::Unchecked:
                    changeEntry(item->ID(),false);
                    item->setData(position,Qt::Unchecked);
                    while(piStart != piEnd)
                    {
                        if (Qt::Unchecked != (*piStart)->data(pho_deinstall).toInt())
                        {
                            ++i;
                            break;
                        }
                        ++piStart;
                    }
                    if (i  == 0)
                        item->parent()->setData(position,Qt::Unchecked);
                    else
                        item->parent()->setData(position,Qt::PartiallyChecked);
                    break;
                case Qt::PartiallyChecked:
                case Qt::Checked:
                    changeEntry(item->ID(),true);
                    item->setData(position,Qt::Checked);
                    while(piStart != piEnd)
                    {
                        if (Qt::Unchecked != (*piStart)->data(pho_deinstall).toInt())
                            ++i;
                        ++piStart;
                    }
                    if (i > 0)
                        item->parent()->setData(position,Qt::Checked);
                    break;
                default:
                    break;
            }
            break;
        case pt_node_only:
            qDebug() << "DeinstallSelections::changeStates() 1 node";
            switch (newState)
            {
                case Qt::Unchecked:
                    changeEntry(item->ID(),false);
                    item->setData(position,Qt::Unchecked);
                    break;
                case Qt::Checked:
                    changeEntry(item->ID(),true);
                    item->setData(position,Qt::Checked);
                    break;
                default:
                    break;
            }
        default:
            break;
    }
    return true;
}
