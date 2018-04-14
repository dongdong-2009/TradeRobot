#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1vNWeakPkt1Entity**********************/
TableId Ut1vNWeakPkt1Entity::GetId() const
{
    return id;
}

void Ut1vNWeakPkt1Entity::SetId(TableId id)
{
    this->id = id;
}

const list<weak_ptr<Ut1vNSharedFktEntity>>& Ut1vNWeakPkt1Entity::GetUt1vNSharedFkts() const
{
    return ut1vNSharedFktEnts;
}

list<weak_ptr<Ut1vNSharedFktEntity>>& Ut1vNWeakPkt1Entity::GetUt1vNSharedFkts()
{
    return ut1vNSharedFktEnts;
}

void Ut1vNWeakPkt1Entity::Bind(shared_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt)
{
    MemberHelper::BindMember(this->ut1vNSharedFktEnts, ut1vNSharedFktEnt, shared_from_this());
}

void Ut1vNWeakPkt1Entity::Unbind(shared_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt)
{
    MemberHelper::UnbindMember(this->ut1vNSharedFktEnts, ut1vNSharedFktEnt, shared_from_this());
}

/* private functions */
void Ut1vNWeakPkt1Entity::Bind(weak_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt)
{
    MemberHelper::BindMember(this->ut1vNSharedFktEnts, ut1vNSharedFktEnt, shared_from_this());
}

void Ut1vNWeakPkt1Entity::Unbind(weak_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt)
{
    MemberHelper::UnbindMember(this->ut1vNSharedFktEnts, ut1vNSharedFktEnt, shared_from_this());
}