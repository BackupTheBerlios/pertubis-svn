
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_TASKBOX_H
#define _PERTUBIS_ENTRY_PROTECTOR_TASKBOX_H

#include <QString>
#include <QAction>
#include <QThread>

#include <vector>
#include <map>
#include <set>
#include <string>

namespace pertubis
{
	class Item;

	class Entry
	{
	public:
		Entry(const char* c, const char* p,const char* v,const char* r) : cat(c),pack(p),version(v),rep(r) {}
		std::string cat;
		std::string pack;
		std::string version;
		std::string rep;
	};

	inline Entry genPackEntry(QString cat,QString pack,QString ver,QString rep)
	{
		return Entry(cat.toLatin1().data(),pack.toLatin1().data(),ver.toLatin1().data(),rep.toLatin1().data());
	}

	bool operator<(const Entry& a,const Entry& b);

	class DBTask : public QObject
	{
		Q_OBJECT

	public:

		DBTask(QObject* parent);

		void addEntry(const Entry&);

		bool hasEntry(const Entry&);
		bool hasEntry(QString pack,QString cat,QString ver,QString rep);

	public slots:

		void changeEntry(const Entry&,bool);

	private:
		std::set<Entry >	m_data;
	};

	class TaskBox : public QThread
	{
		Q_OBJECT
	public:
		TaskBox(QObject* parent) : QThread(parent) {}
		DBTask* addTask(QString name);
		DBTask* task(QString name) const;
		bool hasTask(QString name) const;
		QVariantMap tasks() const;
		QVariantMap selectionData(const pertubis::Entry& pack);

		void run() {}

	public slots:
		void slotTaskChanged(Item* item, QString task, bool state);

	private:
		std::map<std::string,DBTask*>				m_tasks;
	};
}

#endif
