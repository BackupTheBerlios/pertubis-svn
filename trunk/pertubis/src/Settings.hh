
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SETTINGS_H
#define _PERTUBIS_ENTRY_PROTECTOR_SETTINGS_H

class QStackedWidget;
class QListWidget;
class QListWidgetItem;
class QTranslator;

#include <QDialog>
#include <QMap>
#include <QString>

namespace pertubis
{

    /*! \brief This settings group handles language and localization specific options.
    *
    */
    class I18NPage : public QWidget
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            I18NPage(QWidget *parent = 0);
            ///@}

            ~I18NPage();

            ///@name Content modification
            ///@{

            /// loads all settings for the gui
            void loadSettings();

            /// saves all settings for the gui
            void saveSettings();

            ///@}

            /// maps language to translation file
            QMap<QString,QString>  m_langToTranslation;

            /// the current language to display
            QString                m_currentLanguage;

        public slots:

            /// changes the actual language
            void languageChanged(const QString& language);

    };

    /*! \brief Presents a settings window with different configuration groups.
    *
    */
    class Settings : public QDialog
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            Settings(QWidget* parent);
            ///@}

            ///@name Content information
            ///@{

            /// returns the current language
            QString currentLang() const { return m_i18n->m_currentLanguage;}
            ///@}

        public slots:

            /// changes the actual settings page to display to current
            void changePage(QListWidgetItem *current, QListWidgetItem *previous);

        private:

            /// a list widget
            QListWidget*        m_contentsWidget;

            /// stacked pages
            QStackedWidget*     m_pagesWidget;

            /// the language settings page
            I18NPage*           m_i18n;
    };
}

#endif
