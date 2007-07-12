
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

bool pertubis::operator<(const pertubis::Entry& a,const pertubis::Entry& b)
{
	if (a.cat < b.cat)
		return true;
	if (a.cat > b.cat)
		return false;

	if (a.pack < b.pack)
		return true;
	if (a.pack > b.pack)
		return false;

	if (a.version < b.version)
		return true;
	if (a.version > b.version)
		return false;

	if (a.rep < b.rep)
		return true;

	return false;
}

pertubis::DBTask::DBTask(QObject* parent) : QObject(parent)
{
}

void pertubis::DBTask::addEntry(const pertubis::Entry& entry)
{
	m_data.insert(entry);
}

bool pertubis::DBTask::hasEntry(QString pack,QString cat,QString ver,QString rep)
{
	pertubis::Entry p = genPackEntry(pack,cat,ver,rep);
	return hasEntry(p);
}

bool pertubis::DBTask::hasEntry(const pertubis::Entry& spec)
{
	return (m_data.find(spec) != m_data.end());
}

void pertubis::DBTask::changeEntry(const pertubis::Entry& entry,bool state)
{
	std::set<Entry >::const_iterator it = m_data.find(entry);
	if (state)
		addEntry(entry);
	else
		m_data.erase(entry);
}

QVariantMap pertubis::TaskBox::selectionData(const pertubis::Entry& pack)
{
	QVariantMap map;
	std::map<std::string,DBTask*>::const_iterator task = m_tasks.begin();
	while (task != m_tasks.end())
	{
		bool res = task->second->hasEntry(pack);
		map.insert(QString(task->first.c_str()),res);
		++task;
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
	QString cat,pack,ver,rep;
	item->entryData(cat,pack,ver,rep);
	Entry entry = genPackEntry(cat,pack,ver,rep);
	DBTask* t = task(tname);
	if(t == 0)
		return;
	t->changeEntry(entry,state);
	item->disconnect(this);
}
