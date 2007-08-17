
#include "ThreadBase.hh"
#include <paludis/environment.hh>

pertubis::ThreadBase::ThreadBase(QObject* parent, std::tr1::shared_ptr<paludis::Environment> env) : QThread(parent), m_env(env)
{
}
