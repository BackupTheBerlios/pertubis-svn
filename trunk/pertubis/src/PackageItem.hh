
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_ITEM_H
#define _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_ITEM_H

#include <QList>
#include <QVariant>

namespace pertubis
{


	class Item : public QObject
	{
		Q_OBJECT

	public:

		enum ItemStatus { is_stable, is_unstable, is_masked };
		enum ItemOrder { io_selected, io_package, io_category, io_repository, io_installed};
		enum ItemType { it_category,it_package,it_version};

		Item(const QList<QVariant> &data, Item* parent,ItemType t);
		virtual ~Item();

		void appendChild(Item *child);

		bool setPackageSelection(QString task,bool state);
		bool setVersionSelection(QString task,bool state);
		void setSelectionData(QString task,bool state);

		bool entryData(QString& cat,QString& pack,QString& ver,QString& rep) const;

		Item *child(int row) const;
		Item *bestChild() const;
		int childCount() const;
		int columnCount() const;
		QVariant data(int column) const;
		void setData(int column,QVariant data);
		int row() const;
		Item *parent();

		QList<Item*>	m_children;
		QList<QVariant>	m_data;
		Item* 			m_parent;
		ItemStatus		m_status;
		ItemType		m_rtti;

	signals:
		void taskChanged(Item* item,QString task,bool state);
	};

	QString status(Item::ItemStatus status);
}

#endif
