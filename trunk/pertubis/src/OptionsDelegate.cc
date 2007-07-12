
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

#include "OptionsDelegate.hh"
#include "PackageItem.hh"
#include <QColor>
#include <QMenu>
#include <QModelIndex>
#include <QPainter>
#include <QPalette>
#include <QPen>

pertubis::OptionsDelegate::OptionsDelegate(QWidget *parent) : QItemDelegate(parent)
{
}

void pertubis::OptionsDelegate::OptionsDelegate::paint(QPainter* painter,
										const QStyleOptionViewItem& option,
		  								const QModelIndex& index) const
{
	if (!index.isValid())
		return;
	painter->save();

	Item* item = static_cast<Item*>(index.internalPointer());
	if (!item)
		return;
	QVariantMap map = item->data(Item::io_selected).toMap();
	QPixmap ins(":button_ok_16.xpm");
	QPixmap dei(":button_cancel_16.xpm");

	QPen pen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	painter->setPen(pen);
	if (item->status() != Item::is_masked)
	{
		painter->drawRect(option.rect.x(),option.rect.y()+3,13,13);
		painter->drawRect(option.rect.x()+16,option.rect.y()+3,13,13);
		if (map.contains("install") && map.value("install").toBool())
			painter->drawPixmap(option.rect.x(),option.rect.y()+3, ins);
		if (map.contains("deinstall") && map.value("deinstall").toBool())
			painter->drawPixmap(option.rect.x()+16,option.rect.y()+3, dei);
	}

	painter->restore();
}

QSize pertubis::OptionsDelegate::sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    QRect decorationRect = rect(option, index, Qt::DecorationRole);
	QRect displayRect;
	Item* item = static_cast<Item*>(index.internalPointer());
	if (index.column() == Item::io_selected && item->type() == Item::it_version)
		displayRect = QRect(QPoint(0,0),QPoint(32,16));
	else
    	displayRect = rect(option, index, Qt::DisplayRole);
    QRect checkRect = rect(option, index, Qt::CheckStateRole);

    doLayout(option, &checkRect, &decorationRect, &displayRect, true);
    return (decorationRect|displayRect|checkRect).size();
}
