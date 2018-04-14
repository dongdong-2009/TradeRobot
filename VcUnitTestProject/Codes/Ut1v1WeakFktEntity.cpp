#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1v1WeakFktEntity**********************/
TableId Ut1v1WeakFktEntity::GetId() const
{ return id; }

void Ut1v1WeakFktEntity::SetId(TableId id)
{ this->id = id; }

uint32_t Ut1v1WeakFktEntity::GetBindNumber() const
{ return bindNumber; }

void Ut1v1WeakFktEntity::SetBindNumber(uint32_t bindNumber)
{ this->bindNumber = bindNumber; }

uint32_t Ut1v1WeakFktEntity::GetUnbindNumber() const
{ return unbindNumber; }

void Ut1v1WeakFktEntity::SetUnbindNumber(uint32_t unbindNumber)
{ this->unbindNumber = unbindNumber; }

shared_ptr<Ut1v1SharedPktEntity> Ut1v1WeakFktEntity::GetUt1v1SharedPkt() const
{
	return ut1v1SharedPktEnt.lock();
}

void Ut1v1WeakFktEntity::SetUt1v1SharedPkt(shared_ptr<Ut1v1SharedPktEntity> ut1v1SharedPktEnt)
{
	MemberHelper::SetMember(this->ut1v1SharedPktEnt, ut1v1SharedPktEnt, shared_from_this());
}

shared_ptr<Ut1v1LazySharedPktEntity> Ut1v1WeakFktEntity::GetUt1v1LazySharedPkt() const
{
	return ut1v1LazySharedPktEnt.lock();
}

void Ut1v1WeakFktEntity::SetUt1v1LazySharedPkt(shared_ptr<Ut1v1LazySharedPktEntity> ut1v1LazySharedPktEnt)
{
	MemberHelper::SetMember(this->ut1v1LazySharedPktEnt, ut1v1LazySharedPktEnt, shared_from_this());
}

/* private functions */
/* Bind, Unbind */
void Ut1v1WeakFktEntity::Bind(weak_ptr<Ut1v1SharedPktEntity> ut1v1SharedPktEnt)
{
	++bindNumber;
	MemberHelper::BindMember(this->ut1v1SharedPktEnt, ut1v1SharedPktEnt, shared_from_this());
}

void Ut1v1WeakFktEntity::Unbind(weak_ptr<Ut1v1SharedPktEntity> ut1v1SharedPktEnt)
{
	++unbindNumber;
	MemberHelper::UnbindMember(this->ut1v1SharedPktEnt, ut1v1SharedPktEnt, shared_from_this());
}

void Ut1v1WeakFktEntity::Bind(weak_ptr<Ut1v1LazySharedPktEntity> ut1v1LazySharedPktEnt)
{
	++bindNumber;
	MemberHelper::BindMember(this->ut1v1LazySharedPktEnt, ut1v1LazySharedPktEnt, shared_from_this());
}

void Ut1v1WeakFktEntity::Unbind(weak_ptr<Ut1v1LazySharedPktEntity> ut1v1LazySharedPktEnt)
{
	++unbindNumber;
	MemberHelper::UnbindMember(this->ut1v1LazySharedPktEnt, ut1v1LazySharedPktEnt, shared_from_this());
}