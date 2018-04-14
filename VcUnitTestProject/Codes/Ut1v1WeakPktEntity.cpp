#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1v1WeakPktEntity**********************/
TableId Ut1v1WeakPktEntity::GetId() const
{
    return id;
}

void Ut1v1WeakPktEntity::SetId(TableId id)
{
    this->id = id;
}

shared_ptr<Ut1v1SharedFktEntity> Ut1v1WeakPktEntity::GetUt1v1SharedFkt() const
{
    return ut1v1SharedFktEnt.lock();
}

void Ut1v1WeakPktEntity::SetUt1v1SharedFkt(shared_ptr<Ut1v1SharedFktEntity> ut1v1SharedFktEnt)
{
    MemberHelper::SetMember(this->ut1v1SharedFktEnt, ut1v1SharedFktEnt, shared_from_this());
}

/* private functions */
/* Bind, Unbind */
void Ut1v1WeakPktEntity::Bind(weak_ptr<Ut1v1SharedFktEntity> ut1v1SharedFktEnt)
{
    MemberHelper::BindMember(this->ut1v1SharedFktEnt, ut1v1SharedFktEnt, shared_from_this());
}

void Ut1v1WeakPktEntity::Unbind(weak_ptr<Ut1v1SharedFktEntity> ut1v1SharedFktEnt)
{
    MemberHelper::UnbindMember(this->ut1v1SharedFktEnt, ut1v1SharedFktEnt, shared_from_this());
}