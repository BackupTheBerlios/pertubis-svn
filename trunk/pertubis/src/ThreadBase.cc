
#include "ThreadBase.hh"

pertubis::ThreadBase::ThreadBase(QObject* pobj,
                                DatabaseView* main) : QThread(pobj), m_main(main)
{
}
