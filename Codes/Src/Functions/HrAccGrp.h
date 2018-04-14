#ifndef _HrAccGrp_h_
#define _HrAccGrp_h_

#include "Include/Functions/RobotInterface.h"

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

/* Functions */
#include "Include/Functions/HrAccGrpInterface.h"

#define ThdEventExit   1
#define ThdEventMarket 2

/**********************class HrAccGrp**********************/
class HrAccGrp : public HrAccGrpInterface
{
public:
    HrAccGrp(shared_ptr<HrAccountGroupEntity> hrAccGrpEnt);
    ~HrAccGrp();

    void Start();
    void Stop();
    bool IsRunning() const;
    shared_ptr<HrAccountGroupEntity> GetHrAccGrpEnt() const;
    void HandleMarket(shared_ptr<MarketEntity> marketEnt);

private:
    void TheAccGrpThreadMain(bool *ready);

private:
    mutable std::mutex mtx;
    std::condition_variable cv;
    bool running;
    uint32_t thdEvent;

    std::shared_ptr<HrAccountGroupEntity> hrAccGrpEnt;
    std::shared_ptr<HrAccountEntity> hrAccEnt0;
    std::shared_ptr<HrAccountEntity> hrAccEnt1;
    std::shared_ptr<PlatformEntity> ptmEnt0;
    std::shared_ptr<PlatformEntity> ptmEnt1;

    std::pair<std::shared_ptr<MarketEntity>, std::shared_ptr<MarketEntity>> marketEntPair;

    /* thread relative variable */
    std::thread theAccGrpThreadMain;
};

#endif