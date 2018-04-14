#include "UtEntities.h"
#include "UtEntities-odb.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

using namespace std;

/**********************class Ut1vNSharedPktEntity**********************/
TableId Ut1vNSharedPktEntity::GetId() const
{
    return id;
}

void Ut1vNSharedPktEntity::SetId(TableId id)
{
    this->id = id;
}

const list<shared_ptr<Ut1vNWeakFkt1Entity>>& Ut1vNSharedPktEntity::GetUt1vNWeakFkt1s() const
{
    return ut1vNWeakFkt1Ents;
}

list<shared_ptr<Ut1vNWeakFkt1Entity>>& Ut1vNSharedPktEntity::GetUt1vNWeakFkt1s()
{
    return ut1vNWeakFkt1Ents;
}

void Ut1vNSharedPktEntity::Bind(shared_ptr<Ut1vNWeakFkt1Entity> ut1v1WeakFkt1Ent)
{
    MemberHelper::BindMember(this->ut1vNWeakFkt1Ents, ut1v1WeakFkt1Ent, shared_from_this());
}

void Ut1vNSharedPktEntity::Unbind(shared_ptr<Ut1vNWeakFkt1Entity> ut1v1WeakFkt1Ent)
{
    MemberHelper::UnbindMember(this->ut1vNWeakFkt1Ents, ut1v1WeakFkt1Ent, shared_from_this());
}

/* private functions */