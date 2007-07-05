
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

#include "PackageModel.hh"
#include "PackageItem.hh"

#include "Workspace.hh"

// #include <QDebug>
#include <QBrush>
#include <QApplication>
#include <QColor>
#include "TaskBox.hh"

#include <QStringList>

bool ItemLessThan(const pertubis::Item* a,const pertubis::Item* b)
{
	return (a->m_data.begin()->toString() < b->m_data.begin()->toString() );
}

pertubis::PackageModel::PackageModel(QObject* parent) : QAbstractItemModel(parent),m_root(0)
{
	m_root = new Item(QList<QVariant>() << "root",0,Item::it_category);
}

pertubis::PackageModel::~PackageModel()
{
}

void pertubis::PackageModel::setBox(TaskBox* t)
{
	m_box = t;
}

bool pertubis::PackageModel::setSelectionData( const QModelIndex & ix, const QString & task, bool state)
{
	Item* item = static_cast<Item*>(ix.internalPointer());
	if (item->m_rtti == Item::it_package)
	{
		item->setPackageSelection(task,state);
		{
			emit dataChanged(ix, index(item->childCount()-1,0,ix));
			return true;
		}
	}

	if (item->m_rtti == Item::it_version)
	{
		if (item->setVersionSelection(task,state))
		{
			QModelIndex p = parent(ix);
			QModelIndex last = index(rowCount(p)-1,0,p);
			emit dataChanged(p,last);
			return true;
		}
	}
	return false;
}

bool pertubis::PackageModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role)
{
	if (orientation == Qt::Horizontal && section < m_header.count() && role == Qt::EditRole)
	{
		m_header.replace(section,value.toString());
		emit headerDataChanged(orientation,section,section);
		return true;
	}
	return false;
}

Qt::ItemFlags pertubis::PackageModel::flags(const QModelIndex &index) const
{
	switch (index.column())
	{
		case ph_selected:
			return Qt::ItemIsEditable;
			break;
		case ph_installed:
			return Qt::ItemIsSelectable;
			break;
		default:
			return 0;
	}
}

QVariant pertubis::PackageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation == Qt::Vertical || section >= m_header.count() )
		return QVariant();
	return *(m_header.begin() + section);
}

QVariant pertubis::PackageModel::data ( const QModelIndex & index, int role) const
{
	if (!index.isValid() || index.column() >= m_header.count())
		return QVariant();

	Item* item = static_cast<Item*>(index.internalPointer());

	if (role == Qt::ForegroundRole)
	{
		if (index.column() == ph_installed)
			return QBrush(QColor(0,0,255));
		if (index.column() == ph_selected)
			return QBrush(QColor(0,255,0));
		if (index.column() == ph_package)
		{
			if (item->m_status == ps_stable)
				return QBrush(QColor(0,255,0));
			if (item->m_status == ps_unstable)
				return QBrush(QColor(255,200,0));
			if (item->m_status == ps_masked)
				return QBrush(QColor(255,0,0));
		}
	}

	if (role == Qt::CheckStateRole )
	{
		switch (index.column())
		{
			case ph_installed:
				return item->data(index.column());
			default:
				return QVariant();
		}
	}

	if (role == Qt::DisplayRole )
	{
		switch (index.column())
		{
			case ph_installed:
				return QVariant();
			default:
				return item->data(index.column());
		}
	}

	return QVariant();
}

QModelIndex pertubis::PackageModel::index(int row, int column, const QModelIndex &parent) const
{
 	if (!hasIndex(row, column, parent))
		return QModelIndex();

	Item *parentItem;

	if (!parent.isValid())
		parentItem = m_root;
	else
		parentItem = static_cast<Item*>(parent.internalPointer());

	Item *childItem = parentItem->child(row);
	if (childItem)
		return QAbstractItemModel::createIndex(row, column, childItem);
	else
		return QModelIndex();
}

void pertubis::PackageModel::setHorizontalHeaderLabels ( const QStringList & labels )
{
	m_header = labels;
}

QModelIndex pertubis::PackageModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	Item *childItem = static_cast<Item*>(index.internalPointer());
	Item *parentItem = childItem->parent();

	if (parentItem == m_root)
		return QModelIndex();

	return QAbstractItemModel::createIndex(parentItem->row(), 0, parentItem);
}

int pertubis::PackageModel::rowCount(const QModelIndex &parent) const
{
	Item *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = m_root;
	else
		parentItem = static_cast<Item*>(parent.internalPointer());

	return parentItem->childCount();
}

int pertubis::PackageModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
         return static_cast<Item*>(parent.internalPointer())->columnCount();
	else if (m_root != 0)
		return m_root->columnCount();
	else
		return 0;
}

void pertubis::PackageModel::slotSetRoot(Item* item)
{
	if (m_root != 0)
		delete m_root;
	m_root = item;
	joinData();
	reset();
}

void pertubis::PackageModel::joinData()
{
	Item* item;
	foreach (item,m_root->m_children)
	{
		Item* version;
		QVariantMap data;
		int status = ps_masked;
		foreach (version,item->m_children)
		{
			QString cat,pack,ver,rep;
// 			qDebug() << pack;
			version->entryData(cat,pack,ver,rep);
			Entry entry = genPackEntry(cat,pack,ver,rep);
			QVariantMap optionsData = m_box->selectionData(entry);
			version->setData(ph_selected,optionsData);
			if (item->m_status < status)
				status = item->m_status;
			if (optionsData[tr("install")].toBool())
				data.insert(tr("install"),true);
			if (optionsData[tr("deinstall")].toBool())
				data.insert(tr("deinstall"),true);
		}

		item->setData(ph_selected,data);
		item->m_status = (Status) status;
	}
}

void pertubis::PackageModel::slotAppendPackage(Item* item)
{
	if (m_root != 0)
	{
		m_root->appendChild(item);
		item->m_parent = m_root;
		reset();
	}
}

