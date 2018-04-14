#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1vNWeakFkt2Entity**********************/
TableId Ut1vNWeakFkt2Entity::GetId() const
{
    return id;
}

void Ut1vNWeakFkt2Entity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<Ut1vNLazySharedPktEntity> Ut1vNWeakFkt2Entity::GetUt1vNLazySharedPkt() const
{
    return ut1vNSharedPktEnt.lock();
}

void Ut1vNWeakFkt2Entity::SetUt1vNLazySharedPkt(shared_ptr<Ut1vNLazySharedPktEntity> ut1vNSharedPktEnt)
{
    MemberHelper::SetMember(this->ut1vNSharedPktEnt, ut1vNSharedPktEnt, shared_from_this());
}

/* private functions */
/* Bind, Unbind */
void Ut1vNWeakFkt2Entity::Bind(weak_ptr<Ut1vNLazySharedPktEntity> ut1vNSharedPktEnt)
{
    MemberHelper::BindMember(this->ut1vNSharedPktEnt, ut1vNSharedPktEnt, shared_from_this());
}

void Ut1vNWeakFkt2Entity::Unbind(weak_ptr<Ut1vNLazySharedPktEntity> ut1vNSharedPktEnt)
{
    MemberHelper::UnbindMember(this->ut1vNSharedPktEnt, ut1vNSharedPktEnt, shared_from_this());
}