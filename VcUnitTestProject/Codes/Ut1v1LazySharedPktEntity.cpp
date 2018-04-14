#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1v1LazySharedPktEntity**********************/
TableId Ut1v1LazySharedPktEntity::GetId() const
{ return id; }

void Ut1v1LazySharedPktEntity::SetId(TableId id)
{ this->id = id; }

uint32_t Ut1v1LazySharedPktEntity::GetBindNumber() const
{ return bindNumber; }

void Ut1v1LazySharedPktEntity::SetBindNumber(uint32_t bindNumber)
{ this->bindNumber = bindNumber; }

uint32_t Ut1v1LazySharedPktEntity::GetUnbindNumber() const
{ return unbindNumber; }

void Ut1v1LazySharedPktEntity::SetUnbindNumber(uint32_t unbindNumber)
{ this->unbindNumber = unbindNumber; }

shared_ptr<Ut1v1WeakFktEntity> Ut1v1LazySharedPktEntity::GetUt1v1WeakFkt(bool loadFirst) const
{
    if (loadFirst && ut1v1WeakFktEnt.get_eager() == nullptr && !ut1v1WeakFktEnt.loaded())
    {
        return ut1v1WeakFktEnt.load();
    }

	return ut1v1WeakFktEnt.get_eager();
}

void Ut1v1LazySharedPktEntity::SetUt1v1WeakFkt(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt)
{
	MemberHelper::SetMember(this->ut1v1WeakFktEnt, ut1v1WeakFktEnt, shared_from_this());
}

/* private functions */
/* Bind, Unbind */
void Ut1v1LazySharedPktEntity::Bind(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt)
{
	++bindNumber;
	MemberHelper::BindMember(this->ut1v1WeakFktEnt, ut1v1WeakFktEnt, shared_from_this());
}

void Ut1v1LazySharedPktEntity::Unbind(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt)
{
	assert(this->ut1v1WeakFktEnt != nullptr);
	++unbindNumber;
    MemberHelper::UnbindMember(this->ut1v1WeakFktEnt, ut1v1WeakFktEnt, shared_from_this());
}
