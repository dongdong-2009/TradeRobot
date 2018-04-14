#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1vNWeakPkt2Entity**********************/
TableId Ut1vNWeakPkt2Entity::GetId() const
{
    return id;
}

void Ut1vNWeakPkt2Entity::SetId(TableId id)
{
    this->id = id;
}

const list<weak_ptr<Ut1vNLazySharedFktEntity>>& Ut1vNWeakPkt2Entity::GetUt1vNLazySharedFkts() const
{ 
    return ut1vNLazySharedFktEnts;
}

list<weak_ptr<Ut1vNLazySharedFktEntity>>& Ut1vNWeakPkt2Entity::GetUt1vNLazySharedFkts()
{
    return ut1vNLazySharedFktEnts;
}

void Ut1vNWeakPkt2Entity::BindUt1vNLazySharedFkt(shared_ptr<Ut1vNLazySharedFktEntity> ut1vNLazySharedFktEnt)
{
    MemberHelper::BindMember(this->ut1vNLazySharedFktEnts, ut1vNLazySharedFktEnt, shared_from_this());
}

void Ut1vNWeakPkt2Entity::UnbindUt1vNLazySharedFkt(shared_ptr<Ut1vNLazySharedFktEntity> ut1vNLazySharedFktEnt)
{
    MemberHelper::UnbindMember(this->ut1vNLazySharedFktEnts, ut1vNLazySharedFktEnt, shared_from_this());
}

/* private functions */
/* Bind, Unbind */
void Ut1vNWeakPkt2Entity::Bind(weak_ptr<Ut1vNLazySharedFktEntity> ut1vNLazySharedFktEnt)
{
    MemberHelper::BindMember(this->ut1vNLazySharedFktEnts, ut1vNLazySharedFktEnt, shared_from_this());
}

void Ut1vNWeakPkt2Entity::Unbind(weak_ptr<Ut1vNLazySharedFktEntity> ut1vNLazySharedFktEnt)
{
    MemberHelper::UnbindMember(this->ut1vNLazySharedFktEnts, ut1vNLazySharedFktEnt, shared_from_this());
}
