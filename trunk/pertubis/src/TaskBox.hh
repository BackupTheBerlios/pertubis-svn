
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

#include <paludis/util/tr1_memory.hh>

#include <vector>
#include <map>
#include <set>
#include <string>

namespace paludis
{
	class PackageID;
}

namespace pertubis
{
	class Item;

	/* \brief Description inside pertubis of a discrete package "P" in category "C" at version "V" in repository "R"
	*
	*/
// 	class Entry
// 	{
// 	public:
// 		Entry(const char* c, const char* p,const char* v,const char* r) : cat(c),pack(p),version(v),rep(r) {}
// 		std::string cat;
// 		std::string pack;
// 		std::string version;
// 		std::string rep;
// 	};

// 	inline Entry genPackEntry(QString cat,QString pack,QString ver,QString rep)
// 	{
// 		return Entry(cat.toLatin1().data(),pack.toLatin1().data(),ver.toLatin1().data(),rep.toLatin1().data());
// 	}

// 	bool operator<(const Entry& a,const Entry& b);

	/*! \brief convenience class around a set which holds Entry items
	*
	*/
	class DBTask : public QObject
	{
		Q_OBJECT

	public:

		DBTask(QObject* pobject);

		void addEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id);
		bool hasEntry(paludis::tr1::shared_ptr<const paludis::PackageID> id);
// 		bool hasEntry(QString pack,QString cat,QString ver,QString rep);

	public slots:

		void changeEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id,bool);

	private:
		std::set<paludis::tr1::shared_ptr<const paludis::PackageID> >	m_data;
	};

	/*! \brief manages the tasks pertubis knows about
	*
	*/
	class TaskBox : public QThread
	{
		Q_OBJECT
	public:
		TaskBox(QObject* pobject) : QThread(pobject) {}
		DBTask* addTask(QString name);
		DBTask* task(QString name) const;
		bool hasTask(QString name) const;
		QVariantMap tasks() const;
		QVariantMap selectionData(paludis::tr1::shared_ptr<const paludis::PackageID> id);

		void run() {}

	public slots:
		void slotTaskChanged(Item* item, QString task, bool state);

	private:
		std::map<std::string,DBTask*>				m_tasks;
	};
}

#endif
