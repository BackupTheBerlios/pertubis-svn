
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SEARCH_WINDOW_H
#define _PERTUBIS_ENTRY_PROTECTOR_SEARCH_WINDOW_H 1

#include <QDialog>

class QLineEdit;
class QCheckBox;
class QDialogButtonBox;

namespace pertubis
{
    /*! \brief provides a text input and option checkboxes the user may select or deselect.
     *
     * \ingroup Widget
     * \todo This class should display a dropdown menu allowing the user to choose the metadata keys to search in
     */
    class SearchWindow : public QDialog
    {
        Q_OBJECT

    public:

        ///\name Constructors
        ///\{

        /// constructs a SearchWindow object
        SearchWindow( QWidget *pwid = 0);
        ///@}

        /// returns the query string
        QString query() const;
        /// returns if name is checked
        bool inName() const { return m_chkName;}
        /// returns if description is checked
        bool inDesc() const { return m_chkDesc;}
        /// returns if homepage is checked
        bool inHomepage() const { return m_chkHomepage;}
        /// returns if "regular expression" is checked
        bool asRegex() const { return m_chkRegex;}

    signals:
        /// sends the request to start the search
        void                search();

        /// sends the request to stop the search
        void                stopSearch();

    private:

        /// line for query input
        QLineEdit*            m_line;
        /// a checkbox for searching in package description
        QCheckBox*            m_chkDesc;
        /// a checkbox for searching in package name
        QCheckBox*            m_chkName;
        /// a checkbox for searching in package homepage
        QCheckBox*            m_chkHomepage;
        /// a checkbox to treat the query string as a regular expression
        QCheckBox*            m_chkRegex;
        /// a button box for the usual buttons
        QDialogButtonBox*    m_dbox;
    };
}
#endif

