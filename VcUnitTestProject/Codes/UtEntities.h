#ifndef _UtEntities_h_
#define _UtEntities_h_

/* C++ */
#include <string>
#include <memory>  /* shared_ptr, weak_ptr, enable_shared_from_this */
#include <list>    /* std::list */
#include <vector>  /* std::list */
#include <chrono>  /* time_point, duration, seconds, ... */

/* ODB */
#include <odb/lazy-ptr.hxx>

/* Base Type */
#include "Include/Type/BaseType.h"  /* TableId */

using std::list;
using std::vector;
using std::set;
using std::shared_ptr;
using std::weak_ptr;

using odb::lazy_shared_ptr;
using odb::lazy_weak_ptr;

#pragma db namespace() pointer(shared_ptr)

/*
Pkt: Primary Key Table, table which hold the primary key in a specific relationship
Fkt: Foreign Key Table, table which hold the foreign key in a specific relationship
*/

class Ut1v1SharedPktEntity;
class Ut1v1LazySharedPktEntity;
class Ut1v1WeakFktEntity;
/**********************class Ut1v1SharedPktEntity**********************/
#pragma db object table("Ut1v1SharedPkt")
class Ut1v1SharedPktEntity : public std::enable_shared_from_this<Ut1v1SharedPktEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

	Ut1v1SharedPktEntity(TableId id, uint32_t bindNumber, uint32_t unbindNumber)
		: id(id), bindNumber(bindNumber), unbindNumber(unbindNumber)
    {}

    Ut1v1SharedPktEntity() : Ut1v1SharedPktEntity(InvalidTableId, 0, 0)
    {}

    ~Ut1v1SharedPktEntity(){}

	TableId GetId() const;
	void SetId(TableId id);

	uint32_t GetBindNumber() const;
	void SetBindNumber(uint32_t bindNumber);

	uint32_t GetUnbindNumber() const;
	void SetUnbindNumber(uint32_t unbindNumber);

	shared_ptr<Ut1v1WeakFktEntity> GetUt1v1WeakFkt() const;
	void SetUt1v1WeakFkt(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt);

private:
    /* disable default copy constructor */
    Ut1v1SharedPktEntity(const Ut1v1SharedPktEntity& right);

    /* Bind, Unbind */
	void Bind(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt);
	void Unbind(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt);

#pragma db id column("Id")
    TableId id;

#pragma db not_null column("BindNumber")
    uint32_t bindNumber;

#pragma db not_null column("UnbindNumber")
	uint32_t unbindNumber;

#pragma db inverse(ut1v1SharedPktEnt)
    shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt;
};

/**********************class Ut1v1LazySharedPktEntity**********************/
/*
	实际使用 Ut1v1LazySharedPktEntity 时, 因按照如下流程设置ut1v1WeakEnt成员, 并save之前
	的 ut1v1WeakEnt 成员保存到数据库.
	SetUt1v1WeakFkt = ut1v1LazySharedPktEnt->GetUt1v1WeakFkt();
	ut1v1LazySharedPktEnt->SetUt1v1WeakFkt(nullptr);
	if (ut1v1LazySharedPktEnt->GetUt1v1WeakFkt().loaded())
	db->update(ut1v1WeakEnt1);
	else
	db->persist(ut1v1WeakEnt1);
	ut1v1WeakEnt = make_shared<Ut1v1WeakFktEntity>(1, 0, 0);
	ut1v1LazySharedPktEnt->SetUt1v1WeakFkt(ut1v1WeakEnt);
	db->persist(ut1v1SharedPktEnt);
	db->persist(ut1v1WeakEnt1);
*/
#pragma db object table("Ut1v1LazySharedPkt")
class Ut1v1LazySharedPktEntity : public std::enable_shared_from_this<Ut1v1LazySharedPktEntity>
{
public:
	friend class odb::access;
	friend class MemberHelper;

	Ut1v1LazySharedPktEntity(TableId id, uint32_t bindNumber, uint32_t unbindNumber)
		: id(id), bindNumber(bindNumber), unbindNumber(unbindNumber),
		ut1v1WeakFktEnt()
	{}

	Ut1v1LazySharedPktEntity() : Ut1v1LazySharedPktEntity(InvalidTableId, 0, 0)
	{}

	~Ut1v1LazySharedPktEntity(){}

	TableId GetId() const;
	void SetId(TableId id);

	uint32_t GetBindNumber() const;
	void SetBindNumber(uint32_t bindNumber);

