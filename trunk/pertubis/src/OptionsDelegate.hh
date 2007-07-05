
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_OPTIONS_DELEGATE_H
#define _PERTUBIS_ENTRY_PROTECTOR_OPTIONS_DELEGATE_H

#include <QItemDelegate>

class QMenu;
#include <QPixmap>

#include <QMetaType>

namespace pertubis
{
	class MyDisplay
	{

	public:
		MyDisplay();
		MyDisplay(const QVariantMap& map);
		MyDisplay(const MyDisplay& d);

		void paint(QPainter *painter, const QRect &rect,
               const QPalette &palette) const;
	private:
		QVariantMap		m_data;
		QMap<QString,QPixmap>	m_pix;
	};

	class OptionsDelegate : public QItemDelegate
	{
		Q_OBJECT

	public:
		OptionsDelegate(QWidget *parent = 0);

		void paint(QPainter* painter,const QStyleOptionViewItem& option,const QModelIndex& index) const;
		QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;

	private:

		QMenu*			m_menu;


// 	private slots:
//  		void emitCommitData();
	};
}

Q_DECLARE_METATYPE(pertubis::MyDisplay)

#endif

