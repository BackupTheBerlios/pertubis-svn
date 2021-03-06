
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
	QStringList findQmFiles();
	QString languageName(const QString &qmFile);


	/*! \brief This settings group handles language and localization specific options.
	*
	*/
	class I18NPage : public QWidget
	{
		Q_OBJECT

		public:
			I18NPage(QWidget *parent = 0);
			~I18NPage();

			void loadSettings();
			void saveSettings();


			QMap<QString,QString> langToTranslation;
			QString				m_currentLanguage;

		public slots:
			void languageChanged(const QString& language);

	};

	/*! \brief Presents a settings window with different configuration groups.
	*
	*/
	class Settings : public QDialog
	{
		Q_OBJECT

		public:

			Settings(QWidget* parent);

			const QString& currentLang() const { return m_i18n->m_currentLanguage;}

		public slots:
     		void changePage(QListWidgetItem *current, QListWidgetItem *previous);

		private:

			QListWidget *contentsWidget;
     		QStackedWidget *pagesWidget;
			I18NPage*		m_i18n;
	};
}

#endif
