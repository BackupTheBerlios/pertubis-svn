#include "Item.hh"
#include <QDebug>
#include <paludis/package_id.hh>



pertubis::Item::Item()  : m_parent(0), m_bestChild(0), m_state(Item::is_stable) {}

pertubis::Item::Item(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                    const QList<QVariant> &dats,
                    ItemState mystate)  :
                    m_id(id),
                    m_data(dats),
                    m_parent(0),
                    m_bestChild(0),
                    m_state(mystate)
{
}

pertubis::Item::Item(const QList<QVariant> &dats,
                    ItemState mystate)  :
                    m_data(dats),
                    m_parent(0),
                    m_bestChild(0),
                    m_state(mystate)
{
}

pertubis::Item::~Item()
{
    m_parent=0;
    qDeleteAll(m_children);
    m_children.clear();
}

pertubis::Item* pertubis::Item::child(int rindex) const
{
    return m_children.value(rindex);
}

int pertubis::Item::childCount() const
{
    return m_children.count();
}

int pertubis::Item::columnCount() const
{
    return m_data.count();
}

QVariant pertubis::Item::data(int column) const
{
    return m_data.value(column);
}

pertubis::Item* pertubis::Item::parent() const
{
    return m_parent;
}

int pertubis::Item::indexOf(Item* item) const
{
    return m_children.indexOf(item);
}

bool pertubis::Item::available() const
{
    return true;
}

pertubis::Item::ItemState pertubis::Item::state() const
{
    return m_state;
}

void pertubis::Item::appendChild(Item *item)
{
    m_children.append(item);
    item->setParent(this);
}

int pertubis::Item::row() const
{
    if (m_parent)
        return m_parent->indexOf( const_cast< Item* >(this) );
    return 0;
}

void pertubis::Item::setBestChild(Item *item)
{
    m_bestChild = item;
}

void pertubis::Item::setData(int column, QVariant dats)
{
    if (column < m_data.count() )
    {
        m_data.replace(column, dats);
    }
}

QList<pertubis::Item*>::iterator pertubis::Item::childBegin()
{
    return m_children.begin();
}

QList<pertubis::Item*>::iterator pertubis::Item::childEnd()
{
    return m_children.end();
}

void pertubis::Item::setParent(Item* pitem)
{
    m_parent=pitem;
}

void pertubis::Item::setState(ItemState s)
{
     m_state = s;
}

void pertubis::Item::setTaskState(int taskid, Qt::CheckState mystate)
{
    QVariantList states = data(io_selected).toList();
    states[taskid] = mystate;
    setData(io_selected,states);
}

paludis::tr1::shared_ptr<const paludis::PackageID> pertubis::Item::ID()
{
    return paludis::tr1::shared_ptr<const paludis::PackageID>(static_cast<const paludis::PackageID*>(0) );
}

pertubis::Item::UpdateRange pertubis::RootItem::updateRange() const
{
    return ur_nothing;
}

pertubis::Item* pertubis::Item::bestChild() const
{
    return m_bestChild;
}
