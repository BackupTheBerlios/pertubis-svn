
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


#include "OptionsDelegate.hh"
#include "PackageFilterModel.hh"
#include "Package.hh"
#include <QColor>
#include <QBrush>
#include <QMenu>
#include <QModelIndex>
#include <QPainter>
#include <QPen>

pertubis::OptionsDelegate::OptionsDelegate(QWidget *pobj,PackageFilterModel* proxy) : QItemDelegate(pobj),m_proxy(proxy)
{
}

void pertubis::OptionsDelegate::OptionsDelegate::paint(QPainter* painter,
                                        const QStyleOptionViewItem& option,
                                        const QModelIndex& mix) const
{
    // custom paint if this is a selection column, otherwise use the std layout
    // if this is a selection column but the Package is masked, don't paint anything
    int column(mix.column());
    if (po_install != column && po_deinstall != column)
    {
        QItemDelegate::paint(painter,option,mix);
        return;
    }

    QModelIndex index;
    if (m_proxy != 0)
        index = m_proxy->mapToSource(mix);
    else
        index = mix;

    Package* item = static_cast<Package*>(index.internalPointer());
    Q_ASSERT(item != 0);

    if (item->state() != ps_masked)
    {
        painter->save();
        QPen pen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter->setPen(pen);
        int ypos( option.rect.y()+ (option.rect.height()-12) /2 );
        painter->drawRect(option.rect.x(),ypos,12,12);
        if (po_install == column && Qt::Checked == item->data(po_install).toInt() )
        {
            painter->fillRect(option.rect.x()+2,ypos+2,9,9,QBrush(QColor(0,255,0)));
        }
        else if (po_deinstall == column && Qt::Checked == item->data(po_deinstall).toInt() )
        {
            painter->fillRect(option.rect.x()+2,ypos+2,9,9,QBrush(QColor(255,0,0)));
        }
        painter->restore();
    }
}

// QSize pertubis::OptionsDelegate::sizeHint(const QStyleOptionViewItem &option,
//                               const QModelIndex &index) const
// {
//     QRect decorationRect = rect(option, index, Qt::DecorationRole);
//     QRect displayRect;
// //     Package* item = static_cast<Package*>(index.internalPointer());
//     if (index.column() == po_install || index.column() == po_deinstall)
//         displayRect = QRect(QPoint(0,0),QPoint(16,16));
//     else
//         displayRect = rect(option, index, Qt::DisplayRole);
//     QRect checkRect = rect(option, index, Qt::CheckStateRole);
//
//     doLayout(option, &checkRect, &decorationRect, &displayRect, true);
//     return (decorationRect|displayRect|checkRect).size();
// }
