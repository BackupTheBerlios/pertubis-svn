
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_USEFLAG_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_USEFLAG_MODEL_H

#include <paludis/environment.hh>

#include <QMap>
#include <QObject>
#include <QSet>
#include <QStringList>
#include <QVariant>
#include <paludis/util/tr1_memory.hh>

namespace paludis
{
    class Repository;
}

namespace pertubis
{

    /*! \brief Holds system wide and package wise useflag settings for application use and editing.
    *
    */
    class UseFlagModel : public QObject
    {
        Q_OBJECT

    public:

        typedef QMap<QString,QString> SMap;
        typedef QMap<QString,bool>  BMap;
        typedef QMap<QString,SMap>  MSMap;
        typedef QMap<QString,BMap>  MBMap;
        typedef QMap<QString,MBMap> GMBMap;
        typedef QSet<QString> SSet;

        enum UseSetOrder{ us_set, us_desc};
        enum UseFlagOrder{ uo_flag, uo_user,uo_profile, uo_prog, uo_desc};

        UseFlagModel(QObject* parent,
                     std::tr1::shared_ptr<paludis::Environment> env);

        ~UseFlagModel();

        void addDescription(QString repository,QString ufset, QString useflag, QString desc);
        void addUseflag(QString repository, QString ufset, QString useflag, bool ison);

        bool changeUseflag(QString repository, QString ufset, QString useflag, bool ison);

        QString description(QString repository, QString ufset, QString useflag) const;
        bool useflag(QString repository, QString ufset, QString useflag) const;
        void useflagSetData(QString repository, QString ufset);
        void expandVariables(QString repository, QString ufset, QStringList vars);

        bool hasSet(QString repository, QString ufset);

        void slotEditItemFlags(QString cat,QString pack,QString version,QString repository);

        bool isExpandVariable(QString line) const;

        void loadExpandVariables(QString repository);
        void loadRepoVars(const paludis::Repository& repo);
        void loadProfileUseflags();
        QString fileFinder(QString pathname, QString filename);
        void loadUseFlagDescGlobal(const paludis::Repository& repo);
        void loadUseFlagDescLocal(const paludis::Repository& repo);
        void loadTargetConfig();

        void saveTargetConfig();

    public slots:

        void slotRefresh();

    signals:

        void useflagResult(QList<QVariantList> list);
        void setResult(QStringList list);

    private:

        std::tr1::shared_ptr<paludis::Environment> m_env;

        GMBMap                            m_expandVars;
        MBMap                            m_useflags;
        MSMap                            m_descriptions;
        MSMap                            m_environVariables;
        SSet                            m_environSet;
        SSet                            m_makeDefaults;
        SSet                            m_useDefaults;
        BMap                            m_useflagChanges;
    };
}

#endif