	uint32_t GetUnbindNumber() const;
	void SetUnbindNumber(uint32_t unbindNumber);

    shared_ptr<Ut1v1WeakFktEntity> GetUt1v1WeakFkt(bool loadFirst) const;
    void SetUt1v1WeakFkt(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt);

private:
	/* disable default copy constructor */
	Ut1v1LazySharedPktEntity(const Ut1v1LazySharedPktEntity& right);

	/* Bind, Unbind */
    void Bind(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt);
    void Unbind(shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt);

#pragma db id column("Id")
	TableId id;

#pragma db not_null column("BindNumber")
	uint32_t bindNumber;

#pragma db not_null column("UnbindNumber")
	uint32_t unbindNumber;

#pragma db inverse(ut1v1LazySharedPktEnt)
	lazy_shared_ptr<Ut1v1WeakFktEntity> ut1v1WeakFktEnt;
};

/**********************class Ut1v1WeakFktEntity**********************/
#pragma db object table("Ut1v1WeakFkt")
class Ut1v1WeakFktEntity : public std::enable_shared_from_this<Ut1v1WeakFktEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

	Ut1v1WeakFktEntity(TableId id, uint32_t bindNumber, uint32_t unbindNumber)
		: id(id), bindNumber(bindNumber), unbindNumber(unbindNumber),
		ut1v1SharedPktEnt(), ut1v1LazySharedPktEnt()
	{}

	Ut1v1WeakFktEntity() : Ut1v1WeakFktEntity(InvalidTableId, 0, 0)
	{}

    ~Ut1v1WeakFktEntity(){}

	TableId GetId() const;
	void SetId(TableId id);

	uint32_t GetBindNumber() const;
	void SetBindNumber(uint32_t bindNumber);

	uint32_t GetUnbindNumber() const;
	void SetUnbindNumber(uint32_t unbindNumber);

    shared_ptr<Ut1v1SharedPktEntity> GetUt1v1SharedPkt() const;
    void SetUt1v1SharedPkt(shared_ptr<Ut1v1SharedPktEntity> ut1v1SharedPktEnt);

    shared_ptr<Ut1v1LazySharedPktEntity> GetUt1v1LazySharedPkt() const;
	void SetUt1v1LazySharedPkt(shared_ptr<Ut1v1LazySharedPktEntity> ut1v1LazySharedPktEnt);

private:
    /* disable default copy constructor */
    Ut1v1WeakFktEntity(const Ut1v1WeakFktEntity& right);

    /* Bind, Unbind */
	void Bind(weak_ptr<Ut1v1SharedPktEntity> ut1v1SharedPktEnt);
	void Unbind(weak_ptr<Ut1v1SharedPktEntity> ut1v1SharedPktEnt);

	void Bind(weak_ptr<Ut1v1LazySharedPktEntity> ut1v1LazySharedPktEnt);
	void Unbind(weak_ptr<Ut1v1LazySharedPktEntity> ut1v1LazySharedPktEnt);

#pragma db id column("Id")
	TableId id;

#pragma db not_null column("BindNumber")
	uint32_t bindNumber;

#pragma db not_null column("UnbindNumber")
	uint32_t unbindNumber;

#pragma db not_null column("Ut1v1SharedPktId")
	weak_ptr<Ut1v1SharedPktEntity> ut1v1SharedPktEnt;

#pragma db not_null column("Ut1v1LazySharedPktId")
	weak_ptr<Ut1v1LazySharedPktEntity> ut1v1LazySharedPktEnt;
};

class Ut1v1SharedFktEntity;
class Ut1v1WeakPktEntity;
/**********************class Ut1v1SharedPktEntity**********************/
#pragma db object table("Ut1v1SharedFkt")
class Ut1v1SharedFktEntity : public std::enable_shared_from_this<Ut1v1SharedFktEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    Ut1v1SharedFktEntity(TableId id)
        : id(id), ut1v1WeakPktEnt()
    {}

    Ut1v1SharedFktEntity() : Ut1v1SharedFktEntity(InvalidTableId)
    {}

    ~Ut1v1SharedFktEntity(){}

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<Ut1v1WeakPktEntity> GetUt1v1WeakPkt() const;
    void SetUt1v1WeakPkt(shared_ptr<Ut1v1WeakPktEntity> ut1v1WeakPktEnt);

