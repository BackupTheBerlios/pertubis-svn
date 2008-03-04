
/* Copyright (C) 2007 Stefan Koegl
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

#include "SelectionModelDelegate.hh"
#include "Package.hh"

#include <QColor>
#include <QBrush>
#include <QModelIndex>
#include <QPainter>
#include <QPen>
#include <QSortFilterProxyModel>

using namespace perturbis;

SelectionModelDelegate::SelectionModelDelegate(QWidget *pobj,
    int install,
    int deinstall,
    int installed) :
    QItemDelegate(pobj),
    m_installColumn(install),
    m_deinstallColumn(deinstall),
    m_installedColumn(installed)
{
}

void
SelectionModelDelegate::SelectionModelDelegate::paint(QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& mix) const
{
    // custom paint if this is a selection column, otherwise use the std layout
    int column(mix.column());
    if (m_installColumn != column &&
        m_deinstallColumn != column &&
        m_installedColumn != column)
    {
        QItemDelegate::paint(painter,option,mix);
        return;
    }

    Package* item = static_cast<Package*>(mix.internalPointer());
    Q_ASSERT(item != 0);
    QPen pen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    int ypos(option.rect.y()+ (option.rect.height()-12) /2);
    painter->save();
    painter->setPen(pen);

    if (m_installColumn == column &&
        item->state() != ps_masked &&
        Qt::Checked == item->data(m_installColumn).toInt())
    {
        painter->drawRect(option.rect.x(),ypos,12,12);
        painter->fillRect(option.rect.x()+2,ypos+2,9,9,QBrush(QColor(0,255,0)));
    }
    else if (m_deinstallColumn == column &&
        Qt::Checked == item->data(m_installedColumn).toInt() &&
        Qt::Checked == item->data(m_deinstallColumn).toInt())
    {
        painter->drawRect(option.rect.x(),ypos,12,12);
        painter->fillRect(option.rect.x()+2,ypos+2,9,9,QBrush(QColor(255,0,0)));
    }
    else if (m_installedColumn == column &&
        Qt::Checked == item->data(m_installedColumn).toInt())
    {
        painter->drawRect(option.rect.x(),ypos,12,12);
        painter->fillRect(option.rect.x()+2,ypos+2,9,9,QBrush(QColor(0,0,255)));
    }
    painter->restore();
}
