#ifndef _HrAccGrpInterface_h_
#define _HrAccGrpInterface_h_

/* Entity */
#include "Include/Entity/Entities.h"

/**********************class HrAccGrpInterface**********************/
class HrAccGrpInterface
{
public:
    HrAccGrpInterface() {}
    virtual ~HrAccGrpInterface() {}
    static HrAccGrpInterface* CreateInstance(shared_ptr<HrAccountGroupEntity> hrAccEnt);

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    virtual shared_ptr<HrAccountGroupEntity> GetHrAccGrpEnt() const = 0;
    virtual void HandleMarket(shared_ptr<MarketEntity> market) = 0;
};

namespace std
{
    inline shared_ptr<HrAccGrpInterface> make_shared(shared_ptr<HrAccountGroupEntity> hrAccEnt)
    {
        return shared_ptr<HrAccGrpInterface>(HrAccGrpInterface::CreateInstance(hrAccEnt));
    }
};

#endif