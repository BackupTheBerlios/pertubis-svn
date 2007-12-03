
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
#include "Item.hh"
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
    if (!mix.isValid())
        return;
    painter->save();

    QModelIndex index(m_proxy->mapToSource(mix));
    Item* item = static_cast<Item*>(index.internalPointer());
    if (!item)
        return;
    QVariantList selections = item->data(Item::io_selected).toList();
    QBrush in(QColor(0,255,0));
    QBrush out(QColor(255,0,0));

    QPen pen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    if (item->state() != Item::is_masked)
    {
        painter->drawRect(option.rect.x(),option.rect.y(),16,16);
        painter->drawRect(option.rect.x()+18,option.rect.y(),16,16);
        switch (selections.value(0).toInt() )
        {
            case Qt::PartiallyChecked:
            case Qt::Checked:
                painter->fillRect(option.rect.x(),option.rect.y(),16,16,in);
                break;
            default:                ;
        }
        switch (selections.value(1).toInt() )
        {
            case Qt::PartiallyChecked:
            case Qt::Checked:
                painter->fillRect(option.rect.x()+18,option.rect.y(),16,16,out);
                break;
            default:
                ;
        }
    }

    painter->restore();
}

QSize pertubis::OptionsDelegate::sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    QRect decorationRect = rect(option, index, Qt::DecorationRole);
    QRect displayRect;
//     Item* item = static_cast<Item*>(index.internalPointer());
    if (index.column() == Item::io_selected)
        displayRect = QRect(QPoint(0,0),QPoint(32,16));
    else
        displayRect = rect(option, index, Qt::DisplayRole);
    QRect checkRect = rect(option, index, Qt::CheckStateRole);

    doLayout(option, &checkRect, &decorationRect, &displayRect, true);
    return (decorationRect|displayRect|checkRect).size();
}
