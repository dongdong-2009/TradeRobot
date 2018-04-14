#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1vNLazySharedPktEntity**********************/
TableId Ut1vNLazySharedPktEntity::GetId() const
{
    return id;
}

void Ut1vNLazySharedPktEntity::SetId(TableId id)
{
    this->id = id;
}

const list<lazy_shared_ptr<Ut1vNWeakFkt2Entity>>& Ut1vNLazySharedPktEntity::GetUt1vNWeakFkt2s() const
{
    return ut1vNWeakFkt2Ents;
}

list<lazy_shared_ptr<Ut1vNWeakFkt2Entity>>& Ut1vNLazySharedPktEntity::GetUt1vNWeakFkt2s()
{
    return ut1vNWeakFkt2Ents;
}

void Ut1vNLazySharedPktEntity::Bind(shared_ptr<Ut1vNWeakFkt2Entity> ut1v1WeakFkt2Ent)
{
    MemberHelper::BindMember(this->ut1vNWeakFkt2Ents, ut1v1WeakFkt2Ent, shared_from_this());
}

void Ut1vNLazySharedPktEntity::Unbind(shared_ptr<Ut1vNWeakFkt2Entity> ut1v1WeakFkt2Ent)
{
    MemberHelper::UnbindMember(this->ut1vNWeakFkt2Ents, ut1v1WeakFkt2Ent, shared_from_this());
}

/* private functions */