#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1vNLazySharedFktEntity**********************/
TableId Ut1vNLazySharedFktEntity::GetId() const
{
    return id;
}

void Ut1vNLazySharedFktEntity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<Ut1vNWeakPkt2Entity> Ut1vNLazySharedFktEntity::GetUt1vNWeakPkt2(bool loadFirst) const
{
    if (loadFirst && ut1vNWeakPkt2Ent.get_eager() == nullptr && !ut1vNWeakPkt2Ent.loaded())
    {
        return ut1vNWeakPkt2Ent.load();
    }

    return ut1vNWeakPkt2Ent.get_eager();
}

void Ut1vNLazySharedFktEntity::SetUt1vNWeakPkt2(shared_ptr<Ut1vNWeakPkt2Entity> ut1vNWeakPkt2Ent)
{
    MemberHelper::SetMember(this->ut1vNWeakPkt2Ent, ut1vNWeakPkt2Ent, shared_from_this());
}

/* private functions */
/* Bind, Unbind */
void Ut1vNLazySharedFktEntity::Bind(shared_ptr<Ut1vNWeakPkt2Entity> ut1vNWeakPkt2Ent)
{
    MemberHelper::BindMember(this->ut1vNWeakPkt2Ent, ut1vNWeakPkt2Ent, shared_from_this());
}

void Ut1vNLazySharedFktEntity::Unbind(shared_ptr<Ut1vNWeakPkt2Entity> ut1vNWeakPkt2Ent)
{
    MemberHelper::UnbindMember(this->ut1vNWeakPkt2Ent, ut1vNWeakPkt2Ent, shared_from_this());
}