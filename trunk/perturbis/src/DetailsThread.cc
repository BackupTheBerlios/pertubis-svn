
/* Copyright (C) 2007-2008 Stefan Koegl
*
* This file is part of perturbis
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


#include "DetailsThread.hh"
#include "FormatterUtils.hh"
#include "HtmlFormatter.hh"
#include <paludis/environment.hh>
#include <paludis/mask.hh>
#include <paludis/metadata_key.hh>
#include <paludis/package_database.hh>
#include <paludis/package_id.hh>
#include <paludis/util/set.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/visitor-impl.hh>

#include <QDebug>

using namespace perturbis;

namespace
{
    class Displayer :
        public paludis::ConstVisitor<paludis::MetadataKeyVisitorTypes>
    {
        private:
            const paludis::tr1::shared_ptr<const paludis::PackageID> id;
            DetailsThread* thread;
            const paludis::Environment * const env;
            const paludis::MetadataKeyType type;

        public:
            Displayer(const paludis::tr1::shared_ptr<const paludis::PackageID> & i,
                    DetailsThread *  t,
                    const paludis::Environment * const e,
                    const paludis::MetadataKeyType k) :
                    id(i),
                    thread(t),
                    env(e),
                    type(k)
            {
            }

            void visit(const paludis::MetadataCollectionKey<paludis::IUseFlagSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(make_row(paludis::stringify(k.human_name()),paludis::join(k.value()->begin(), k.value()->end(), "<br>"),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataCollectionKey<paludis::UseFlagNameSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(make_row(paludis::stringify(k.human_name()),join(k.value()->begin(), k.value()->end(), "<br>"),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataCollectionKey<paludis::Set<std::string> > & k)
            {
                if (k.type() == type)
                {
                    HtmlFormatter formatter;
                    thread->appendOutput(make_row(k.human_name(),paludis::stringify(k.pretty_print_flat(formatter)),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataCollectionKey<paludis::KeywordNameSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(make_row(paludis::stringify(k.human_name()),join(k.value()->begin(), k.value()->end(), "<br>"),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataCollectionKey<paludis::FSEntrySequence> & k)
            {
                if (k.type() == type)
                {
                    HtmlFormatter formatter;
                    thread->appendOutput(make_row(k.human_name() + ":",k.pretty_print_flat(formatter),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::DependencySpecTree> & k)
            {
                if (k.type() == type)
                {
                    HtmlFormatter formatter;
                    thread->appendOutput(make_row(paludis::stringify(k.human_name()),k.pretty_print(formatter),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::FetchableURISpecTree> & k)
            {
                if (k.type() == type)
                {
                    HtmlFormatter formatter;
                    thread->appendOutput(
                            make_row(paludis::stringify(k.human_name()),
                            k.pretty_print(formatter) ,std::string("bgcolor=\"#ddddff\"")));                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::SimpleURISpecTree> & k)
            {
                if (k.type() == type)
                {
                    HtmlFormatter formatter;
                    thread->appendOutput(
                        make_row(paludis::stringify(k.human_name()),
                                            link(k.pretty_print_flat(formatter),k.pretty_print_flat(formatter)),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::LicenseSpecTree> & k)
            {
                if (k.type() == type)
                {
                    HtmlFormatter formatter;
                    thread->appendOutput(make_row(paludis::stringify(k.human_name()),k.pretty_print_flat(formatter),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::ProvideSpecTree> & k)
            {
                if (k.type() == type)
                {
                    HtmlFormatter formatter;
                    thread->appendOutput(make_row(paludis::stringify(k.human_name()),k.pretty_print_flat(formatter),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSizeKey & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(make_row(paludis::stringify(k.human_name()),paludis::stringify(k.pretty_print()),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::RestrictSpecTree> & k)
            {
                if (k.type() == type)
                {
                    HtmlFormatter formatter;
                    thread->appendOutput(make_row(paludis::stringify(k.human_name()),k.pretty_print_flat(formatter),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataCollectionKey<paludis::PackageIDSequence> & k)
            {
                if (k.type() == type)
                {
                    HtmlFormatter formatter;
                    thread->appendOutput(make_row(k.human_name(),k.pretty_print_flat(formatter),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataPackageIDKey & k)
            {
                if (k.type() == type)
                    thread->appendOutput(make_row(paludis::stringify(k.human_name()),paludis::stringify(*k.value()),std::string("bgcolor=\"#ddddff\"")));
            }

            void visit(const paludis::MetadataStringKey & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(make_row(paludis::stringify(k.human_name()),paludis::stringify(k.value()),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSectionKey & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(make_row(k.human_name() + ":","",std::string("bgcolor=\"#ddddff\"")));

                    Displayer v(id, thread, env, type);
                    std::for_each(paludis::indirect_iterator(k.begin_metadata()), paludis::indirect_iterator(k.end_metadata()),
                                  paludis::accept_visitor(v));
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
                    thread->appendOutput(make_row(std::string("time"),std::string(buf),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataRepositoryMaskInfoKey &)
            {
//                 if (k.type() == type)
//                     thread->appendOutput(make_row(k.human_name(),paludis::stringify(k.value()->mask_file),std::string("bgcolor=\"#ddddff\"")));
            }

            void visit(const paludis::MetadataContentsKey &)
            {
            }

            void visit(const paludis::MetadataFSEntryKey &)
            {
//                 if (k.type() == type)
//                     thread->appendOutput(make_row(k.human_name(),paludis::stringify(k.value()),std::string("bgcolor=\"#ddddff\"")));
            }
    };
}

DetailsThread::DetailsThread(QObject* pobj,
                                        const paludis::tr1::shared_ptr<paludis::Environment>&  myenv) : ThreadBase(pobj,myenv)
{
}

DetailsThread::~DetailsThread()
{
    qDebug() << "DetailsThread::~DetailsThread()";
}

void
DetailsThread::setup(paludis::tr1::shared_ptr<const paludis::PackageID> id)
{
    m_id = id;
}

void
DetailsThread::run()
{
    m_text = QString("<html><body><table border=\"0\" summary=\"\" width=\"100%\" height=\"100%\" cellpadding=\"10\">\
    <colgroup><col width=\"30%\"><col width=\"70%\"></colgroup><tr><th bgcolor=\"#5a3aca\"></th><th bgcolor=\"#000000\" align=\"left\"><font color=\"#ffffff\">%1-%2</font></th>\n</tr><tbody>\n")
            .arg(QString::fromStdString(stringify(m_id->name()))).arg(QString::fromStdString(stringify(m_id->version())));

    Displayer ds(m_id,this,env().get(),paludis::mkt_significant);
    Displayer dn(m_id,this,env().get(),paludis::mkt_normal);
    Displayer di(m_id,this,env().get(),paludis::mkt_internal);
    Displayer da(m_id,this,env().get(),paludis::mkt_author);
    Displayer dp(m_id,this,env().get(),paludis::mkt_dependencies);
    std::for_each(indirect_iterator(m_id->begin_metadata()), indirect_iterator(m_id->end_metadata()), accept_visitor(ds));
    std::for_each(indirect_iterator(m_id->begin_metadata()), indirect_iterator(m_id->end_metadata()), accept_visitor(dn));
    std::for_each(indirect_iterator(m_id->begin_metadata()), indirect_iterator(m_id->end_metadata()), accept_visitor(di));
    std::for_each(indirect_iterator(m_id->begin_metadata()), indirect_iterator(m_id->end_metadata()), accept_visitor(da));
    std::for_each(indirect_iterator(m_id->begin_metadata()), indirect_iterator(m_id->end_metadata()), accept_visitor(dp));

    m_text.append("</tbody></table></body></html>\n");
//     qDebug() << m_text;
    emit sendResult(m_text);
}
