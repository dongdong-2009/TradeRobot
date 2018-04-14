#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1vNWeakFkt1Entity**********************/
TableId Ut1vNWeakFkt1Entity::GetId() const
{
    return id;
}

void Ut1vNWeakFkt1Entity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<Ut1vNSharedPktEntity> Ut1vNWeakFkt1Entity::GetUt1vNSharedPkt() const
{
    return ut1vNSharedPktEnt.lock();
}

void Ut1vNWeakFkt1Entity::SetUt1vNSharedPkt(shared_ptr<Ut1vNSharedPktEntity> ut1vNSharedPktEnt)
{
    MemberHelper::SetMember(this->ut1vNSharedPktEnt, ut1vNSharedPktEnt, shared_from_this());
}

/* private functions */
/* Bind, Unbind */
void Ut1vNWeakFkt1Entity::Bind(weak_ptr<Ut1vNSharedPktEntity> ut1vNSharedPktEnt)
{
    MemberHelper::BindMember(this->ut1vNSharedPktEnt, ut1vNSharedPktEnt, shared_from_this());
}

void Ut1vNWeakFkt1Entity::Unbind(weak_ptr<Ut1vNSharedPktEntity> ut1vNSharedPktEnt)
{
    MemberHelper::UnbindMember(this->ut1vNSharedPktEnt, ut1vNSharedPktEnt, shared_from_this());
}