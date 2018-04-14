#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1v1SharedFktEntity**********************/
TableId Ut1v1SharedFktEntity::GetId() const
{
    return id;
}

void Ut1v1SharedFktEntity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<Ut1v1WeakPktEntity> Ut1v1SharedFktEntity::GetUt1v1WeakPkt() const
{
    return ut1v1WeakPktEnt;
}

void Ut1v1SharedFktEntity::SetUt1v1WeakPkt(shared_ptr<Ut1v1WeakPktEntity> ut1v1WeakPktEnt)
{
    MemberHelper::SetMember(this->ut1v1WeakPktEnt, ut1v1WeakPktEnt, shared_from_this());
}

/* private functions */
/* Bind, Unbind */
void Ut1v1SharedFktEntity::Bind(shared_ptr<Ut1v1WeakPktEntity> ut1v1WeakPktEnt)
{
    MemberHelper::BindMember(this->ut1v1WeakPktEnt, ut1v1WeakPktEnt, shared_from_this());
}

void Ut1v1SharedFktEntity::Unbind(shared_ptr<Ut1v1WeakPktEntity> ut1v1WeakPktEnt)
{
    MemberHelper::UnbindMember(this->ut1v1WeakPktEnt, ut1v1WeakPktEnt, shared_from_this());
}