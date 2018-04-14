#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1v1SharedPktEntity**********************/
TableId Ut1v1SharedPktEntity::GetId() const
{ return id; }

void Ut1v1SharedPktEntity::SetId(TableId id)
{ this->id = id; }

uint32_t Ut1v1SharedPktEntity::GetBindNumber() const
{ return bindNumber; }

void Ut1v1SharedPktEntity::SetBindNumber(uint32_t bindNumber)
{ this->bindNumber = bindNumber; }

uint32_t Ut1v1SharedPktEntity::GetUnbindNumber() const
{ return unbindNumber; }

void Ut1v1SharedPktEntity::SetUnbindNumber(uint32_t unbindNumber)
{ this->unbindNumber = unbindNumber; }

shared_ptr<Ut1v1WeakFktEntity> Ut1v1SharedPktEntity::GetUt1v1WeakFkt() const
{
	return ut1v1WeakFktEnt;
}

void Ut1v1SharedPktEntity::SetUt1v1WeakFkt(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt)
{
	MemberHelper::SetMember(this->ut1v1WeakFktEnt, ut1v1WeakFktEnt, shared_from_this());
}

/* private functions */
/* Bind, Unbind */
void Ut1v1SharedPktEntity::Bind(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt)
{
	++bindNumber;
	MemberHelper::BindMember(this->ut1v1WeakFktEnt, ut1v1WeakFktEnt, shared_from_this());
}

void Ut1v1SharedPktEntity::Unbind(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt)
{
	++unbindNumber;
	MemberHelper::UnbindMember(this->ut1v1WeakFktEnt, ut1v1WeakFktEnt, shared_from_this());
}