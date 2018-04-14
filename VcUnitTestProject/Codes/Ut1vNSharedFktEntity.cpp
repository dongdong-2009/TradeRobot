#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1vNSharedFktEntity**********************/
TableId Ut1vNSharedFktEntity::GetId() const
{
    return id;
}

void Ut1vNSharedFktEntity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<Ut1vNWeakPkt1Entity> Ut1vNSharedFktEntity::GetUt1vNWeakPkt1() const
{
    return ut1vNWeakPkt1Ent;
}

void Ut1vNSharedFktEntity::SetUt1vNWeakPkt1(shared_ptr<Ut1vNWeakPkt1Entity> ut1vNWeakPkt1Ent)
{
    MemberHelper::SetMember(this->ut1vNWeakPkt1Ent, ut1vNWeakPkt1Ent, shared_from_this());
}

/* private functions */
/* Bind, Unbind */
void Ut1vNSharedFktEntity::Bind(shared_ptr<Ut1vNWeakPkt1Entity> ut1vNWeakPkt1Ent)
{
    MemberHelper::BindMember(this->ut1vNWeakPkt1Ent, ut1vNWeakPkt1Ent, shared_from_this());
}

void Ut1vNSharedFktEntity::Unbind(shared_ptr<Ut1vNWeakPkt1Entity> ut1vNWeakPkt1Ent)
{
    MemberHelper::UnbindMember(this->ut1vNWeakPkt1Ent, ut1vNWeakPkt1Ent, shared_from_this());
}