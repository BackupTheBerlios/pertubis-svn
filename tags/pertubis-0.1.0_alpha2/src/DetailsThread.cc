
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

namespace
{
    class Displayer :
        public paludis::ConstVisitor<paludis::MetadataKeyVisitorTypes>
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

            void visit(const paludis::MetadataCollectionKey<paludis::IUseFlagSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),paludis::join(k.value()->begin(), k.value()->end(), "<br>"),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataCollectionKey<paludis::UseFlagNameSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),join(k.value()->begin(), k.value()->end(), "<br>"),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataCollectionKey<paludis::Set<std::string> > & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(pertubis::make_row(k.human_name(),paludis::stringify(k.pretty_print_flat(formatter)),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataCollectionKey<paludis::KeywordNameSet> & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),join(k.value()->begin(), k.value()->end(), "<br>"),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataCollectionKey<paludis::FSEntrySequence> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(pertubis::make_row(k.human_name() + ":",k.pretty_print_flat(formatter),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::DependencySpecTree> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),k.pretty_print(formatter),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::FetchableURISpecTree> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(
                            pertubis::make_row( paludis::stringify(k.human_name()),
                            k.pretty_print(formatter) ,std::string("bgcolor=\"#ddddff\"")));                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::SimpleURISpecTree> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(
                        pertubis::make_row( paludis::stringify(k.human_name()),
                                            pertubis::link(k.pretty_print_flat(formatter),k.pretty_print_flat(formatter)),std::string("bgcolor=\"#ddddff\"") ));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::LicenseSpecTree> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),k.pretty_print_flat(formatter),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::ProvideSpecTree> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()).c_str(),k.pretty_print_flat(formatter),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSpecTreeKey<paludis::RestrictSpecTree> & k)
            {
                if (k.type() == type)
                {
                    pertubis::HtmlFormatter formatter;
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),k.pretty_print_flat(formatter),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataCollectionKey<paludis::PackageIDSequence> & k)
            {
                pertubis::HtmlFormatter formatter;
                thread->appendOutput(pertubis::make_row(k.human_name(),k.pretty_print_flat(formatter),std::string("bgcolor=\"#ddddff\"")));
            }

            void visit(const paludis::MetadataPackageIDKey & k)
            {
                if (k.type() == type)
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),paludis::stringify(*k.value()),std::string("bgcolor=\"#ddddff\"")));
            }

            void visit(const paludis::MetadataStringKey & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(pertubis::make_row(paludis::stringify(k.human_name()),paludis::stringify(k.value()),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataSectionKey & k)
            {
                if (k.type() == type)
                {
                    thread->appendOutput(pertubis::make_row(k.human_name() + ":","",std::string("bgcolor=\"#ddddff\"")));

                    Displayer v(thread, env, id, type);
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
                    thread->appendOutput(pertubis::make_row(std::string("time"),std::string(buf),std::string("bgcolor=\"#ddddff\"")));
                }
            }

            void visit(const paludis::MetadataRepositoryMaskInfoKey & k)
            {
                if (k.type() == type)
                    thread->appendOutput(pertubis::make_row(k.human_name(),paludis::stringify(k.value()->mask_file),std::string("bgcolor=\"#ddddff\"")));
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
                                        const paludis::tr1::shared_ptr<paludis::Environment>&  env,
                                        TaskBox* box) : ThreadBase(pobj,env,box)
{
}

void pertubis::DetailsThread::start( paludis::tr1::shared_ptr<const paludis::PackageID> id)
{
    m_id = id;
    QThread::start();
}

void pertubis::DetailsThread::run()
{
    m_text = QString("<html><body><table border=\"0\" summary=\"\" width=\"100%\" height=\"100%\" cellpadding=\"25\">\
    <colgroup><col width=\"30%\"><col width=\"70%\"></colgroup><tr><th bgcolor=\"#5a3aca\"></th><th bgcolor=\"#000000\" align=\"left\"><font color=\"#ffffff\">%1-%2</font></th>\n</tr><tbody>\n")
            .arg(stringify(m_id->name()).c_str()).arg(stringify(m_id->version()).c_str());

    Displayer ds(this,m_env.get(),m_id,paludis::mkt_significant);
    Displayer dn(this,m_env.get(),m_id,paludis::mkt_normal);
    Displayer dp(this,m_env.get(),m_id,paludis::mkt_dependencies);
    std::for_each(indirect_iterator(m_id->begin_metadata()), indirect_iterator(m_id->end_metadata()), accept_visitor(ds));
    std::for_each(indirect_iterator(m_id->begin_metadata()), indirect_iterator(m_id->end_metadata()), accept_visitor(dn));
    std::for_each(indirect_iterator(m_id->begin_metadata()), indirect_iterator(m_id->end_metadata()), accept_visitor(dp));

    m_text.append("</tbody></table></body></html>\n");
    qDebug() << m_text;
    emit sendResult(m_text);
}
