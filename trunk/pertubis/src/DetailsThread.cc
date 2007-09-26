
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


#include "DetailsThread.hh"

#include "DatabaseView.hh"
#include "FormatterUtils.hh"
#include "HtmlFormatter.hh"

#include <paludis/dep_spec.hh>
#include <paludis/environment.hh>
#include <paludis/mask.hh>
#include <paludis/metadata_key.hh>
#include <paludis/package_database.hh>
#include <paludis/package_id.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/set.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/visitor-impl.hh>

#include <QDebug>

namespace
{
    class Displayer : public paludis::ConstVisitor<paludis::MetadataKeyVisitorTypes>
    {
        private:
            pertubis::DetailsThread* thread;
            const paludis::Environment * const env;
            const paludis::tr1::shared_ptr<const paludis::PackageID> id;
            const paludis::MetadataKeyType type;

        public:
            Displayer(pertubis::DetailsThread *  t,
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
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),paludis::join(k.value()->begin(), k.value()->end(), "<br>")));
                }
            }

            void visit(const paludis::MetadataSetKey<paludis::UseFlagNameSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),join(k.value()->begin(), k.value()->end(), "<br>")));
                }
            }

            void visit(const paludis::MetadataSetKey<paludis::Set<std::string> > & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(pertubis::make_row(k.human_name(),paludis::stringify(k.pretty_print_flat(formatter))));
                }
            }

            void visit(const paludis::MetadataSetKey<paludis::KeywordNameSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),join(k.value()->begin(), k.value()->end(), "<br>")));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::DependencySpecTree> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),k.pretty_print(formatter)));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::URISpecTree> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(
                        pertubis::make_row( paludis::stringify(k.human_name()),
                                            pertubis::link(k.pretty_print_flat(formatter),k.pretty_print(formatter)) ));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::LicenseSpecTree> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),k.pretty_print(formatter)));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::ProvideSpecTree> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()).c_str(),k.pretty_print(formatter)));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::RestrictSpecTree> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),k.pretty_print(formatter).c_str()));
                }
            }

            void visit(const paludis::MetadataSetKey<paludis::PackageIDSequence> & k)
            {
                pertubis::HtmlFormatter formatter;
                thread->appendOutput(pertubis::make_row(k.human_name(),k.pretty_print_flat(formatter)));
            }

            void visit(const paludis::MetadataPackageIDKey & k)
            {
                if (k.type() == type)
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),paludis::stringify(*k.value())));
            }

            void visit(const paludis::MetadataStringKey & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),paludis::stringify(k.value())));
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
                    thread->appendOutput(pertubis::make_row(std::string("time"),std::string(buf)));
                }
            }

            void visit(const paludis::MetadataRepositoryMaskInfoKey & k)
            {
                if (k.type() == type)
                    thread->appendOutput(pertubis::make_row(k.human_name(),paludis::stringify(k.value()->mask_file)));
            }

            void visit(const paludis::MetadataContentsKey &)
            {
            }

            void visit(const paludis::MetadataFSEntryKey &)
            {
            }
    };
}

pertubis::DetailsThread::DetailsThread(QObject* pobj,
        DatabaseView* main) : ThreadBase(pobj,main)
{
}

void pertubis::DetailsThread::appendOutput(std::string text)
{
    m_output.append(QString::fromStdString(text));
}

void pertubis::DetailsThread::search( paludis::tr1::shared_ptr<const paludis::PackageID> id)
{
    m_id = id;
    start();
}

void pertubis::DetailsThread::run()
{
    qDebug() << "PackageDetails::fetchDetails() - start";

    m_output = QString(
"<html>\n"
"<body bgcolor=\"#aaaaaa\" text=\"black\">\n"
"    <table border=\"0\" summary=\"\" width=\"100%\" height=\"100%\" cellpadding=\"15\">\n"
"            <colgroup>\n"
"                <col width=\"30%\">\n"
"                <col width=\"70%\">\n"
"            </colgroup>\n"
"            <tr>\n"
"               <th colspan=\"2\" align=\"left\">%1-%2</th>\n"
"            </tr>\n"
"            <tbody >\n").arg(stringify(m_id->name()).c_str()).arg(stringify(m_id->version()).c_str());

    Displayer ds(this,m_main->getEnv().get(),m_id,paludis::mkt_significant);
    Displayer dn(this,m_main->getEnv().get(),m_id,paludis::mkt_normal);
    Displayer dp(this,m_main->getEnv().get(),m_id,paludis::mkt_dependencies);
    std::for_each(indirect_iterator(m_id->begin_metadata()), indirect_iterator(m_id->end_metadata()), accept_visitor(ds));
    std::for_each(indirect_iterator(m_id->begin_metadata()), indirect_iterator(m_id->end_metadata()), accept_visitor(dn));
    std::for_each(indirect_iterator(m_id->begin_metadata()), indirect_iterator(m_id->end_metadata()), accept_visitor(dp));

    m_output.append(QString(
"            </tbody>\n"
"        </table>\n"
" </body>\n"
"</html>\n"));
//     qDebug() << m_output;
    emit detailsResult(m_output);
    qDebug() << "PackageDetails::fetchDetails() - done";
}
