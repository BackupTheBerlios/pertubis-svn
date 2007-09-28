
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

pertubis::I18NPage::I18NPage(QWidget *pobj)
     : QWidget(pobj)
{
    QGroupBox *packagesGroup = new QGroupBox(tr("Language Settings"));
    QLabel *nameLabel = new QLabel(tr("Language:"));

    QComboBox *choice = new QComboBox;

    loadSettings();

    QStringList list = findQmFiles();
    QString filename;
    int i=0;
    foreach (filename,list)
    {
        qDebug() << "filename" << i << filename;
        QString name = languageName(filename);
        langToTranslation.insert(name,filename);
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
    m_currentLanguage=langToTranslation[language];
    QMessageBox msgBox;
    msgBox.information(this,tr("pertubis info"),tr("language setting changes will be applied after a restart"));
}

QString pertubis::languageName(const QString &qmFile)
{
    QString prefix(qmFile.split("-").value(1));
    qDebug() << qmFile << prefix;
    if (prefix == "de")
        return ("Deutsch");
    if (prefix == "fr")
        return ("français");
    return ("English");
}

pertubis::Settings::Settings(QWidget* pobj) : QDialog(pobj)
{
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(96, 84));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(128);
    contentsWidget->setSpacing(12);

    pagesWidget = new QStackedWidget;
    m_i18n = new I18NPage;
    pagesWidget->addWidget(m_i18n);

    QPushButton *closeButton = new QPushButton(tr("Close"));

    connect(contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this,
            SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));

    connect(closeButton,
            SIGNAL(clicked()),
            this,
            SLOT(close()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);


    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    QListWidgetItem *tItem = new QListWidgetItem(contentsWidget);
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

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

QStringList pertubis::findQmFiles()
{
    QDir dir(":i18n");
    QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files,
                                        QDir::Name);

    QMutableStringListIterator i(fileNames);

    while (i.hasNext())
    {
        i.next();
         QFileInfo info(i.value());
        QString tmp(dir.filePath(info.baseName()));
        qDebug() << "basename" << tmp;
        i.setValue(tmp);
    }

    return fileNames;
}
