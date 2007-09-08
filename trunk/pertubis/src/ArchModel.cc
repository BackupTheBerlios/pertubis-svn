
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

#include "ArchModel.hh"

#include <QFile>
#include <QTextStream>

int pertubis::ArchModel::rowCount(const QModelIndex &/* parent*/) const
{
    return m_knownArches.count();
}

int pertubis::ArchModel::columnCount(const QModelIndex &/* parent*/) const
{
    return 1;
}

QVariant pertubis::ArchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical || section != 1 )
        return QVariant();
    return m_header;
}

QVariant pertubis::ArchModel::data ( const QModelIndex & m_index, int role) const
{
    if (!m_index.isValid())
         return QVariant();

     if (m_index.row() >= m_knownArches.size())
         return QVariant();

     if (role == Qt::DisplayRole)
         return m_knownArches.at(m_index.row());
     else
         return QVariant();
}

void pertubis::ArchModel::initKnownArches(QString path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly) )
    {
        QTextStream stream( &file );
        QString line;

        while ( !stream.atEnd() )
        {
            line = stream.readLine();
            if (!line.isEmpty() && !line.isNull())
            {
                m_knownArches << line;
            }
        }
        file.close();
    }
}
