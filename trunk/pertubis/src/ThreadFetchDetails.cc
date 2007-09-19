
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

#include "ThreadFetchDetails.hh"
#include "DatabaseView.hh"

#include <paludis/environments/environment_maker.hh>
#include <paludis/environment.hh>
#include <paludis/package_database.hh>
#include <paludis/dep_spec.hh>
#include <paludis/query.hh>
#include <QDebug>
#include <paludis/package_id.hh>
// #include <paludis/repositories/gentoo/ebuild.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/set.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/visitor-impl.hh>
#include <paludis/metadata_key.hh>

#include "pcre_matcher.hh"
#include "text_matcher.hh"
#include "description_extractor.hh"
#include "name_extractor.hh"

#include <set>

namespace pertubis
{
    class Displayer :
        public paludis::ConstVisitor<paludis::MetadataKeyVisitorTypes>
    {
        private:
            ThreadFetchDetails* thread;
            const paludis::Environment * const env;
            const paludis::tr1::shared_ptr<const paludis::PackageID> id;
            const paludis::MetadataKeyType type;

        public:
            Displayer(ThreadFetchDetails *  t,
                    const paludis::Environment * const e,
                    const paludis::tr1::shared_ptr<const paludis::PackageID> & i,
                    const paludis::MetadataKeyType k) :
                thread(t),
                env(e),
                id(i),
                type(k)
            {
            }

            void visit(const paludis::MetadataSetKey<paludis::IUseFlagSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(paludis::join(k.value()->begin(), k.value()->end(), "<br>").c_str()));
                }
            }

            void visit(const paludis::MetadataSetKey<paludis::InheritedSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(paludis::join(k.value()->begin(), k.value()->end(), "<br>").c_str()));
                }
            }

            void visit(const paludis::MetadataSetKey<paludis::UseFlagNameSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(paludis::join(k.value()->begin(), k.value()->end(), "<br>").c_str()));
                }
            }

            void visit(const paludis::MetadataSetKey<paludis::KeywordNameSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(paludis::join(k.value()->begin(), k.value()->end(), "<br>").c_str()));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::DependencySpecTree> & k)
            {
                if (k.type() == type)
                {
                    // TODO: use paludis::formatters for pretty printing
                    thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(k.pretty_print().c_str()));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::URISpecTree> & k)
            {
                if (k.type() == type)
                {
                    if (paludis::stringify(k.human_name()) == "Homepage")
                    {
                        thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td><a href=\"%2\">%3</a></td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(k.pretty_print_flat().c_str()).arg(k.pretty_print().c_str()));
                    }
                    else
                    {
                        thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(k.pretty_print().c_str()));
                    }
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::LicenseSpecTree> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(k.pretty_print().c_str()));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::ProvideSpecTree> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(k.pretty_print().c_str()));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::RestrictSpecTree> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(k.pretty_print().c_str()));
                }
            }

            void visit(const paludis::MetadataSetKey<paludis::PackageIDSequence> & k)
            {
    //             if (k.type() == type)
    //             {
    //                 if (k.value()->empty() || next(k.value()->begin()) == k.value()->end())
    //                     task->display_metadata_pde(k.human_name(), k.raw_name(), **k.value()->begin());
    //                 else if (task->want_raw())
    //                 {
    //                     task->output_left_column(k.raw_name() + ":");
    //                     task->output_right_column(join(indirect_iterator(k.value()->begin()), indirect_iterator(k.value()->end()), ", "));
    //                 }
    //                 else
    //                 {
    //                     for (PackageIDSequence::Iterator i(k.value()->begin()), i_end(k.value()->end()) ;
    //                             i != i_end ; ++i)
    //                     {
    //                         if (i == k.value()->begin())
    //                             task->output_left_column(k.human_name() + ":");
    //                         else
    //                             task->output_left_column("");
    //
    //                         task->output_right_column(task->render_as_package_name(stringify(**i)));
    //                     }
    //                 }
    //             }
            }
    //
            void visit(const paludis::MetadataPackageIDKey & k)
            {
                if (k.type() == type)
                    thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(paludis::stringify(*k.value()).c_str()));
            }

            void visit(const paludis::MetadataStringKey & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(paludis::stringify(k.value()).c_str()));
                }
            }

            void visit(const paludis::MetadataTimeKey & k)
            {
                time_t t = k.value();
                if (k.type() != type)
                    return;
                if (0 == t)
                    return;
                char buf[255];
                if (strftime(buf, 254, "%c", gmtime(&t)))
                {
                    thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg("time").arg(buf));
                }
            }

            void visit(const paludis::MetadataRepositoryMaskInfoKey & k)
            {
//                 if (k.type() == type)
//                     thread->appendOutput(QString("<tr>\n<td>%1</td>\n<td>%2</td>\n</tr>\n").arg(paludis::stringify(k.human_name()).c_str()).arg(paludis::stringify(k.value()).c_str()));
            }

            void visit(const paludis::MetadataContentsKey &)
            {
            }

            void visit(const paludis::MetadataFSEntryKey &)
            {
            }
    };
}

pertubis::ThreadFetchDetails::ThreadFetchDetails(QObject* pobj,
        DatabaseView* main) : ThreadBase(pobj,main)
{
}

void pertubis::ThreadFetchDetails::appendOutput(QString text)
{
    m_output.append(text);
}

void pertubis::ThreadFetchDetails::search(const paludis::tr1::shared_ptr<const paludis::PackageID>& id)
{
    m_id = id;
    start();
}

void pertubis::ThreadFetchDetails::run()
{
    qDebug() << "PackageDetails::fetchDetails() - start";
    using namespace paludis;
    m_output = QString(
"<html>\n"
"<body bgcolor=\"#aaaaaa\" text=\"black\">\n"
"    <table border=\"0\" summary=\"\" width=\"100%\" height=\"100%\" cellpadding=\"15\">\n"
"            <colgroup>\n"
"                <col width=\"50%\">\n"
"                <col width=\"50%\">\n"
"            </colgroup>\n"
"            <tr>\n"
"               <th colspan=\"2\" align=\"left\">%1-%2</th>\n"
"            </tr>\n"
            "            <tbody >\n").arg(paludis::stringify(m_id->name()).c_str()).arg(paludis::stringify(m_id->version()).c_str());

    Displayer ds(this,m_main->getEnv().get(),m_id,paludis::mkt_significant);
    Displayer dn(this,m_main->getEnv().get(),m_id,paludis::mkt_normal);
    Displayer dp(this,m_main->getEnv().get(),m_id,paludis::mkt_dependencies);
    std::for_each(paludis::indirect_iterator(m_id->begin_metadata()), paludis::indirect_iterator(m_id->end_metadata()), paludis::accept_visitor(ds));
    std::for_each(paludis::indirect_iterator(m_id->begin_metadata()), paludis::indirect_iterator(m_id->end_metadata()), paludis::accept_visitor(dn));
    std::for_each(paludis::indirect_iterator(m_id->begin_metadata()), paludis::indirect_iterator(m_id->end_metadata()), paludis::accept_visitor(dp));

    m_output.append(QString(
"            </tbody>\n"
"        </table>\n"
" </body>\n"
"</html>\n"));
    emit detailsResult(m_output);
    qDebug() << "PackageDetails::fetchDetails() - done";
}
