
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_THREAD_FETCH_ITEM_H
#define _PERTUBIS_ENTRY_PROTECTOR_THREAD_FETCH_ITEM_H

#include "ThreadBase.hh"

#include <QString>

namespace pertubis
{
	class Item;
	class TaskBox;
	class ThreadKeywordManager;

	/*! \brief this thread is used in conjunction with SearchWindow. Returns a complete item tree.
	*
	*/
	class ThreadFetchItem : public ThreadBase
	{
		Q_OBJECT

	public:

		ThreadFetchItem(QObject* parent,
						paludis::tr1::shared_ptr<paludis::Environment> env,
	  					ThreadKeywordManager* keywords,
						TaskBox* box);

		void search(QString str,bool name,bool desc);

	protected:

		void run();

	private:

		QString									m_query;
		bool									m_optName;
		bool									m_optDesc;

		ThreadKeywordManager*					m_keywords;
		TaskBox*								m_box;

	signals:
		void itemResult(Item* root);
	};
}

#endif
