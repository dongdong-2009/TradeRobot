#ifndef _MemberHelper_h_
#define _MemberHelper_h_

#include <memory>
#include <list>    /* std::list */
#include <vector>  /* std::list */
#include <algorithm> /* remove_if */
#include <functional>

#include <odb/lazy-ptr.hxx>

using std::list;
using std::vector;
using std::set;
using std::shared_ptr;
using std::weak_ptr;

using odb::lazy_shared_ptr;
using odb::lazy_weak_ptr;

class MemberHelper
{
public:
    /******************** member is value ********************/
    /* BindMember */
    template<typename M, typename ThisPtr>
    static void BindMember(shared_ptr<M>& l, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        assert(l == nullptr);
        assert(r != nullptr);
        r->Bind(weak_ptr<typename ThisPtr::element_type>(thisPtr));
        l = r;
    }

    template<typename M, typename ThisPtr>
    static void BindMember(weak_ptr<M>& l, weak_ptr<M>& r, ThisPtr)
    {
		assert(l.expired());
        assert(!r.expired());
        l = r;
    }

	template<typename M, typename ThisPtr>
	static void BindMember(lazy_shared_ptr<M>& l, shared_ptr<M>& r, ThisPtr thisPtr)
	{
        assert(l.loaded() && l.get_eager() == nullptr);
        r->Bind(weak_ptr<typename ThisPtr::element_type>(thisPtr));
        l = r;
	}

    /* UnbindMember */
    template<typename M, typename ThisPtr>
    static void UnbindMember(shared_ptr<M>& l, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        assert((l != nullptr) && !(r == nullptr));
        assert(l->GetId() == r->GetId());
        r->Unbind(weak_ptr<typename ThisPtr::element_type>(thisPtr));
        l.reset();
    }

    template<typename M, typename ThisPtr>
    static void UnbindMember(weak_ptr<M>& l, weak_ptr<M>& r, ThisPtr)
    {
        assert(!l.expired() && !r.expired() && l.lock()->GetId() == r.lock()->GetId());
        l.reset();
    }

    template<typename M, typename ThisPtr>
    static void UnbindMember(lazy_shared_ptr<M>& l, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        /* valid pointer to transient object (unloaded()),
         * or valid pointer to persistent object (unloaded())
         */
		assert(l.get_eager() != nullptr && l.object_id() == r->GetId());
        r->Unbind(weak_ptr<typename ThisPtr::element_type>(thisPtr));
        l.reset();
    }

    /* SetMember, can't be called by odb */
    template<typename M>
    static void SetMember(shared_ptr<M>& l, M r)
    {
        if (l == nullptr)
        {
            l = std::make_shared<M>(r);
        }
        else
        {
            *l = r;
        }
    }

    template<typename M, typename ThisPtr>
    static void SetMember(shared_ptr<M>& l, shared_ptr<M> r, ThisPtr thisPtr)
    {
        if (r == nullptr)
        {
            assert(l != nullptr);
            thisPtr->Unbind(l);
        }
        else
        {
            assert(l == nullptr);
            thisPtr->Bind(r);
        }
    }

    template<typename M, typename ThisPtr>
    static void SetMember(weak_ptr<M>& l, shared_ptr<M> r, ThisPtr thisPtr)
    {
        if (r == nullptr)
        {
            assert(!l.expired());
            l.lock()->Unbind(thisPtr);
        }
        else
        {
            assert(l.expired());
            r->Bind(thisPtr);
        }
    }

    template<typename M, typename ThisPtr>
    static void SetMember(lazy_shared_ptr<M>& l, shared_ptr<M> r, ThisPtr thisPtr)
    {
        if (r == nullptr)
        {
            assert(l.get_eager() != nullptr);
            /* unloaded transient object or loaded persistent object*/
            thisPtr->Unbind(l.get_eager());
        }
        else /* if (r == nullptr) */
        {
            assert(l.get_eager() == nullptr && l.loaded());
            thisPtr->Bind(r);
        } /* else if (r == nullptr) */
    }

    /******************** member is list ********************/
    /* BindMember */
    template <typename M, typename ThisPtr>
    static void BindMember(list<shared_ptr<M>>& l, shared_ptr<M>& r, ThisPtr thisPtr)
    {
#ifdef _DEBUG
		auto cmp = [r](shared_ptr<M> iter)->bool
		{
			return (r->GetId() == iter->GetId());
		};
        assert(find_if(l.begin(), l.end(), cmp) == l.end());
#endif

        assert(r != nullptr);
        r->Bind(weak_ptr<typename ThisPtr::element_type>(thisPtr));
        l.push_back(r);
    }