private:
    /* disable default copy constructor */
    Ut1v1SharedFktEntity(const Ut1v1SharedFktEntity& right);

    /* Bind, Unbind */
    void Bind(shared_ptr<Ut1v1WeakPktEntity> ut1v1WeakPktEnt);
    void Unbind(shared_ptr<Ut1v1WeakPktEntity> ut1v1WeakPktEnt);

#pragma db id column("Id")
    TableId id;

#pragma db not_null column("Ut1v1WeakPktId")
    shared_ptr<Ut1v1WeakPktEntity> ut1v1WeakPktEnt;
};

/**********************class Ut1v1WeakPktEntity**********************/
#pragma db object table("Ut1v1WeakPkt")
class Ut1v1WeakPktEntity : public std::enable_shared_from_this<Ut1v1WeakPktEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    Ut1v1WeakPktEntity(TableId id)
        : id(id), ut1v1SharedFktEnt()
    {}

    Ut1v1WeakPktEntity() : Ut1v1WeakPktEntity(InvalidTableId)
    {}

    ~Ut1v1WeakPktEntity(){}

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<Ut1v1SharedFktEntity> GetUt1v1SharedFkt() const;
    void SetUt1v1SharedFkt(shared_ptr<Ut1v1SharedFktEntity> ut1v1SharedFktEnt);

private:
    /* disable default copy constructor */
    Ut1v1WeakPktEntity(const Ut1v1WeakPktEntity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<Ut1v1SharedFktEntity> ut1v1SharedFktEnt);
    void Unbind(weak_ptr<Ut1v1SharedFktEntity> ut1v1SharedFktEnt);

#pragma db id column("Id")
    TableId id;

#pragma db inverse(ut1v1WeakPktEnt)
    weak_ptr<Ut1v1SharedFktEntity> ut1v1SharedFktEnt;
};

class Ut1vNSharedPktEntity;
class Ut1vNWeakFkt1Entity;
/**********************class Ut1vNSharedPktEntity**********************/
#pragma db object table("Ut1vNSharedPkt")
class Ut1vNSharedPktEntity : public std::enable_shared_from_this<Ut1vNSharedPktEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    Ut1vNSharedPktEntity(TableId id)
        : id(id)
    {}

    Ut1vNSharedPktEntity() : Ut1vNSharedPktEntity(InvalidTableId)
    {}

    ~Ut1vNSharedPktEntity(){}

    TableId GetId() const;
    void SetId(TableId id);

    const list<shared_ptr<Ut1vNWeakFkt1Entity>>& GetUt1vNWeakFkt1s() const;
    list<shared_ptr<Ut1vNWeakFkt1Entity>>& GetUt1vNWeakFkt1s();
    void Bind(shared_ptr<Ut1vNWeakFkt1Entity> ut1v1WeakFkt1Ent);
    void Unbind(shared_ptr<Ut1vNWeakFkt1Entity> ut1v1WeakFkt1Ent);

private:
    /* disable default copy constructor */
    Ut1vNSharedPktEntity(const Ut1vNSharedPktEntity& right);

#pragma db id column("Id")
    TableId id;

#pragma db inverse(ut1vNSharedPktEnt)
    list<shared_ptr<Ut1vNWeakFkt1Entity>> ut1vNWeakFkt1Ents;
};

/**********************class Ut1vNWeakFkt1Entity**********************/
#pragma db object table("Ut1vNWeakFkt1")
class Ut1vNWeakFkt1Entity : public std::enable_shared_from_this<Ut1vNWeakFkt1Entity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    Ut1vNWeakFkt1Entity(TableId id)
        : id(id), ut1vNSharedPktEnt()
    {}

    Ut1vNWeakFkt1Entity() : Ut1vNWeakFkt1Entity(InvalidTableId)
    {}

    ~Ut1vNWeakFkt1Entity(){}

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<Ut1vNSharedPktEntity> GetUt1vNSharedPkt() const;
    void SetUt1vNSharedPkt(shared_ptr<Ut1vNSharedPktEntity> ut1vNSharedPktEnt);

private:
    /* disable default copy constructor */
    Ut1vNWeakFkt1Entity(const Ut1vNWeakFkt1Entity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<Ut1vNSharedPktEntity> ut1vNSharedPktEnt);
    void Unbind(weak_ptr<Ut1vNSharedPktEntity> ut1vNSharedPktEnt);

#pragma db id column("Id")
    TableId id;

#pragma db not_null column("Ut1vNSharedPktId")
    weak_ptr<Ut1vNSharedPktEntity> ut1vNSharedPktEnt;
};

