
#include "ThreadBase.hh"
#include <paludis/environment.hh>

pertubis::ThreadBase::ThreadBase(QObject* pobj, paludis::tr1::shared_ptr<paludis::Environment> env) : QThread(pobj), m_env(env)
{
}
