
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

#include "Settings.hh"
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QStackedWidget>
#include <QVBoxLayout>

static QStringList findTranslationFiles()
{
    QDir dir(":i18n");
    QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files,
                                          QDir::Name);


    for (QList<QString>::iterator i(fileNames.begin()), iEnd(fileNames.end());
        i != iEnd;++i)
    {
        QFileInfo info(*i);
        QString tmp(dir.filePath(info.baseName()));
        qDebug() << "pertubis::findTranslationFiles() - found" << tmp;
        *i = tmp;
    }

    return fileNames;
}


static QString languageName(const QString &qmFile)
{
    QString prefix(qmFile.split("-").value(1));
    if (prefix == "de")
        return ("Deutsch");
    if (prefix == "fr")
        return ("Français");
    return ("English");
}

pertubis::I18NPage::I18NPage(QWidget *pobj)
     : QWidget(pobj)
{
    QGroupBox *packagesGroup = new QGroupBox(tr("Language Settings"));
    QLabel *nameLabel = new QLabel(tr("Language:"));

    QComboBox *choice = new QComboBox;

    loadSettings();

    QStringList list = findTranslationFiles();
    QString filename;
    int i=0;
    foreach (filename,list)
    {
        QString name = languageName(filename);
        m_langToTranslation.insert(name,filename);
        choice->addItem(name);
        if (filename == m_currentLanguage)
            choice->setCurrentIndex(i);
        ++i;
    }

    connect(choice,
            SIGNAL(currentIndexChanged(const QString&)),
            this,
            SLOT(languageChanged(const QString&)));

    QGridLayout *packagesLayout = new QGridLayout;
    packagesLayout->addWidget(nameLabel, 0, 0);
    packagesLayout->addWidget(choice, 0, 1);
    packagesGroup->setLayout(packagesLayout);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(packagesGroup);
    mainLayout->addSpacing(12);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

pertubis::I18NPage::~I18NPage()
{
    qDebug("I18NPage::~I18NPage() - start");
    saveSettings();
    qDebug("I18NPage::~I18NPage() - done");
}

void pertubis::I18NPage::loadSettings()
{
    QSettings settings;
    settings.beginGroup( "i18npage" );
        setVisible(settings.value( "visible",true).toBool());
        resize(settings.value("size",QVariant(QSize(800,600))).toSize());
        move(settings.value("pos",QVariant(QPoint(341,21))).toPoint());
        m_currentLanguage=settings.value("language",":i18n/pertubis-de").toString();
    settings.endGroup();
}

void pertubis::I18NPage::saveSettings()
{
    QSettings settings;
    settings.beginGroup( "i18npage" );
        settings.setValue("visible", isVisible() );
        settings.setValue("size", size() );
        settings.setValue("pos", pos());
        settings.setValue("language",m_currentLanguage);
    settings.endGroup();
}

void pertubis::I18NPage::languageChanged(const QString& language)
{
    m_currentLanguage=m_langToTranslation[language];
    QMessageBox msgBox;
    msgBox.information(this,tr("pertubis info"),tr("language setting changes will be applied after a restart"));
}

pertubis::Settings::Settings(QWidget* pobj) : QDialog(pobj)
{
    m_contentsWidget = new QListWidget;
    m_contentsWidget->setViewMode(QListView::IconMode);
    m_contentsWidget->setIconSize(QSize(96, 84));
    m_contentsWidget->setMovement(QListView::Static);
    m_contentsWidget->setMaximumWidth(128);
    m_contentsWidget->setSpacing(12);

    m_pagesWidget = new QStackedWidget;
    m_i18n = new I18NPage;
    m_pagesWidget->addWidget(m_i18n);

    QPushButton *closeButton = new QPushButton(tr("Close"));

    connect(m_contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this,
            SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));

    connect(closeButton,
            SIGNAL(clicked()),
            this,
            SLOT(close()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(m_contentsWidget);
    horizontalLayout->addWidget(m_pagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);


    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    QListWidgetItem *tItem = new QListWidgetItem(m_contentsWidget);
    tItem->setIcon(QIcon(":/images/settings.png"));
    tItem->setText(tr("Configuration"));
    tItem->setTextAlignment(Qt::AlignHCenter);
    tItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    setWindowTitle(tr("pertubis :: settings"));
}

void pertubis::Settings::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    m_pagesWidget->setCurrentIndex(m_contentsWidget->row(current));
}