class Ut1vNLazySharedPktEntity;
class Ut1vNWeakFkt2Entity;
/**********************class Ut1vNLazySharedPktEntity**********************/
#pragma db object table("Ut1vNLazySharedPkt")
class Ut1vNLazySharedPktEntity : public std::enable_shared_from_this<Ut1vNLazySharedPktEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    Ut1vNLazySharedPktEntity(TableId id)
        : id(id)
    {}

    Ut1vNLazySharedPktEntity() : Ut1vNLazySharedPktEntity(InvalidTableId)
    {}

    ~Ut1vNLazySharedPktEntity(){}

    TableId GetId() const;
    void SetId(TableId id);

    const list<lazy_shared_ptr<Ut1vNWeakFkt2Entity>>& GetUt1vNWeakFkt2s() const;
    list<lazy_shared_ptr<Ut1vNWeakFkt2Entity>>& GetUt1vNWeakFkt2s();
    void Bind(shared_ptr<Ut1vNWeakFkt2Entity> ut1v1WeakFkt2Ent);
    void Unbind(shared_ptr<Ut1vNWeakFkt2Entity> ut1v1WeakFkt2Ent);

private:
    /* disable default copy constructor */
    Ut1vNLazySharedPktEntity(const Ut1vNLazySharedPktEntity& right);

#pragma db id column("Id")
    TableId id;

#pragma db inverse(ut1vNSharedPktEnt)
    list<lazy_shared_ptr<Ut1vNWeakFkt2Entity>> ut1vNWeakFkt2Ents;
};

/**********************class Ut1vNWeakFkt2Entity**********************/
#pragma db object table("Ut1vNWeakFkt2")
class Ut1vNWeakFkt2Entity : public std::enable_shared_from_this<Ut1vNWeakFkt2Entity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    Ut1vNWeakFkt2Entity(TableId id)
        : id(id), ut1vNSharedPktEnt()
    {}

    Ut1vNWeakFkt2Entity() : Ut1vNWeakFkt2Entity(InvalidTableId)
    {}

    ~Ut1vNWeakFkt2Entity(){}

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<Ut1vNLazySharedPktEntity> GetUt1vNLazySharedPkt() const;
    void SetUt1vNLazySharedPkt(shared_ptr<Ut1vNLazySharedPktEntity> ut1vNSharedPktEnt);

private:
    /* disable default copy constructor */
    Ut1vNWeakFkt2Entity(const Ut1vNWeakFkt2Entity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<Ut1vNLazySharedPktEntity> ut1vNSharedPktEnt);
    void Unbind(weak_ptr<Ut1vNLazySharedPktEntity> ut1vNSharedPktEnt);

#pragma db id column("Id")
    TableId id;

#pragma db not_null column("Ut1vNLazySharedPktId")
    weak_ptr<Ut1vNLazySharedPktEntity> ut1vNSharedPktEnt;
};

class Ut1vNWeakPkt1Entity;
class Ut1vNSharedFktEntity;
/**********************class Ut1vNSharedPktEntity**********************/
#pragma db object table("Ut1vNWeakPkt1")
class Ut1vNWeakPkt1Entity : public std::enable_shared_from_this<Ut1vNWeakPkt1Entity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    Ut1vNWeakPkt1Entity(TableId id)
        : id(id)
    {}

    Ut1vNWeakPkt1Entity() : Ut1vNWeakPkt1Entity(InvalidTableId)
    {}

    ~Ut1vNWeakPkt1Entity(){}

    TableId GetId() const;
    void SetId(TableId id);

    const list<weak_ptr<Ut1vNSharedFktEntity>>& GetUt1vNSharedFkts() const;
    list<weak_ptr<Ut1vNSharedFktEntity>>& GetUt1vNSharedFkts();
    void Bind(shared_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt);
    void Unbind(shared_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt);

private:
    /* disable default copy constructor */
    Ut1vNWeakPkt1Entity(const Ut1vNWeakPkt1Entity& right);

    void Bind(weak_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt);
    void Unbind(weak_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt);

#pragma db id column("Id")
    TableId id;

#pragma db inverse(ut1vNWeakPkt1Ent)
    list<weak_ptr<Ut1vNSharedFktEntity>> ut1vNSharedFktEnts;
};

