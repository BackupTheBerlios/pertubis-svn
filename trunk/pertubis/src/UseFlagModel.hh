
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_USEFLAGMODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_USEFLAGMODEL_H

#include <QMap>

#include <QAbstractListModel>

#include <tr1/memory>
#include <QThread>
#include <QList>
#include <QStringList>

namespace paludis
{
    class Environment;
	class Repository;
}

namespace pertubis
{

	class UseFlagModel : public QAbstractListModel
	{
		Q_OBJECT
	public:

		typedef QMap<QString,QString> Desc;
		typedef QMap<QString,Desc> DescSet;
		typedef QMap<QString,DescSet> RepoDescSet;

		typedef QMap<QString,bool> UseFlagSet;
		typedef QMap<QString,UseFlagSet> RepoUseFlagSet;

		UseFlagModel(std::tr1::shared_ptr<paludis::Environment> env, QObject* parent);
		~UseFlagModel();

		void refreshModel();

		QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		void setHorizontalHeaderLabels ( const QStringList & labels );

		int rowCount( const QModelIndex & index ) const;
		int columnCount( const QModelIndex & index ) const;
		QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

		DescSet							m_descriptions;
		DescSet							m_environVariables;
		RepoUseFlagSet					m_useflags;
		QStringList						m_header;
		std::tr1::shared_ptr<paludis::Environment> m_env;

	public slots:
		void slotPopulateModel(QStringList cl);
	};

}
#endif

