
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

#include "TaskBox.hh"
#include "PackageItem.hh"

#include <QMenu>
#include <QVariant>
#include <QMap>
#include <QDebug>

#include <paludis/util/stringify.hh>

pertubis::DBTask::DBTask(QObject* pobj) : QObject(pobj)
{
}

void pertubis::DBTask::addEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id)
{
	m_data.insert(id);
}

// bool pertubis::DBTask::hasEntry(QString pack,QString cat,QString ver,QString rep)
// {
// 	pertubis::Entry p = genPackEntry(pack,cat,ver,rep);
// 	return hasEntry(p);
// }

bool pertubis::DBTask::hasEntry(paludis::tr1::shared_ptr<const paludis::PackageID> id)
{
	return (m_data.find(id) != m_data.end());
}

void pertubis::DBTask::changeEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id,bool state)
{
	std::set<paludis::tr1::shared_ptr<const paludis::PackageID> >::const_iterator it = m_data.find(id);
	if (state)
		addEntry(id);
	else
		m_data.erase(id);
}

QVariantMap pertubis::TaskBox::selectionData(paludis::tr1::shared_ptr<const paludis::PackageID> sid)
{
	QVariantMap map;
	std::map<std::string,DBTask*>::const_iterator mytask = m_tasks.begin();
	while (mytask != m_tasks.end())
	{
		bool res = mytask->second->hasEntry(sid);
		map.insert(QString(mytask->first.c_str()),res);
		++mytask;
	}
// 	qDebug() << "TaskBox::selectionData()" << map;
	return map;
}

pertubis::DBTask* pertubis::TaskBox::addTask(QString name)
{
// 	qDebug() << "TaskBox::addTask()" << name;
	DBTask* t = new DBTask(this);
	std::string sname = name.toLatin1().data();
	m_tasks[sname] = t;
	return t;
}

pertubis::DBTask* pertubis::TaskBox::task(QString name) const
{
	std::string sname(name.toLatin1().data());
	DBTask* res=0;
	std::map<std::string,DBTask*>::const_iterator it=m_tasks.find(sname);
	if (it != m_tasks.end())
	{
		res = it->second;
	}
	return res;
}

QVariantMap pertubis::TaskBox::tasks() const
{
	QVariantMap map;
	std::map<std::string,DBTask*>::const_iterator it=m_tasks.begin();
	while (it != m_tasks.end() )
	{
		map.insert(it->first.c_str(),false);
		++it;
	}
	return map;
}

bool pertubis::TaskBox::hasTask(QString name) const
{
	std::string sname(name.toLatin1().data());
	if (m_tasks.find(sname) != m_tasks.end())
		return true;
	return false;
}

void pertubis::TaskBox::slotTaskChanged(Item* item,QString tname,bool state)
{
	qDebug() << "TaskBox::slotTaskChanged" << item << tname << state;

	DBTask* t = task(tname);
	if(t == 0)
		return;
	t->changeEntry(item->ID(),state);
	item->disconnect(this);
}
