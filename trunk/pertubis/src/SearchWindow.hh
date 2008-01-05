
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
class QPushButton;
class QDialogButtonBox;
class QProgressBar;


/*! \brief all classes and functions are in this namespace
 *
 */
namespace pertubis
{

    class QuerySettingsModel;
    class QuerySettingsView;

    /*! \brief provides a text input and option checkboxes the user may select or deselect.
     *
     * \ingroup Widget
     * \todo This class should display a dropdown menu allowing the user to choose the metadata keys to search in
     */
    class SearchWindow : public QDialog
    {
        friend class SearchPage;
        Q_OBJECT

        public:

            ///\name Constructors
            ///\{

            /// constructs a SearchWindow object
            SearchWindow( QWidget *pobj, QuerySettingsModel* querySettings);

            ~SearchWindow();
            ///@}


            QString text();

            QProgressBar*           m_bar;

        private:


            void loadSettings();
            void saveSettings();

        private slots:

            void progress(int);

        public slots:

            void displaySearch(bool start);

        private:
            /// ask for query settings
            QuerySettingsModel*     m_querySettings;
            QLineEdit*              m_line;
            QPushButton*            m_bStart;
            QPushButton*            m_bStop;
            QPushButton*            m_bClose;
            QPushButton*            m_bOptions;
    };
}
#endif