/**********************class Ut1vNSharedFktEntity**********************/
#pragma db object table("Ut1vNSharedFkt")
class Ut1vNSharedFktEntity : public std::enable_shared_from_this<Ut1vNSharedFktEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    Ut1vNSharedFktEntity(TableId id)
        : id(id)
    {}

    Ut1vNSharedFktEntity() : Ut1vNSharedFktEntity(InvalidTableId)
    {}

    ~Ut1vNSharedFktEntity(){}

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<Ut1vNWeakPkt1Entity> GetUt1vNWeakPkt1() const;
    void SetUt1vNWeakPkt1(shared_ptr<Ut1vNWeakPkt1Entity> ut1vNWeakPkt1Ent);

private:
    /* disable default copy constructor */
    Ut1vNSharedFktEntity(const Ut1vNSharedFktEntity& right);

    /* Bind, Unbind */
    void Bind(shared_ptr<Ut1vNWeakPkt1Entity> ut1vNWeakPkt1Ent);
    void Unbind(shared_ptr<Ut1vNWeakPkt1Entity> ut1vNWeakPkt1Ent);

#pragma db id column("Id")
    TableId id;

#pragma db not_null column("Ut1vNWeakPkt1Id")
    shared_ptr<Ut1vNWeakPkt1Entity> ut1vNWeakPkt1Ent;
};

class Ut1vNWeakPkt2Entity;
class Ut1vNLazySharedFktEntity;
/**********************class Ut1vNWeakPkt2Entity**********************/
#pragma db object table("Ut1vNWeakPkt2")
class Ut1vNWeakPkt2Entity : public std::enable_shared_from_this<Ut1vNWeakPkt2Entity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    Ut1vNWeakPkt2Entity(TableId id)
        : id(id)
    {}

    Ut1vNWeakPkt2Entity() : Ut1vNWeakPkt2Entity(InvalidTableId)
    {}

    ~Ut1vNWeakPkt2Entity(){}

    TableId GetId() const;
    void SetId(TableId id);

    const list<weak_ptr<Ut1vNLazySharedFktEntity>>& GetUt1vNLazySharedFkts() const;
    list<weak_ptr<Ut1vNLazySharedFktEntity>>& GetUt1vNLazySharedFkts();
    void BindUt1vNLazySharedFkt(shared_ptr<Ut1vNLazySharedFktEntity> ut1vNLazySharedFktEnt);
    void UnbindUt1vNLazySharedFkt(shared_ptr<Ut1vNLazySharedFktEntity> ut1vNLazySharedFktEnt);

private:
    /* disable default copy constructor */
    Ut1vNWeakPkt2Entity(const Ut1vNWeakPkt2Entity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<Ut1vNLazySharedFktEntity> ut1vNLazySharedFktEnt);
    void Unbind(weak_ptr<Ut1vNLazySharedFktEntity> ut1vNLazySharedFktEnt);

#pragma db id column("Id")
    TableId id;

#pragma db inverse(ut1vNWeakPkt2Ent)
    list<weak_ptr<Ut1vNLazySharedFktEntity>> ut1vNLazySharedFktEnts;
};

/**********************class Ut1vNLazySharedFktEntity**********************/
#pragma db object table("Ut1vNLazySharedFkt")
class Ut1vNLazySharedFktEntity : public std::enable_shared_from_this<Ut1vNLazySharedFktEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    Ut1vNLazySharedFktEntity(TableId id)
        : id(id)
    {}

    Ut1vNLazySharedFktEntity() : Ut1vNLazySharedFktEntity(InvalidTableId)
    {}

    ~Ut1vNLazySharedFktEntity(){}

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<Ut1vNWeakPkt2Entity> GetUt1vNWeakPkt2(bool loadFirst) const;
    void SetUt1vNWeakPkt2(shared_ptr<Ut1vNWeakPkt2Entity> ut1vNWeakPkt2Ent);

private:
    /* disable default copy constructor */
    Ut1vNLazySharedFktEntity(const Ut1vNLazySharedFktEntity& right);

    /* Bind, Unbind */
    void Bind(shared_ptr<Ut1vNWeakPkt2Entity> ut1vNWeakPkt2Ent);
    void Unbind(shared_ptr<Ut1vNWeakPkt2Entity> ut1vNWeakPkt2Ent);

#pragma db id column("Id")
    TableId id;

#pragma db not_null column("Ut1vNWeakPkt1Id")
    lazy_shared_ptr<Ut1vNWeakPkt2Entity> ut1vNWeakPkt2Ent;
};

#endif