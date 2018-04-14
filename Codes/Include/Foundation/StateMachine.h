#ifndef _StateMachine_h_
#define _StateMachine_h_

#include <map>
#include <memory>   //shared_ptr
#include <utility>  //make_pair

/*  reference material; B31-full.pdf */
template<typename E>   //E: Event
class EventSinkInterface
{
public:
    virtual void CastEvent(E e, const char *func, int line) = 0;
};

template<typename A, typename E>  //A: Automaton Interface, E: Event
class StateBase
{
public:
    StateBase(A& automaton): automaton(automaton)
    {}

protected:
    /* func: caller's function name. 
     * line: caller's line number.
     * example: CastEvent(e, __func__, __LINE__)
     */
    void CastEvent(E e, const char *func, int line)
    {
        automaton.CastEvent(e, func, line);
    }

protected:
    A& automaton; /* context */
};

template<typename A, typename E>  //A: Automaton Interface, E: Event
class AutomatonBase : public EventSinkInterface<E>
{
public:
    AutomatonBase()
    {}
    virtual ~AutomatonBase()
    {}

    void AddEdge(std::shared_ptr<A> src, E e, std::shared_ptr<A> dst)
    {
        auto iter = edges.find(src);
        if (iter == edges.end())
        {
             edges.insert(std::make_pair(src, std::map<E, std::shared_ptr<A>>{ { e, dst } }));
        }
        else
        {
            iter->second.insert(make_pair(e, dst));
        }
    }

    virtual void CastEvent(E e, const char *, int)
    {
        state = edges[state][e];
    }

protected:
    std::map<std::shared_ptr<A>, std::map<E, std::shared_ptr<A>>> edges;
    std::shared_ptr<A> state;
};

#endif