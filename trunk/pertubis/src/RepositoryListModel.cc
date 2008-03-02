
/* Copyright (C) 2007-2008 Stefan Koegl
*
* This file is part of pertubis
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

#include "RepositoryListModel.hh"
#include <paludis/environment.hh>
#include <paludis/name.hh>
#include <paludis/package_database.hh>
#include <paludis/repository.hh>
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/wrapped_forward_iterator.hh>

#include <QSet>
#include <QList>
#include <QSettings>
#include <QString>
#include <QDebug>
#include <QColor>
#include <QBrush>

using namespace pertubis;

RepositoryListItem::RepositoryListItem() : m_data(QList<QVariant>() << "" << Qt::Checked)
{
}

RepositoryListItem::RepositoryListItem(const QString& name, Qt::CheckState state) :
        m_data(QVariantList() << name << state)
{
}

bool
RepositoryListItem::setData(int col,const QVariant& value)
{
    if (m_data.count() > col)
    {
        m_data.replace(col,value);
        return true;
    }
    return false;
}

void
RepositoryListThread::run()
{
    using namespace paludis;
    QStringList list;
    for (IndirectIterator<PackageDatabase::RepositoryConstIterator, const Repository>
         r((*env()).package_database()->begin_repositories()), r_end((*env()).package_database()->end_repositories()) ;
         r != r_end ; ++r)
    {
        list << QString::fromStdString(paludis::stringify(r->name()));
    }
    emit sendNames(list);
}

RepositoryListModel::RepositoryListModel(QObject* pobj,bool firstrun) : QAbstractTableModel(pobj) , m_firstrun(firstrun)
{
    loadSettings();
}

RepositoryListModel::~RepositoryListModel()
{
    clear();
    saveSettings();
}

void
RepositoryListModel::clear()
{
    qDeleteAll(m_data);
    m_data.clear();
    emit layoutChanged();
}

Qt::ItemFlags
RepositoryListModel::flags(const QModelIndex &mix) const
{
    switch (mix.column())
    {
        case rlmho_on:
            return Qt::ItemIsUserCheckable;
            break;
        default:
            return 0;
    }
    return 0;
}

bool
RepositoryListModel::setData (const QModelIndex & ix, const QVariant & value, int /*role*/)
{
    if (!ix.isValid() || ix.column() != rlmho_on)
        return false;
    m_data.value(ix.row())->setData(rlmho_on,value);
    changeActiveRepos(m_data.value(ix.row())->data(rlmho_name).toString());
    emit dataChanged(ix,ix);
    return true;
}

void
RepositoryListModel::changeActiveRepos(const QString& name)
{
    QSet<QString>::iterator it(m_activeRepos.find(name));
    if (it == m_activeRepos.end())
        m_activeRepos.insert(name);
    else
        m_activeRepos.erase(it);
}

void
RepositoryListModel::setHorizontalHeaderLabels (const QStringList & labels)
{
    m_header = labels;
}

QVariant
RepositoryListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole ||
        orientation == Qt::Vertical ||
        section >= m_header.count())
        return QVariant();
    return *(m_header.begin() + section);
}

void
RepositoryListModel::slotResult(QStringList list)
{
    QSet<QString> tmp;
    for (QStringList::const_iterator iter(list.constBegin()),iEnd(list.constEnd());iter != iEnd;++iter)
    {
        tmp.insert(*iter);
        if (m_firstrun)
        {
            changeActiveRepos(*iter);
        }
        RepositoryListItem* item(new RepositoryListItem(*iter,(m_activeRepos.contains(*iter)  ? Qt::Checked : Qt::Unchecked)));
        m_data.push_back(item);
    }

    // delete possibly unavailable repositories
    m_activeRepos.intersect(tmp);
    emit sendNames(list);
    reset();
}

int
RepositoryListModel::rowCount(const QModelIndex & pobj) const
{
    return pobj.isValid() ? 0 : m_data.count();
}

int
RepositoryListModel::columnCount(const QModelIndex & pobj) const
{
    return pobj.isValid() ? 0 : m_header.count();
}

QVariant
RepositoryListModel::data (const QModelIndex & m_index, int role) const
{
//     qDebug() << "RepositoryListModel::data()";
    if (!m_index.isValid())
         return QVariant();

    if (role == Qt::ForegroundRole)
    {
        return QBrush(QColor(0,0,0));
    }

    else if (role == Qt::DisplayRole && m_index.column() == rlmho_name)
    {
        return m_data.value(m_index.row())->data(rlmho_name);
    }

    else if (role == Qt::CheckStateRole && m_index.column() == rlmho_on)
    {
        return m_data.value(m_index.row())->data(rlmho_on);
    }

    return QVariant();
}

const QSet<QString>
RepositoryListModel::activeRepositories() const
{
    return m_activeRepos;
}

void
RepositoryListModel::loadSettings()
{
    qDebug() << "RepositoryListModel::loadSettings() - start";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup("RepositoryListModel");
    QVariantList selected(settings.value("activeRepositories",QVariantList()).toList());
    settings.endGroup();
    // here we injecting all repository names despite its availability since at
    for (QVariantList::const_iterator it(selected.constBegin()),iEnd(selected.constEnd());
         it != iEnd;++it)
    {
        m_activeRepos.insert((*it).toString());
    }
    qDebug() << "RepositoryListModel::loadSettings() - done";
}

void RepositoryListModel::saveSettings()
{
    qDebug() << "RepositoryListModel::saveSettings() - start";
    QVariantList selected;
    for (QSet<QString>::const_iterator it(m_activeRepos.constBegin()),iEnd(m_activeRepos.constEnd());
         it != iEnd;++it)
    {
        selected.push_back(*it);
    }
    qDebug() << selected;
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup("RepositoryListModel");
    settings.setValue("activeRepositories", selected);
    settings.endGroup();
}
