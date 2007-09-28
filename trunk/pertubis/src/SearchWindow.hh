
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SEARCH_WINDOW_H
#define _PERTUBIS_ENTRY_PROTECTOR_SEARCH_WINDOW_H 1

#include <QDialog>

class QGroupBox;
class QLineEdit;
class QLabel;
class QCheckBox;
class QDialogButtonBox;

namespace pertubis
{
    /*! \brief provides a text input and option checkboxes the user may select or deselect.
    *
    */
    class SearchWindow : public QDialog
    {
        Q_OBJECT

    public:
        SearchWindow( QWidget *pwid = 0);

        QLineEdit*            m_line;
        QCheckBox*            m_chkDesc;
        QCheckBox*            m_chkName;
        QCheckBox*            m_chkRegex;
        QDialogButtonBox*    m_dbox;

    protected:
        void initCategories();

        QCheckBox*            m_chkHomepage;
    signals:
        void                search();
    };
}
#endif

