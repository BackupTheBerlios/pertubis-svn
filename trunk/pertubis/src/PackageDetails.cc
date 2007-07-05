
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

#include "PackageDetails.hh"
#include "PackageItem.hh"

#include <paludis/environment.hh>
#include <paludis/package_database.hh>
#include <paludis/query.hh>
#include <paludis/repositories/gentoo/ebuild.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>

#include <QString>


pertubis::PackageDetails::PackageDetails(std::tr1::shared_ptr<paludis::Environment> env, QObject* parent) : QThread(parent), m_env(env)
{
}

void pertubis::PackageDetails::search(QString cn,QString pn,QString ver,QString rep,int status)
{
	m_pn=pn;
	m_cn=cn;
	m_ver=ver;
	m_rep=rep;
	m_status=status;
	run();
}

void pertubis::PackageDetails::run()
{
// 	qDebug() << "PackageDetails::run() - start";
	using namespace paludis;
QString tpl(
"<html>"
"<body bgcolor=\"#aaaaaa\" text=\"black\">\n"
"	<table border=\"0\" summary=\"\" width=\"100%\" height=\"100%\" cellpadding=\"15\">\n"
"			<colgroup>\n"
"				<col width=\"50%\">\n"
"				<col width=\"50%\">\n"
"			</colgroup>\n"
"			<tr>\n"
"   			<th colspan=\"2\" align=\"left\">%1-%2 ( %3 ) </th>\n"
"			</tr>\n"
"			<tbody >\n"
"				<tr>\n"
"					<td>Description</td>\n"
"					<td>%4</td>\n"
"				</tr>\n"
"				<tr>\n"
"					<td>HomePage</td>\n"
"					<td><a href=\"%5\">%6</a></td>\n"
"				</tr>\n"
"				<tr>\n"
"					<td>Use Flags</td>\n"
"					<td>%7</td>\n"
"				</tr>\n"
"				<tr>\n"
"					<td>Keywords</td>\n"
"					<td>%8</td>\n"
"				</tr>\n"
"				<tr>\n"
"					<td>Provides</td>\n"
"					<td>%9</td>\n"
"				</tr>\n"
"				<tr>\n"
"					<td>License</td>\n"
"					<td>%10</td>\n"
"				</tr>\n"
"			</tbody>\n"
"		</table>\n"
" </body>\n"
"</html>\n");

	CategoryNamePart c(m_cn.toLatin1().data());
	PackageNamePart p(m_pn.toLatin1().data());

	RepositoryName ins("installed");
	RepositoryName n(m_rep.toLatin1().data());
// 	qDebug() << "PackageDetails::run() - 1";
	QString output;
	if (m_env->package_database()->has_repository_named(n) && m_env->package_database()->has_repository_named(ins))
	{
		std::tr1::shared_ptr< Repository > rep = m_env->package_database()->fetch_repository(n);
		QualifiedPackageName qpn(c,p);
		std::tr1::shared_ptr< const VersionMetadata > data = rep->version_metadata(qpn, VersionSpec(m_ver.toLatin1().data() ));
		output = tpl.arg(stringify(qpn).c_str() ).arg(m_ver).arg(status(m_status)).arg(data->description.c_str()).arg(data->homepage.c_str()).arg(data->homepage.c_str()).arg(stringify(data->ebuild_interface->iuse).c_str()).arg(data->ebuild_interface->keywords_string.c_str()).arg(data->ebuild_interface->provide_string.c_str()).arg("");
	}
// 	qDebug() << "PackageDetails::run() - 2";
	emit sendDetails(output);
}
