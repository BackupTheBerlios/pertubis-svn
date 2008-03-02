
/* Copyright (C) 2007 Stefan Koegl
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

#include "ReportModelDelegate.hh"
#include "ReportPackage.hh"
#include "Package.hh"

#include <QColor>
#include <QBrush>
#include <QModelIndex>
#include <QPainter>
#include <QPen>
#include <QSortFilterProxyModel>

using namespace pertubis;

ReportModelDelegate::ReportModelDelegate(QWidget *pobj) :
    QItemDelegate(pobj)
{
}

void
ReportModelDelegate::ReportModelDelegate::paint(QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& mix) const
{
    Package* item = static_cast<Package*>(mix.internalPointer());
    Q_ASSERT(item != 0);
    int ypos(option.rect.y() + (option.rect.height()-12) /2);
    painter->save();
    painter->setPen(QPen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if (Qt::Checked == item->data(rpho_deinstall).toInt())
    {
        painter->drawRect(option.rect.x(),ypos,12,12);
        painter->fillRect(option.rect.x()+2,ypos+2,9,9,QBrush(QColor(255,0,0)));
    }
    painter->restore();
}
