
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_INFO_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_INFO_MODEL_H 1

#include <QList>
#include <QObject>
#include <QVariant>
#include <QStringList>
#include "ThreadBase.hh"
#include <QAbstractTableModel>
#include <paludis/util/tr1_memory.hh>
#include <paludis/environment-fwd.hh>
#include <paludis/util/visitor.hh>
#include <paludis/metadata_key.hh>

namespace pertubis
{
    class RepositoryInfoThread;

    class InfoDisplayer :
            public paludis::ConstVisitor<paludis::MetadataKeyVisitorTypes>
            {
                RepositoryInfoThread* m_thread;
                public:

                    InfoDisplayer(RepositoryInfoThread* thread) : m_thread(thread)
                    {
                    }

                    void visit(const paludis::MetadataSectionKey & k);
                    void visit(const paludis::MetadataStringKey & k);
                    void visit(const paludis::MetadataFSEntryKey & k);
                    void visit(const paludis::MetadataPackageIDKey & k);
                    void visit(const paludis::MetadataRepositoryMaskInfoKey & k);
                    void visit(const paludis::MetadataContentsKey & k);
                    void visit(const paludis::MetadataTimeKey & k);
                    void visit(const paludis::MetadataSpecTreeKey<paludis::RestrictSpecTree> & k);
                    void visit(const paludis::MetadataSpecTreeKey<paludis::LicenseSpecTree> & k);
                    void visit(const paludis::MetadataSpecTreeKey<paludis::FetchableURISpecTree> & k);
                    void visit(const paludis::MetadataSpecTreeKey<paludis::SimpleURISpecTree> & k);
                    void visit(const paludis::MetadataSpecTreeKey<paludis::DependencySpecTree> & k);
                    void visit(const paludis::MetadataSpecTreeKey<paludis::ProvideSpecTree> & k);
                    void visit(const paludis::MetadataCollectionKey<paludis::FSEntrySequence> & k);
                    void visit(const paludis::MetadataCollectionKey<paludis::PackageIDSequence> & k);
                    void visit(const paludis::MetadataCollectionKey<paludis::KeywordNameSet> & k);
                    void visit(const paludis::MetadataCollectionKey<paludis::IUseFlagSet> & k);
                    void visit(const paludis::MetadataCollectionKey<paludis::UseFlagNameSet> & k);
                    void visit(const paludis::MetadataCollectionKey<paludis::Set<std::string> > & k);

            };

    /*! \brief not finished
     *
     */
    class RepositoryInfoThread : public ThreadBase
    {
        Q_OBJECT
        public:
            RepositoryInfoThread(QObject* pobj,
                                 const paludis::tr1::shared_ptr<paludis::Environment>&  env,
                                 TaskBox* box) : ThreadBase(pobj,env,box) {}

            void start(const QString& name);

        protected:

            void run();

        signals:

            void sendResult(const QList<QVariantList>& list);

        public slots:
            void append(QString,QString);

        private:
            QString     m_repName;
            QList<QVariantList> m_list;
    };

    /*! \brief not finished
    *
    */
    class RepositoryInfoModel : public QAbstractTableModel
    {
        Q_OBJECT

    public:

        RepositoryInfoModel(QObject* parent);

        int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

        QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

        void setHorizontalHeaderLabels ( const QStringList & labels );

    public slots:

        void slotResult(const QList<QVariantList>& list);

    private:
        QList<QVariantList>       m_data;
        QStringList               m_header;
    };

}

#endif



