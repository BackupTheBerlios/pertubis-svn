
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_KEYWORD_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_KEYWORD_MODEL_H

#include <QAbstractItemModel>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVariant>

namespace pertubis
{
	/*! \brief model stores different keyword sets
	* for views this datastructure seems to be a 3-level-tree
	* unnamed_root
	* -> keyword_set(s)
	*   -> keyword(s)
	*/

	class ArchModel : public QAbstractItemModel
	{
		public:
			ArchModel(){}
			~ArchModel(){}

			int classifyKeywords(QSet<QString> keywords, QString keyset);

			int rowCount(const QModelIndex & parent = QModelIndex()) const;
			int columnCount(const QModelIndex & parent = QModelIndex()) const;
// 			QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
			QVariant data(const QModelIndex & parent,int role) const;
			QVariant headerData(int section,Qt::Orientation orientation,int role) const;

		private:

			QSet<QString> KeywordsAt(int offset) const;

// 			void loadKeywords(QString path = QString("/etc/paludis/keywords.conf"));
			void initKnownArches(QString path);

			QString		m_header;
			QStringList m_knownArches;
	};
}

#endif

