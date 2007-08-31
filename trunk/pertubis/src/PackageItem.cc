
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

#include "PackageItem.hh"


#include <QStringList>
#include <QDebug>

QString pertubis::Item::status(Item::ItemStatus status)
{
	switch (status)
	{
		case Item::is_stable:
			return "stable";
		case Item::is_unstable:
			return "possibly unstable";
		case Item::is_masked:
			return "not avaliable since masked";
		default:
			return "unknown package status";
	}
}

pertubis::Item::Item(const QList<QVariant> &dats, Item *pitem,ItemType t) : m_data(dats),m_parent(pitem),m_status(is_stable),m_rtti(t)
{
}

pertubis::Item::Item(const QList<QVariant> &dats, Item *pitem,ItemType t, const paludis::tr1::shared_ptr<const paludis::PackageID>& id) : m_data(dats),m_parent(pitem),m_status(is_stable),m_rtti(t)
{
}

pertubis::Item::~Item()
{
	m_parent=0;
	qDeleteAll(m_children);
	m_children.clear();
}

void pertubis::Item::appendChild(Item *item)
{
	m_children.append(item);
}

pertubis::Item* pertubis::Item::child(int rindex) const
{
	return m_children.value(rindex);
}

void pertubis::Item::setData(int column, QVariant dats)
{
	if (column < m_data.count() )
	{
		m_data.replace(column, dats);
	}
}

void pertubis::Item::setSelectionData(QString task,bool state)
{
	QVariantMap m = m_data.value(io_selected).toMap();
	m[task]=state;
	setData(io_selected,m);
	emit taskChanged(this,task,state);
}

bool pertubis::Item::setPackageSelection(QString task,bool state)
{
	if (m_status ==is_masked)
		return false;
	if (state)
	{
		if (task == tr("install"))
		{
			Item* bestVersion = bestChild();
			if (bestVersion != 0)
			{
				bestVersion->setSelectionData(task,true);
				setSelectionData(task,true);
				return true;
			}
		}
		if (task == tr("deinstall"))
		{
			Item* item;
			bool inUse=false;
			foreach (item,m_children)
			{
				if (item->data(io_installed).toBool())
				{
					item->setSelectionData(task,true);
					inUse=true;
				}
			}
			if (inUse)
			{
				setSelectionData(task,true);
				return true;
			}
		}
	}
	else
	{
		Item* item;
		setSelectionData(task,state);
		foreach (item,m_children)
		{
			item->setSelectionData(task,state);
		}
		return true;
	}
	return false;
}

bool pertubis::Item::entryData(QString& cat,QString& pack,QString& ver,QString& rep) const
{
	if (m_rtti == it_version)
	{
		cat = m_parent->data(io_category).toString();
		pack = m_parent->data(io_package).toString();
		ver = data(io_package).toString();
		rep = m_parent->data(io_repository).toString();
		return true;
	}
	else if (m_rtti == it_package)
	{
		Item* version = bestChild();
		cat = data(io_category).toString();
		pack = data(io_package).toString();
		ver = version->data(io_package).toString();
		rep = data(io_repository).toString();
		return true;
	}
	return false;
}

bool pertubis::Item::setVersionSelection(QString task,bool state)
{
	if (m_status ==is_masked)
		return false;
	if (state)
	{
		if (task == tr("install") )
		{
			if (m_status != is_masked)
			{
				setSelectionData(task,state);
				m_parent->setSelectionData(task,true);
				return true;
			}
		}
		else if (task == tr("deinstall"))
		{
			if (data(io_installed).toBool())
			{
				setSelectionData(task,true);
				m_parent->setSelectionData(task,true);
				return true;
			}
		}
	}
	else
	{
		Item* item;
		bool inUse=false;
		setSelectionData(task,state);
		foreach (item,m_parent->m_children)
		{
			QVariantMap idata= item->data(io_selected).toMap();
			QVariantMap::const_iterator it = idata.find(task);
			if (it.value().toBool())
				inUse=true;
		}
		if (!inUse)
		{
			m_parent->setSelectionData(task,state);
		}
		return true;
	}
	return false;
}

pertubis::Item* pertubis::Item::bestChild() const
{
	if (m_rtti == it_package)
	{
		QListIterator<Item*> item(m_children);
		item.toBack();
		while (item.hasPrevious() )
		{
			Item* tmp = item.previous();
			if ( tmp->m_status != is_masked)
				return tmp;
		}
	}
	return 0;
}

int pertubis::Item::childCount() const
{
	return m_children.count();
}

int pertubis::Item::columnCount() const
{
	return m_data.count();
}

QVariant pertubis::Item::data(int column) const
{
	return m_data.value(column);
}

pertubis::Item* pertubis::Item::parent()
{
	return m_parent;
}

int pertubis::Item::row() const
{
	if (m_parent)
		return m_parent->m_children.indexOf(const_cast<Item*>(this));
	return 0;
}
