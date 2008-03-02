
/* Copyright (C) 2007-2008 Stefan Koegl.
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

#include "RepositoryInfoModel.hh"
#include "HtmlFormatter.hh"
#include <QHeaderView>
#include <QDebug>
#include <paludis/repository.hh>
#include <paludis/name.hh>
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/visitor_cast.hh>
#include <paludis/util/visitor-impl.hh>
#include <paludis/util/set.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/package_database.hh>
#include <paludis/util/wrapped_forward_iterator.hh>
#include <paludis/environment.hh>
#include <paludis/util/stringify.hh>
#include <set>

using namespace pertubis;
using namespace paludis;

struct MetadataKeyComparator
{
    bool operator() (const tr1::shared_ptr<const MetadataKey> & a, const tr1::shared_ptr<const MetadataKey> & b) const
    {
        bool a_is_section(visitor_cast<const MetadataSectionKey>(*a));
        bool b_is_section(visitor_cast<const MetadataSectionKey>(*b));
        if (a_is_section != b_is_section)
            return b_is_section;
        if (a->type() != b->type())
            return a->type() < b->type();
        return a->human_name() < b->human_name();
    }
};

namespace
{
    class InfoDisplayer :
            public paludis::ConstVisitor<paludis::MetadataKeyVisitorTypes>
    {
        RepositoryInfoThread* m_thread;
        public:

            InfoDisplayer(RepositoryInfoThread* thread) : m_thread(thread)
            {
            }

            void visit(const MetadataSectionKey & /*k*/)
            {
            }

            void visit(const MetadataStringKey & k)
            {
                m_thread->append(QString::fromStdString(k.human_name()),QString::fromStdString(k.value()));
            }

            void visit(const MetadataFSEntryKey & k)
            {
                m_thread->append(QString::fromStdString(k.human_name()),QString::fromStdString(stringify(k.value())));
            }

            void visit(const MetadataPackageIDKey &)
            {
            }

            void visit(const MetadataRepositoryMaskInfoKey &)
            {
            }

            void visit(const MetadataContentsKey &)
            {
            }

            void visit(const paludis::MetadataSizeKey & k)
            {
                m_thread->append(QString::fromStdString(paludis::stringify(k.human_name())),QString::fromStdString(paludis::stringify(k.pretty_print())));
            }

            void visit(const MetadataTimeKey & k)
            {
                time_t t = k.value();

                if (0 == t)
                    return;
                char buf[255];
                if (strftime(buf, 254, "%c", gmtime(&t)))
                {
                    m_thread->append("time",buf);
                }
            }

            void visit(const MetadataSpecTreeKey<RestrictSpecTree> &)
            {
            }

            void visit(const MetadataSpecTreeKey<LicenseSpecTree> &)
            {
            }

            void visit(const MetadataSpecTreeKey<FetchableURISpecTree> &)
            {
            }

            void visit(const MetadataSpecTreeKey<SimpleURISpecTree> &)
            {
            }

            void visit(const MetadataSpecTreeKey<DependencySpecTree> &)
            {
            }

            void visit(const MetadataSpecTreeKey<ProvideSpecTree> &)
            {
            }

            void visit(const MetadataCollectionKey<FSEntrySequence> & k)
            {
                HtmlFormatter f;
                m_thread->append(QString::fromStdString(k.human_name()), QString::fromStdString(k.pretty_print_flat(f)));
            }

            void visit(const MetadataCollectionKey<PackageIDSequence> & k)
            {
                HtmlFormatter f;
                m_thread->append(QString::fromStdString(k.human_name()), QString::fromStdString(k.pretty_print_flat(f)));
            }

            void visit(const MetadataCollectionKey<KeywordNameSet> & k)
            {
                HtmlFormatter f;
                m_thread->append(QString::fromStdString(k.human_name()), QString::fromStdString(k.pretty_print_flat(f)));
            }

            void visit(const MetadataCollectionKey<IUseFlagSet> & k)
            {
                HtmlFormatter f;
                m_thread->append(QString::fromStdString(k.human_name()),QString::fromStdString(k.pretty_print_flat(f)));
            }

            void visit(const MetadataCollectionKey<UseFlagNameSet> & k)
            {
                HtmlFormatter f;
                m_thread->append(QString::fromStdString(k.human_name()), QString::fromStdString(k.pretty_print_flat(f)));
            }

            void visit(const MetadataCollectionKey<Set<std::string> > & k)
            {
                HtmlFormatter f;
                m_thread->append(QString::fromStdString(k.human_name()) , QString::fromStdString(k.pretty_print_flat(f)));
            }
    };
}

void
RepositoryInfoThread::setup(const QString& name)
{
    m_repName = name;
}

void
RepositoryInfoModel::clear()
{
    m_data.clear();
    emit layoutChanged();
}

RepositoryInfoModel::RepositoryInfoModel(QObject* pobj) : QAbstractTableModel(pobj)
{

}

QVariant
RepositoryInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical || section >= m_header.count())
        return QVariant();
    return *(m_header.begin() + section);
}

void
RepositoryInfoThread ::run()
{
    using namespace paludis;
    m_list.clear();

    RepositoryName repoName(m_repName.toStdString());
    tr1::shared_ptr<Repository> repo = env()->package_database()->fetch_repository(repoName);

    std::set<tr1::shared_ptr<const MetadataKey>, MetadataKeyComparator> keys(repo->begin_metadata(),repo->end_metadata());
    InfoDisplayer i(this);
    std::for_each(indirect_iterator(keys.begin()), indirect_iterator(keys.end()), accept_visitor(i));

    emit sendResult(m_list);
}

void
RepositoryInfoThread::append(QString a,QString b)
{
    m_list.push_back(QVariantList() << a << b);
}

void
RepositoryInfoModel::slotResult(const QList<QVariantList>& list)
{
    m_data = list;
    reset();
}

QVariant
RepositoryInfoModel::data (const QModelIndex & ix, int role) const
{
    if (!ix.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        return m_data.value(ix.row()).value(ix.column());
    }
//     if (role == Qt::TextAlignmentRole)
//         return Qt::AlignHCenter;

    return QVariant();
}

void
RepositoryInfoModel::setHorizontalHeaderLabels (const QStringList & labels)
{
    m_header = labels;
}

int
RepositoryInfoModel::rowCount(const QModelIndex& /*pmi*/) const
{
    return m_data.count();
}

int
RepositoryInfoModel::columnCount (const QModelIndex& /*parent*/) const
{
    return 2;
}