    /* example: void Ut1vNWeakPkt1Entity::BindUt1vNSharedFkt(shared_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt) */
    template <typename M, typename ThisPtr>
    static void BindMember(list<weak_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        assert(r != nullptr);
        r->Bind(thisPtr);
    }

    template <typename M, typename ThisPtr>
    static void BindMember(list<weak_ptr<M>>& l, weak_ptr<M>& r, ThisPtr thisPtr)
	{
#ifdef _DEBUG
        auto cmp = [r](weak_ptr<M> iter)->bool
        {
            return (r.lock()->GetId() == iter.lock()->GetId());
        };
        assert(find_if(l.begin(), l.end(), cmp) == l.end());
#endif

        assert(!r.expired());
        l.push_back(r);
    }

    template <typename M, typename ThisPtr>
    static void BindMember(list<lazy_shared_ptr<M>>& l, shared_ptr<M>& r, ThisPtr thisPtr)
    {
#ifdef _DEBUG
		auto cmp = [r](lazy_shared_ptr<M> iter)->bool
		{
			return (r->GetId() == iter.object_id());
		};
        assert(find_if(l.begin(), l.end(), cmp) == l.end());
#endif

		assert(r != nullptr);
        r->Bind(weak_ptr<typename ThisPtr::element_type>(thisPtr));
		l.push_back(r);
    }

    /* UnbindMember */
    template <typename M, typename ThisPtr>
    static void UnbindMember(list<shared_ptr<M>>& l, shared_ptr<M>& r, ThisPtr thisPtr)
    {
		auto cmp = [r](shared_ptr<M> iter)->bool
		{
			return (r->GetId() == iter->GetId());
		};

        r->Unbind(weak_ptr<typename ThisPtr::element_type>(thisPtr));

        auto iter = remove_if(l.begin(), l.end(), cmp);
        assert(iter != l.end());
        l.erase(iter);
    }

    /* Example:
    void Ut1vNWeakPkt1Entity::UnbindUt1vNSharedFkt(shared_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt)
    => void Ut1vNWeakPkt1Entity::Unbind(weak_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt)
    */
    template <typename M, typename ThisPtr>
    static void UnbindMember(list<weak_ptr<M>>& l, weak_ptr<M>& r, ThisPtr)
    {
        auto cmp = [r](weak_ptr<M> iter)->bool
        {
            return (r.lock()->GetId() == iter.lock()->GetId());
        };

        auto iter = remove_if(l.begin(), l.end(), cmp);
        assert(iter != l.end());
        l.erase(iter);
    }

    /* example: void Ut1vNWeakPkt1Entity::UnbindUt1vNSharedFkt(shared_ptr<Ut1vNSharedFktEntity> ut1vNSharedFktEnt) */
    template <typename M, typename ThisPtr>
    static void UnbindMember(list<weak_ptr<M>>& l, shared_ptr<M>& r, ThisPtr thisPtr)
    {
#ifdef _DEBUG
        auto cmp = [r](weak_ptr<M> iter)->bool
        {
            return (r->GetId() == iter.lock()->GetId());
        };

        auto iter = find_if(l.begin(), l.end(), cmp);
        assert(iter != l.end());
#endif

        assert(r != nullptr);
        r->Unbind(thisPtr);
    }

    /* Example: void Ut1vNLazySharedPktEntity::UnbindUt1vNWeakFkt2(shared_ptr<Ut1vNWeakFkt2Entity> ut1v1WeakFkt2Ent) */
    template <typename M, typename ThisPtr>
    static void UnbindMember(list<lazy_shared_ptr<M>>& l, shared_ptr<M>& r, ThisPtr thisPtr)
    {
		auto cmp = [r](lazy_shared_ptr<M> iter)->bool
		{
			return (r->GetId() == iter.object_id());
		};

        r->Unbind(weak_ptr<typename ThisPtr::element_type>(thisPtr));
        auto iter = remove_if(l.begin(), l.end(), cmp);
        assert(iter != l.end());
        l.erase(iter);
    }
};

#endif
