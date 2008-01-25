
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_MODEL_H 1

#include "Package-fwd.hh"

#include <QAbstractItemModel>
#include <QStringList>
#include <QModelIndex>
#include <QVariant>
#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id-fwd.hh>

class QAction;

namespace pertubis
{
    /*! \brief holds package data fetched from paludis in a tree structure
     *
     * \ingroup PackageModelClass
     */
    class PackageModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:

        PackageModel(QObject* parent);

        virtual ~PackageModel();

        ///@name Content information
        ///@{

        QVariant data ( const QModelIndex & index,
                        int role = Qt::DisplayRole ) const;

        Package* getRootPackage() const { return m_root;}

        QModelIndex index ( int row, int column,
                            const QModelIndex & parent = QModelIndex() ) const;

        QModelIndex parent ( const QModelIndex & index ) const;

        int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

        int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

        QVariant headerData(int section,
                            Qt::Orientation orientation,
                            int role) const;

         ///@name Content modification
        ///@{

        /// sets displayable column data at QModelIndex index
        bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

        bool setHeaderData ( int section,
                             Qt::Orientation orientation,
                             const QVariant & value,
                             int role = Qt::EditRole );

        void setHorizontalHeaderLabels ( const QStringList & labels );

        ///@}

    public slots:

        virtual void appendPackage(Package* item);
        virtual void prependPackage(Package* item);
        virtual void clear();
        virtual void onUpdateModel() { emit layoutChanged();}

    protected:

        Package*        m_root;

    private:

        QModelIndex createIndex ( int row, int column, void * ptr = 0 ) const;

        QStringList     m_header;
    };

    /** \brief creates a node to diplay a package, can have version subnodes
     * \ingroup PackageModelClass
     *
     */
    Package* makePackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                         Qt::CheckState install,
                         Qt::CheckState deinstall,
                         QString pack,
                         QString cat,
                         Qt::CheckState isInstalled,
                         PackageState mystate,
                         Package* myparent,
                         QString mask_reasons);

    /** \brief creates a subnode to display a version
     * \ingroup PackageModelClass
     *
     */
    Package* makeVersionPackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                                Qt::CheckState install,
                                Qt::CheckState deinstall,
                                QString version,
                                QString rep,
                                Qt::CheckState isInstalled,
                                PackageState mystate,
                                Package* pitem,
                                QString mask_reasons);

    Package* makeRootPackage();


}

#endif
