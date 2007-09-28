
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of the pertubis frontend for paludis package manager.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef _PERTUBIS_ENTRY_PROTECTOR_KEYWORD_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_KEYWORD_MODEL_H 1

#include <QAbstractItemModel>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVariant>

namespace pertubis
{
    /*!\brief model stores all known keywords the gentoo system provides
    *
    */

    class ArchModel : public QAbstractItemModel
    {
        public:
            ArchModel(){}
            ~ArchModel(){}

            int classifyKeywords(QSet<QString> keywords, QString keyset);
            int rowCount(const QModelIndex & parent = QModelIndex()) const;
            int columnCount(const QModelIndex & parent = QModelIndex()) const;
            QVariant data(const QModelIndex & parent,int role) const;
            QVariant headerData(int section,Qt::Orientation orientation,int role) const;

        private:

            QSet<QString> KeywordsAt(int offset) const;

            void initKnownArches(QString path);

            QString        m_header;
            QStringList m_knownArches;
    };
}

#endif

