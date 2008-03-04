
/* Copyright (C) 2007 Stefan Koegl
*
* This file is part of perturbis
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SELECTION_MODEL_DELEGATE_H
#define _PERTUBIS_ENTRY_PROTECTOR_SELECTION_MODEL_DELEGATE_H 1

#include <QItemDelegate>

class QMenu;
class QSortFilterProxyModel;

#include <QSize>

namespace perturbis
{

    /*! \brief Used for presenting a customized selections display in PackageView
    * \ingroup ItemModelClass
    */
    class SelectionModelDelegate : public QItemDelegate
    {
//         Q_OBJECT

    public:

        SelectionModelDelegate(QWidget *parent,
                        int install,
                        int deinstall,
                        int installed);

        void paint( QPainter* painter,
                    const QStyleOptionViewItem& option,
                    const QModelIndex& index) const;

    private:

        QMenu*                  m_menu;
        int                     m_installColumn;
        int                     m_deinstallColumn;
        int                     m_installedColumn;
    };
}

#endif
