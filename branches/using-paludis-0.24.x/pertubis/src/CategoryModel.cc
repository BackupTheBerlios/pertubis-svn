
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

#include "CategoryModel.hh"


#include <QSet>
#include <QString>

pertubis::CategoryModel::CategoryModel(QObject* parent) : QAbstractListModel(parent)
{
}

pertubis::CategoryModel::~CategoryModel()
{
}

void pertubis::CategoryModel::setHorizontalHeaderLabels ( const QStringList & labels )
{
	m_header = labels;
}

QVariant pertubis::CategoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation == Qt::Vertical || section >= m_header.count() )
		return QVariant();
	return *(m_header.begin() + section);
}

void pertubis::CategoryModel::slotPopulateModel(QStringList cl)
{
	QString cat;
	foreach (cat,cl)
	{
		m_data << cat;
	}
	m_data.sort();
	reset();
}

int pertubis::CategoryModel::rowCount( const QModelIndex & parent ) const
{
	return parent.isValid() ? 0 : m_data.count();
}

int pertubis::CategoryModel::columnCount( const QModelIndex & parent ) const
{
	return parent.isValid() ? 0 : m_header.count();
}

QVariant pertubis::CategoryModel::data ( const QModelIndex & index, int role) const
{
	if (!index.isValid())
         return QVariant();

     if (index.row() >= m_data.size())
         return QVariant();

     if (role == Qt::DisplayRole)
         return m_data.at(index.row());
     else
         return QVariant();
}
