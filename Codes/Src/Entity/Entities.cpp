#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

/* C++ */
#include <algorithm> /* find_if() */
#include <cassert>   /* assert() */
#include <ctime>     /* time_t time (time_t* timer); */
#include <functional>
#include <iostream>

#include <odb/lazy-ptr.hxx>  /* lazy_shared_ptr, lazy_weak_ptr */

/* Base Type */
#include "Include/Type/BaseType.h"

/* Entity */
#include "Include/Entity/MemberHelper.h"

#if defined(_WIN32) && defined(_MSC_VER)
#	ifdef _DEBUG
#	    pragma comment(lib, "odb-mysql-vc12-d.lib")
#	    pragma comment(lib, "odb-vc12-d.lib")
#   else
#	    pragma comment(lib, "odb-mysql-vc12.lib")
#	    pragma comment(lib, "odb-vc12.lib")
#   endif
#endif

using namespace std;
using namespace placeholders;

namespace std
{
    inline std::shared_ptr<int64_t> make_shared(const time_point& v)
    {
        return make_shared<int64_t>(chrono::duration_cast<seconds>(v.time_since_epoch()).count());
    }

    inline std::shared_ptr<int64_t> make_shared(const seconds& v)
    {
        return make_shared<int64_t>(v.count());
    }

    inline std::shared_ptr<int64_t> make_shared(const milliseconds& v)
    {
        return make_shared<int64_t>(v.count());
    }

    inline std::shared_ptr<double> make_shared(double v)
    {
        return make_shared<double>(v);
    }

    inline std::shared_ptr<double> make_shared(Rate v)
    {
        return make_shared<double>((double)v);
    }

    inline std::shared_ptr<double> make_shared(Money v)
    {
        return make_shared<double>((double)v);
    }

    inline std::shared_ptr<double> make_shared(CoinNumber v)
    {
        return make_shared<double>((double)v);
    }

    inline std::shared_ptr<TableId> make_shared(TableId v)
    {
        return make_shared<TableId>(v);
    }
};

/**********************class TableIdEntity**********************/
/* public functions */
TableIdEntity::TableIdEntity(const string& tableName, shared_ptr<TableId> tableId)
    : tableName(tableName), tableId(tableId)
{}

TableIdEntity::TableIdEntity()
    : TableIdEntity("", nullptr)
{}

TableIdEntity::TableIdEntity(const char *tableName, TableId tableId)
    : TableIdEntity(tableName, make_shared<TableId>(tableId))
{}

TableIdEntity::~TableIdEntity()
{}

const string& TableIdEntity::GetTableName() const
{
    return tableName;
}

void TableIdEntity::SetTableName(const string& tableName)
{
    this->tableName = tableName;
}

TableId TableIdEntity::GetTableId() const
{
    return *tableId;
}

void TableIdEntity::SetTableId(TableId tableId)
{
    MemberHelper::SetMember(this->tableId, tableId);
}

/* private functions */

/**********************class MarketEntity**********************/
MarketEntity::MarketEntity(TableId id, shared_ptr<CoinType> coinType,
    shared_ptr<double> bidPrice, shared_ptr<double> askPrice, shared_ptr<double> lastPrice)
    : id(id), coinType(coinType),
    bidPrice(bidPrice), askPrice(askPrice), lastPrice(lastPrice),
    time(::time(nullptr)), platformEnt()
{}

MarketEntity::MarketEntity()
    : MarketEntity(InvalidTableId, nullptr, nullptr, nullptr, nullptr)
{}

MarketEntity::MarketEntity(TableId id, CoinType coinType, Money bidPrice, Money askPrice, Money lastPrice)
    : MarketEntity(id, make_shared<CoinType>(coinType),
    make_shared(bidPrice), make_shared(askPrice), make_shared(lastPrice))
{}

MarketEntity::~MarketEntity()
{}

TableId MarketEntity::GetId() const
{
    return id;
}

void MarketEntity::SetId(TableId id)
{
    this->id = id;
}

CoinType MarketEntity::GetCoinType() const
{
    return *coinType;
}

void MarketEntity::SetCoinType(CoinType coinType)
{
    MemberHelper::SetMember(this->coinType, coinType);
}

Money MarketEntity::GetBidPrice() const
{
    return Money(*bidPrice);
}

void MarketEntity::SetBidPrice(Money bidPrice)
{
    MemberHelper::SetMember(this->bidPrice, (double)bidPrice);
}

Money MarketEntity::GetAskPrice() const
{
    return Money(*askPrice);
}

void MarketEntity::SetAskPrice(Money askPrice)
{
    MemberHelper::SetMember(this->askPrice, (double)askPrice);
}

Money MarketEntity::GetLastPrice() const
{
    return Money(*lastPrice);
}

void MarketEntity::SetLastPrice(Money lastPrice)
{
    MemberHelper::SetMember(this->lastPrice, (double)lastPrice);
}

time_point MarketEntity::GetTime() const
{
    return time_point(seconds(time));
}

void MarketEntity::SetTime(time_point time)
{
    this->time = (int64_t)system_clock::to_time_t(time);
}

shared_ptr<PlatformEntity> MarketEntity::GetPlatform() const
{
    return platformEnt.lock();
}

void MarketEntity::SetPlatform(shared_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::SetMember(this->platformEnt, platformEnt, shared_from_this());
}

/* auxiliary functions */
TableId MarketEntity::GetPlatformId() const
{
    return platformEnt.lock()->GetId();
}

/* private functions */
/* Bind, Unbind */
void MarketEntity::Bind(weak_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::BindMember(this->platformEnt, platformEnt, shared_from_this());
}

void MarketEntity::Unbind(weak_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::UnbindMember(this->platformEnt, platformEnt, shared_from_this());
}

/**********************class AccountEntity**********************/
/* public functions */
AccountEntity::AccountEntity(TableId id, shared_ptr<string> userName,
    shared_ptr<string> publicKey, shared_ptr<string> privateKey,
    shared_ptr<double> investedMoney, shared_ptr<double> investedCoinNumber)
	: id(id), userName(userName), publicKey(publicKey), privateKey(privateKey),
    investedMoney(investedMoney), investedCoinNumber(investedCoinNumber), description(""),
    availableMoney(), availableCoinNumber()
{}

AccountEntity::AccountEntity()
: AccountEntity(InvalidTableId, nullptr, nullptr, nullptr, nullptr, nullptr)
{}

AccountEntity::AccountEntity(TableId id, const char *userName,
    const char *publicKey, const char *privateKey,
    Money investedMoney, CoinNumber investedCoinNumber)
    : AccountEntity(id, make_shared<string>(userName),
    make_shared<string>(publicKey), make_shared<string>(privateKey),
    make_shared(investedMoney), make_shared(investedCoinNumber))
{}

AccountEntity::~AccountEntity()
{}

TableId AccountEntity::GetId() const
{
    return id;
}

void AccountEntity::SetId(TableId id)
{
    this->id = id;
}

const string& AccountEntity::GetAccountUserName() const
{
    return *userName;
}

void AccountEntity::SetAccountUserName(const string& userName)
{
    MemberHelper::SetMember(this->userName, userName);
}

const string& AccountEntity::GetPublicKey() const
{
    return *publicKey;
}

void AccountEntity::SetPublicKey(const string& publicKey)
{
    MemberHelper::SetMember(this->publicKey, publicKey);
}

const string& AccountEntity::GetPrivateKey() const
{
    return *privateKey;
}

void AccountEntity::SetPrivateKey(const string& privateKey)
{
    MemberHelper::SetMember(this->privateKey, privateKey);
}

Money AccountEntity::GetInvestedMoney() const
{
    return Money(*investedMoney);
}

void AccountEntity::SetInvestedMoney(Money investedMoney)
{
    MemberHelper::SetMember(this->investedMoney, (double)investedMoney);
}

CoinNumber AccountEntity::GetInvestedCoinNumber() const
{
    return CoinNumber(*investedCoinNumber);
}

void AccountEntity::SetInvestedCoinNumber(CoinNumber investedCoinNumber)
{
    MemberHelper::SetMember(this->investedCoinNumber, (double)investedCoinNumber);
}

const string& AccountEntity::GetDescription() const
{
    return description;
}

void AccountEntity::SetDescription(const string& description)
{
    this->description = description;
}

Money AccountEntity::GetAvailableMoney() const
{
    return *availableMoney;
}

void AccountEntity::SetAvailableMoney(Money availableMoney)
{
    MemberHelper::SetMember(this->availableMoney, availableMoney);
}

void AccountEntity::IncAvailableMoney(Money money)
{
    *availableMoney += money;
}

void AccountEntity::DecAvailableMoney(Money money)
{
    assert(*availableMoney >= money);
    *availableMoney -= money;
}

CoinNumber AccountEntity::GetAvailableCoinNumber() const
{
    return *availableCoinNumber;
}

void AccountEntity::SetAvailableCoinNumber(CoinNumber availableCoinNumber)
{
    MemberHelper::SetMember(this->availableCoinNumber, availableCoinNumber);
}

void AccountEntity::IncAvailableCoinNumber(CoinNumber coinNumber)
{
    *availableCoinNumber += coinNumber;
}

void AccountEntity::DecAvailableCoinNumber(CoinNumber coinNumber)
{
    assert(*availableCoinNumber >= coinNumber);
    *availableCoinNumber -= coinNumber;
}

const list<lazy_shared_ptr<OrderEntity>>& AccountEntity::GetOrders() const
{
    return orderEnts;
}

list<lazy_shared_ptr<OrderEntity>>& AccountEntity::GetOrders()
{
    return orderEnts;
}

void AccountEntity::Bind(shared_ptr<OrderEntity> orderEnt)
{
    MemberHelper::BindMember(this->orderEnts, orderEnt, shared_from_this());
}

void AccountEntity::Unbind(shared_ptr<OrderEntity> orderEnt)
{
    MemberHelper::UnbindMember(this->orderEnts, orderEnt, shared_from_this());
}

shared_ptr<PlatformEntity> AccountEntity::GetPlatform() const
{
    return platformEnt.lock();
}

void AccountEntity::SetPlatform(shared_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::SetMember(this->platformEnt, platformEnt, shared_from_this());
}

/* auxiliary functions */
milliseconds AccountEntity::GetPlatformApiAverageResponseTime() const
{
    assert(!platformEnt.expired());
    return platformEnt.lock()->GetApiAverageResponseTime();
}

/* private function */
/* Bind, Unbind */
void AccountEntity::Bind(weak_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::BindMember(this->platformEnt, platformEnt, shared_from_this());
}

void AccountEntity::Unbind(weak_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::UnbindMember(this->platformEnt, platformEnt, shared_from_this());
}

/**********************class OrderEntity**********************/
OrderEntity::OrderEntity(TableId id, shared_ptr<OrderType> orderType, shared_ptr<PlatformOrderId> platformOrderId,
    shared_ptr<double> limitedPrice, shared_ptr<double> coinNumber)
    : id(id), orderType(orderType), platformOrderId(platformOrderId),
    limitedPrice(limitedPrice), concludedPrice(), coinNumber(coinNumber),
	filledCoinNumber(0.0),
    creatingTime(time(nullptr)),
	closingTime(), canceledReason()
{}

OrderEntity::OrderEntity()
    : OrderEntity(InvalidTableId, nullptr /* orderType */, nullptr /* platformOrderId */,
    nullptr /* limitedPrice */, nullptr /* coinNumber */)
{}

OrderEntity::OrderEntity(TableId id, OrderType orderType, Money limitedPrice, CoinNumber coinNumber)
    : OrderEntity(id, make_shared<OrderType>(orderType), nullptr /* platformOrderId */,
    make_shared(limitedPrice), make_shared(coinNumber))
{}

OrderEntity::~OrderEntity()
{}

TableId OrderEntity::GetId() const
{
    return id;
}

void OrderEntity::SetId(TableId id)
{
    this->id = id;
}

OrderType OrderEntity::GetOrderType() const
{
    return *orderType;
}

void OrderEntity::SetOrderType(OrderType orderType)
{
    MemberHelper::SetMember(this->orderType, orderType);
}

PlatformOrderId OrderEntity::GetPlatformOrderId() const
{
    return *platformOrderId;
}

void OrderEntity::SetPlatformOrderId(PlatformOrderId platformOrderId)
{
    MemberHelper::SetMember(this->platformOrderId, platformOrderId);
}

Money OrderEntity::GetLimitedPrice() const
{
    return Money(*limitedPrice);
}

void OrderEntity::SetLimitedPrice(Money limitedPrice)
{
    MemberHelper::SetMember(this->limitedPrice, (double)limitedPrice);
}

Money OrderEntity::GetConcludedPrice() const
{
    return Money(*concludedPrice);
}

void OrderEntity::SetConcludedPrice(Money concludedPrice)
{
    MemberHelper::SetMember(this->concludedPrice, (double)concludedPrice);
}

CoinNumber OrderEntity::GetCoinNumber() const
{
    return CoinNumber(*coinNumber);
}

void OrderEntity::SetCoinNumber(CoinNumber coinNumber)
{
    MemberHelper::SetMember(this->coinNumber, (double)coinNumber);
}

CoinNumber OrderEntity::GetFilledCoinNumber() const
{
    return CoinNumber(filledCoinNumber);
}

void OrderEntity::SetFilledCoinNumber(CoinNumber filledCoinNumber)
{
    assert(CoinNumber(*coinNumber) >= filledCoinNumber);
    this->filledCoinNumber = (double)filledCoinNumber;

    if (filledCoinNumber == CoinNumber::Zero())
        concludedPrice.reset();
}

time_point OrderEntity::GetCreatingTime() const
{
    return system_clock::from_time_t(creatingTime);
}

void OrderEntity::SetCreatingTime(time_point creatingTime)
{
    this->creatingTime = (int64_t)system_clock::to_time_t(creatingTime);
}

shared_ptr<time_point> OrderEntity::GetClosingTime() const
{
	if (closingTime == nullptr)
	{
		return nullptr;
	}

    return make_shared<time_point>(system_clock::from_time_t(*closingTime));
}

void OrderEntity::SetClosingTime(time_point closingTime)
{
    MemberHelper::SetMember(this->closingTime, (int64_t)system_clock::to_time_t(closingTime));
}

shared_ptr<OrderCanceledReason> OrderEntity::GetCanceledReason() const
{
    return canceledReason;
}

void OrderEntity::SetCanceledReason(OrderCanceledReason canceledReason)
{
    MemberHelper::SetMember(this->canceledReason, canceledReason);
}

shared_ptr<AccountEntity> OrderEntity::GetAccount() const
{
    return accountEnt.lock();
}

void OrderEntity::SetAccount(shared_ptr<AccountEntity> accountEnt)
{
    MemberHelper::SetMember(this->accountEnt, accountEnt, shared_from_this());
}

/* auxiliary functions */
Money OrderEntity::GetFreezingAmount() const
{
    return Money(*limitedPrice * *coinNumber);
}

Money OrderEntity::GetFilledCoinAmount() const
{
    if (CoinNumber(filledCoinNumber) == CoinNumber::Zero())
    {
        assert(concludedPrice == nullptr);
        return Money::Zero();
    }

    return Money(*concludedPrice * filledCoinNumber);
}

CoinNumber OrderEntity::GetUnfilledCoinNumber() const
{
    assert(CoinNumber(*coinNumber) >= CoinNumber(filledCoinNumber));
    return CoinNumber(*coinNumber - filledCoinNumber);
}


shared_ptr<PlatformEntity> OrderEntity::GetPlatform() const
{
    shared_ptr<AccountEntity> account = GetAccount();
    return account->GetPlatform();
}

/* private functions */
/* Bind, Unbind */
void OrderEntity::Bind(weak_ptr<AccountEntity> accountEnt)
{
    MemberHelper::BindMember(this->accountEnt, accountEnt, shared_from_this());
}

void OrderEntity::Unbind(weak_ptr<AccountEntity> accountEnt)
{
    MemberHelper::UnbindMember(accountEnt, accountEnt, shared_from_this());
}

bool operator<(lazy_shared_ptr<OrderEntity> left, lazy_shared_ptr<OrderEntity> right)
{
    return left.object_id() < right.object_id();
}

/**********************class PlatformEntity**********************/
PlatformEntity::PlatformEntity(TableId id, shared_ptr<string> name,
    shared_ptr<int64_t> minMarketQueryInvl, shared_ptr<int64_t> minOrderQueryInvl,
    shared_ptr<int64_t> minTradeInvl, shared_ptr<double> cashWithdrawalFeeRate,
    shared_ptr<int64_t> apiAverageResponseTime)
    : id(id), name(name), minMarketQueryInvl(minMarketQueryInvl), minOrderQueryInvl(minOrderQueryInvl),
    minTradeInvl(minTradeInvl), cashWithdrawalFeeRate(cashWithdrawalFeeRate),
	apiAverageResponseTime(apiAverageResponseTime), description("")
{}

PlatformEntity::PlatformEntity()
: PlatformEntity(InvalidTableId, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
{}

PlatformEntity::PlatformEntity(TableId id, const char *name,
    milliseconds minMarketQueryInvl, milliseconds minOrderQueryInvl,
    milliseconds minTradeInvl, Rate cashWithdrawalFeeRate,
    milliseconds apiAverageResponseTime)
    : PlatformEntity(id, make_shared<string>(name),
    make_shared(minMarketQueryInvl), make_shared(minOrderQueryInvl),
    make_shared(minTradeInvl), make_shared(cashWithdrawalFeeRate),
    make_shared(apiAverageResponseTime))
{}

PlatformEntity::~PlatformEntity()
{}

TableId PlatformEntity::GetId() const
{
    return id;
}

void PlatformEntity::SetId(TableId id)
{
    this->id = id;
}

const string& PlatformEntity::GetName() const
{
    return *name;
}

void PlatformEntity::SetName(const string& name)
{
    MemberHelper::SetMember(this->name, name);
}

milliseconds PlatformEntity::GetMinMarketQueryInvl() const
{
    return chrono::milliseconds(*minMarketQueryInvl);
}

void PlatformEntity::SetMinMarketQueryInvl(milliseconds minMarketQueryInvl)
{
    MemberHelper::SetMember(this->minMarketQueryInvl, minMarketQueryInvl.count());
}

milliseconds PlatformEntity::GetMinOrderQueryInvl() const
{
    return chrono::milliseconds(*minOrderQueryInvl);
}

void PlatformEntity::SetMinOrderQueryInvl(milliseconds minOrderQueryInvl)
{
    MemberHelper::SetMember(this->minOrderQueryInvl, minOrderQueryInvl.count());
}

milliseconds PlatformEntity::GetMinTradeInvl() const
{
    return chrono::milliseconds(*minTradeInvl);
}

void PlatformEntity::SetMinTradeInvl(milliseconds minTradeInvl)
{
    MemberHelper::SetMember(this->minTradeInvl, minTradeInvl.count());
}

Rate PlatformEntity::GetCashWithdrawalFeeRate() const
{
    return Rate(*cashWithdrawalFeeRate);
}

void PlatformEntity::SetCashWithdrawalFeeRate(Rate cashWithdrawalFeeRate)
{
    MemberHelper::SetMember(this->cashWithdrawalFeeRate, (double)cashWithdrawalFeeRate);
}

milliseconds PlatformEntity::GetApiAverageResponseTime() const
{
    return chrono::milliseconds(*apiAverageResponseTime);
}

void PlatformEntity::SetApiAverageResponseTime(milliseconds apiAverageResponseTime)
{
    MemberHelper::SetMember(this->apiAverageResponseTime, apiAverageResponseTime.count());
}

const string& PlatformEntity::GetDescription() const
{
	return description;
}

void PlatformEntity::SetDescription(const string& description)
{
	this->description = description;
}

const list<shared_ptr<PlatformApiRuleEntity>>& PlatformEntity::GetApiRules() const
{
    return apiRuleEnts;
}

list<shared_ptr<PlatformApiRuleEntity>>& PlatformEntity::GetApiRules()
{
    return apiRuleEnts;
}

void PlatformEntity::Bind(shared_ptr<PlatformApiRuleEntity> apiRuleEnt)
{
    MemberHelper::BindMember(this->apiRuleEnts, apiRuleEnt, shared_from_this());
}

void PlatformEntity::Unbind(shared_ptr<PlatformApiRuleEntity> apiRuleEnt)
{
    MemberHelper::UnbindMember(this->apiRuleEnts, apiRuleEnt, shared_from_this());
}

const std::list<shared_ptr<PlatformCoinRuleEntity>>& PlatformEntity::GetCoinRules() const
{
    return coinRuleEnts;
}

std::list<shared_ptr<PlatformCoinRuleEntity>>& PlatformEntity::GetCoinRules()
{
    return coinRuleEnts;
}

void PlatformEntity::Bind(shared_ptr<PlatformCoinRuleEntity> apiRuleEnt)
{
    MemberHelper::BindMember(this->coinRuleEnts, apiRuleEnt, shared_from_this());
}

void PlatformEntity::Unbind(shared_ptr<PlatformCoinRuleEntity> apiRuleEnt)
{
    MemberHelper::UnbindMember(this->coinRuleEnts, apiRuleEnt, shared_from_this());
}

const list<shared_ptr<AccountEntity>>& PlatformEntity::GetAccounts() const
{
    return accountEnts;
}

list<shared_ptr<AccountEntity>>& PlatformEntity::GetAccounts()
{
    return accountEnts;
}

void PlatformEntity::Bind(shared_ptr<AccountEntity> accountEnt)
{
    MemberHelper::BindMember(this->accountEnts, accountEnt, shared_from_this());
}

void PlatformEntity::Unbind(shared_ptr<AccountEntity> accountEnt)
{
    MemberHelper::UnbindMember(this->accountEnts, accountEnt, shared_from_this());
}

const list<lazy_shared_ptr<MarketEntity>>& PlatformEntity::GetMarkets() const
{
    return marketEnts;
}

list<lazy_shared_ptr<MarketEntity>>& PlatformEntity::GetMarkets()
{
    return marketEnts;
}

void PlatformEntity::Bind(shared_ptr<MarketEntity> marketEnt)
{
    MemberHelper::BindMember(this->marketEnts, marketEnt, shared_from_this());
}

void PlatformEntity::Unbind(shared_ptr<MarketEntity> marketEnt)
{
    MemberHelper::UnbindMember(this->marketEnts, marketEnt, shared_from_this());
}

/* auxiliary functions */
shared_ptr<PlatformApiRuleEntity> PlatformEntity::GetApiRule(ApiType apiType) const
{
    auto cmp = [](ApiType apiType, shared_ptr<PlatformApiRuleEntity> rule)->bool
    {
        return apiType == rule->GetApiType();
    };

    auto result = find_if(apiRuleEnts.begin(), apiRuleEnts.end(), bind(cmp, apiType, _1));
    return *result;
}

shared_ptr<PlatformCoinRuleEntity> PlatformEntity::GetCoinRule(CoinType coinType) const
{
    auto cmp = [](CoinType coinType, shared_ptr<PlatformCoinRuleEntity> rule)->bool
    {
        return coinType == rule->GetCoinType();
    };

    auto result = find_if(coinRuleEnts.begin(), coinRuleEnts.end(), bind(cmp, coinType, _1));
    return *result;
}

shared_ptr<MarketEntity> PlatformEntity::GetLastMarket()
{
    if (marketEnts.empty())
        return nullptr;

    lazy_shared_ptr<MarketEntity> market = *marketEnts.rbegin();
    if (!market.loaded())
        market.load();

    return market.get_eager();
}

/* private functions */

/**********************class PlatformApiRuleEntity**********************/
PlatformApiRuleEntity::PlatformApiRuleEntity(TableId id, shared_ptr<ApiType> apiType, shared_ptr<string> url,
    shared_ptr<int64_t> curlTimeout)
    : id(id), apiType(apiType), url(url), curlTimeout(curlTimeout)
{}

PlatformApiRuleEntity::PlatformApiRuleEntity()
: PlatformApiRuleEntity(InvalidTableId, nullptr, nullptr, nullptr)
{}

PlatformApiRuleEntity::PlatformApiRuleEntity(TableId id, ApiType apiType, const char *url, seconds curlTimeout)
    : PlatformApiRuleEntity(id, make_shared<ApiType>(apiType), make_shared<string>(url),
    make_shared(curlTimeout))
{}

PlatformApiRuleEntity::~PlatformApiRuleEntity()
{}

TableId PlatformApiRuleEntity::GetId() const
{
    return id;
}

void PlatformApiRuleEntity::SetId(TableId id)
{
    this->id = id;
}

std::shared_ptr<PlatformEntity> PlatformApiRuleEntity::GetPlatform() const
{
    return platformEnt.lock();
}

void PlatformApiRuleEntity::SetPlatform(std::shared_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::SetMember(this->platformEnt, platformEnt, shared_from_this());
}

ApiType PlatformApiRuleEntity::GetApiType() const
{
    return *apiType;
}

void PlatformApiRuleEntity::SetApiType(ApiType apiType)
{
    MemberHelper::SetMember(this->apiType, apiType);
}

const string& PlatformApiRuleEntity::GetUrl() const
{
    return *url;
}

void PlatformApiRuleEntity::SetUrl(const string& url)
{
    MemberHelper::SetMember(this->url, url);
}

seconds PlatformApiRuleEntity::GetCurlTimeout() const
{
    return chrono::seconds(*curlTimeout);
}

void PlatformApiRuleEntity::SetCurlTimeout(seconds curlTimeout)
{
    MemberHelper::SetMember(this->curlTimeout, curlTimeout.count());
}

/* private functions */
/* Bind, Unbind */
void PlatformApiRuleEntity::Bind(weak_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::BindMember(this->platformEnt, platformEnt, shared_from_this());
}

void PlatformApiRuleEntity::Unbind(weak_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::UnbindMember(this->platformEnt, platformEnt, shared_from_this());
}

/**********************class PlatformCoinRuleEntity**********************/
PlatformCoinRuleEntity::PlatformCoinRuleEntity(TableId id, shared_ptr<CoinType> coinType,
    shared_ptr<double> minTradingUnit, shared_ptr<double> transactionFeeRate,
    shared_ptr<ChargeType> coinWithdrawalChargeType, shared_ptr<double> coinWithdrawalPara,
    shared_ptr<double> minPriceSize)
    : id(id), platformEnt(), coinType(coinType),
    minTradingUnit(minTradingUnit), transactionFeeRate(transactionFeeRate),
    coinWithdrawalChargeType(coinWithdrawalChargeType), coinWithdrawalPara(coinWithdrawalPara),
    minPriceSize(minPriceSize)
{}

PlatformCoinRuleEntity::PlatformCoinRuleEntity()
: PlatformCoinRuleEntity(InvalidTableId, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
{}

PlatformCoinRuleEntity::PlatformCoinRuleEntity(TableId id, CoinType coinType,  CoinNumber minTradingUnit,
    Rate transactionFeeRate, ChargeType coinWithdrawalChargeType, double coinWithdrawalPara,
    Money minPriceSize)
    : PlatformCoinRuleEntity(id, make_shared<CoinType>(coinType),  make_shared(minTradingUnit),
    make_shared(transactionFeeRate), make_shared<ChargeType>(coinWithdrawalChargeType),
    make_shared(coinWithdrawalPara), make_shared(minPriceSize))
{}

PlatformCoinRuleEntity::~PlatformCoinRuleEntity()
{}

TableId PlatformCoinRuleEntity::GetId() const
{
    return id;
}

void PlatformCoinRuleEntity::SetId(TableId id)
{
    this->id = id;
}

std::shared_ptr<PlatformEntity> PlatformCoinRuleEntity::GetPlatform() const
{
    return platformEnt.lock();
}

void PlatformCoinRuleEntity::SetPlatform(shared_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::SetMember(this->platformEnt, platformEnt, shared_from_this());
}

CoinType PlatformCoinRuleEntity::GetCoinType() const
{
    return *coinType;
}

void PlatformCoinRuleEntity::SetCoinType(CoinType coinType)
{
    MemberHelper::SetMember(this->coinType, coinType);
}

CoinNumber PlatformCoinRuleEntity::GetMinTradingUnit() const
{
    return CoinNumber(*minTradingUnit);
}

void PlatformCoinRuleEntity::SetMinTradingUnit(CoinNumber minTradingUnit)
{
    MemberHelper::SetMember(this->minTradingUnit, (double)minTradingUnit);
}

Rate PlatformCoinRuleEntity::GetTransactionFeeRate() const
{
    return Rate(*transactionFeeRate);
}

void PlatformCoinRuleEntity::SetTransactionFeeRate(Rate transactionFeeRate)
{
    MemberHelper::SetMember(this->transactionFeeRate, (double)transactionFeeRate);
}

ChargeType PlatformCoinRuleEntity::GetCoinWithdrawalChargeType() const
{
    return *coinWithdrawalChargeType;
}

void PlatformCoinRuleEntity::SetCoinWithdrawalChargeType(ChargeType coinWithdrawalChargeType)
{
    MemberHelper::SetMember(this->coinWithdrawalChargeType, coinWithdrawalChargeType);
}

double PlatformCoinRuleEntity::GetCoinWithdrawalPara() const
{
    return *coinWithdrawalPara;
}

void PlatformCoinRuleEntity::SetCoinWithdrawalPara(double coinWithdrawalPara)
{
    MemberHelper::SetMember(this->coinWithdrawalPara, coinWithdrawalPara);
}

Money PlatformCoinRuleEntity::GetMinPriceSize() const
{
    return Money(*minPriceSize);
}

void PlatformCoinRuleEntity::SetMinPriceSize(Money minPriceSize)
{
    MemberHelper::SetMember(this->minPriceSize, (double)minPriceSize);
}

/* private functions */
/* Bind, Unbind */
void PlatformCoinRuleEntity::Bind(weak_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::BindMember(this->platformEnt, platformEnt, shared_from_this());
}

void PlatformCoinRuleEntity::Unbind(weak_ptr<PlatformEntity> platformEnt)
{
    MemberHelper::UnbindMember(this->platformEnt, platformEnt, shared_from_this());
}

/**********************class RobotEntity**********************/
/* public functions */
RobotEntity::RobotEntity(TableId id, shared_ptr<RobotCfgState> robotCfgState)
: id(id), robotCfgState(robotCfgState), description("")
{}

RobotEntity::RobotEntity()
: RobotEntity(InvalidTableId, nullptr)
{}

RobotEntity::RobotEntity(TableId id, RobotCfgState robotCfgState)
: RobotEntity(id, make_shared<RobotCfgState>(robotCfgState))
{}

RobotEntity::~RobotEntity()
{}

TableId RobotEntity::GetId() const
{
    return id;
}

void RobotEntity::SetId(TableId id)
{
    this->id = id;
}

RobotCfgState RobotEntity::GetRobotCfgState() const
{
    return *robotCfgState;
}

void RobotEntity::SetRobotCfgState(RobotCfgState robotCfgState)
{
    MemberHelper::SetMember(this->robotCfgState, robotCfgState);
}

const string& RobotEntity::GetDescription() const
{
    return description;
}

void RobotEntity::SetDescription(const string& description)
{
    this->description = description;
}

/* private functions */

/***********************************************************************
************************  Hedge Robot Entity   *************************
***********************************************************************/

/**********************class HrAccountEntity**********************/
/* public functions */
HrAccountEntity::HrAccountEntity(TableId id, shared_ptr<string> userName,
    shared_ptr<string> publicKey, shared_ptr<string> privateKey,
    shared_ptr<double> investedMoney, shared_ptr<double> investedCoinNumber)
    : AccountEntity(id, userName, publicKey, privateKey, investedMoney, investedCoinNumber)
{}

HrAccountEntity::HrAccountEntity()
    : HrAccountEntity(InvalidTableId, nullptr /* userName */,
    nullptr /* publicKey */, nullptr /* privateKey */,
    nullptr /* investedMoney */, nullptr /* investedCoinNumber */)
{}

HrAccountEntity::HrAccountEntity(TableId id, const char *userName,
    const char *publicKey, const char *privateKey,
    Money investedMoney, CoinNumber investedCoinNumber)
    : HrAccountEntity(id, make_shared<string>(userName),
    make_shared<string>(publicKey), make_shared<string>(privateKey),
    make_shared(investedMoney), make_shared(investedCoinNumber))
{}

HrAccountEntity::~HrAccountEntity()
{}

shared_ptr<HrAccountGroupEntity> HrAccountEntity::GetHrAccountGroup() const
{
    return hrAccountGroupEnt.lock();
}

void HrAccountEntity::SetHrAccountGroup(shared_ptr<HrAccountGroupEntity> hrAccountGroupEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccountEntity>(shared_from_this());
    MemberHelper::SetMember(this->hrAccountGroupEnt, hrAccountGroupEnt, thisPtr);
}

/* auxiliary functions */

/* private functions */
/* Bind, Unbind */
void HrAccountEntity::Bind(weak_ptr<HrAccountGroupEntity> hrAccountGroupEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccountEntity>(shared_from_this());
    MemberHelper::BindMember(this->hrAccountGroupEnt, hrAccountGroupEnt, thisPtr);
}

void HrAccountEntity::Unbind(weak_ptr<HrAccountGroupEntity> hrAccountGroupEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccountEntity>(shared_from_this());
    MemberHelper::UnbindMember(this->hrAccountGroupEnt, hrAccountGroupEnt, thisPtr);
}

/**********************class HrAccountGroupEntity**********************/
HrAccountGroupEntity::HrAccountGroupEntity(TableId id)
    : id(id), state(""), description("")
{}

HrAccountGroupEntity::HrAccountGroupEntity()
    : HrAccountGroupEntity(InvalidTableId)
{}

HrAccountGroupEntity::~HrAccountGroupEntity()
{}

TableId HrAccountGroupEntity::GetId() const
{
    return id;
}

void HrAccountGroupEntity::SetId(TableId id)
{
    this->id = id;
}

const string& HrAccountGroupEntity::GetState() const
{
    return state;
}

void HrAccountGroupEntity::SetState(const string& state)
{
    this->state = state;
}

const string& HrAccountGroupEntity::GetDescription() const
{
    return description;
}

void HrAccountGroupEntity::SetDescription(const string& description)
{
    this->description = description;
}

shared_ptr<HrEntity> HrAccountGroupEntity::GetRobot() const
{
    return hrEnt.lock();
}

void HrAccountGroupEntity::SetRobot(shared_ptr<HrEntity> hrEnt)
{
    MemberHelper::SetMember(this->hrEnt, hrEnt, shared_from_this());
}

shared_ptr<HrAccountEntity> HrAccountGroupEntity::GetHrAccount0() const
{
    return hrAccountEnt0;
}

shared_ptr<HrAccountEntity> HrAccountGroupEntity::GetHrAccount1() const
{
    return hrAccountEnt1;
}

void HrAccountGroupEntity::Bind(shared_ptr<HrAccountEntity> hrAccountEnt)
{
    assert(hrAccountEnt1 == nullptr);
    if (hrAccountEnt0 == nullptr)
    {
        MemberHelper::BindMember(hrAccountEnt0, hrAccountEnt, shared_from_this());
    }
    else
    {
		MemberHelper::BindMember(hrAccountEnt1, hrAccountEnt, shared_from_this());
		if (hrAccountEnt0->GetPlatformApiAverageResponseTime() > hrAccountEnt1->GetPlatformApiAverageResponseTime())
        {
            swap(hrAccountEnt0, hrAccountEnt1);
        }
    }
}

void HrAccountGroupEntity::Unbind(shared_ptr<HrAccountEntity> hrAccountEnt)
{
    assert(hrAccountEnt0 != nullptr);
    if (hrAccountEnt0->GetId() == hrAccountEnt->GetId())
    {
        MemberHelper::UnbindMember(hrAccountEnt0, hrAccountEnt, shared_from_this());
        swap(hrAccountEnt0, hrAccountEnt1);
    }
    else
    {
        assert(hrAccountEnt1 != nullptr && hrAccountEnt1->GetId() == hrAccountEnt->GetId());
        MemberHelper::UnbindMember(hrAccountEnt1, hrAccountEnt, shared_from_this());
    }
}

/* private functions */
/* Bind, Unbind */
void HrAccountGroupEntity::Bind(weak_ptr<HrEntity> hrEnt)
{
    MemberHelper::BindMember(this->hrEnt, hrEnt, shared_from_this());
}

void HrAccountGroupEntity::Unbind(weak_ptr<HrEntity> hrEnt)
{
    MemberHelper::UnbindMember(this->hrEnt, hrEnt, shared_from_this());
}

/**********************class HrAccGrpNormalEntity**********************/
/* public functions */
HrAccGrpNormalEntity::HrAccGrpNormalEntity(TableId id, shared_ptr<HrNs::AccountGroupMln> accGrpMln)
: HrAccountGroupEntity(id), accGrpMln(accGrpMln)
{}

HrAccGrpNormalEntity::HrAccGrpNormalEntity()
: HrAccGrpNormalEntity(InvalidTableId, nullptr)
{}

HrAccGrpNormalEntity::HrAccGrpNormalEntity(TableId id, HrNs::AccountGroupMln accGrpMln)
    : HrAccGrpNormalEntity(id, make_shared<HrNs::AccountGroupMln>(accGrpMln))
{}

HrAccGrpNormalEntity::~HrAccGrpNormalEntity()
{}

HrNs::AccountGroupMln HrAccGrpNormalEntity::GetAccGrpMln() const
{
	return *accGrpMln;
}

void HrAccGrpNormalEntity::SetAccGrpMln(HrNs::AccountGroupMln accGrpMln)
{
    MemberHelper::SetMember(this->accGrpMln, accGrpMln);
}

shared_ptr<HrAccGrpOpmTrialEntity> HrAccGrpNormalEntity::GetAccGrpOpmTrial() const
{
    return accGrpOpmTrialEnt.lock();
}

void HrAccGrpNormalEntity::SetAccGrpOpmTrial(shared_ptr<HrAccGrpOpmTrialEntity> accGrpOpmTrialEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccGrpNormalEntity>(shared_from_this());
    MemberHelper::SetMember(this->accGrpOpmTrialEnt, accGrpOpmTrialEnt, thisPtr);
}

shared_ptr<HrAccGrpHorzTrialEntity> HrAccGrpNormalEntity::GetAccGrpHorzTrial() const
{
    return accGrpHorzTrialEnt.lock();
}

void HrAccGrpNormalEntity::SetAccGrpHorzTrial(shared_ptr<HrAccGrpHorzTrialEntity> accGrpHorzTrialEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccGrpNormalEntity>(shared_from_this());
    MemberHelper::SetMember(this->accGrpHorzTrialEnt, accGrpHorzTrialEnt, thisPtr);
}

void HrAccGrpNormalEntity::Bind(shared_ptr<HrAccountEntity> hrAccountEnt)
{
    HrAccountGroupEntity::Bind(hrAccountEnt);
}

void HrAccGrpNormalEntity::Unbind(shared_ptr<HrAccountEntity> hrAccountEnt)
{
    HrAccountGroupEntity::Unbind(hrAccountEnt);
}

/* private functions */
/* Bind, Unbind */
void HrAccGrpNormalEntity::Bind(weak_ptr<HrAccGrpOpmTrialEntity> accGrpOpmTrialEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccGrpNormalEntity>(shared_from_this());
    MemberHelper::BindMember(this->accGrpOpmTrialEnt, accGrpOpmTrialEnt, thisPtr);
}

void HrAccGrpNormalEntity::Unbind(weak_ptr<HrAccGrpOpmTrialEntity> accGrpOpmTrialEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccGrpNormalEntity>(shared_from_this());
    MemberHelper::UnbindMember(this->accGrpOpmTrialEnt, accGrpOpmTrialEnt, thisPtr);
}

void HrAccGrpNormalEntity::Bind(weak_ptr<HrAccGrpHorzTrialEntity> accGrpHorzTrialEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccGrpNormalEntity>(shared_from_this());
    MemberHelper::BindMember(this->accGrpHorzTrialEnt, accGrpHorzTrialEnt, thisPtr);
}

void HrAccGrpNormalEntity::Unbind(weak_ptr<HrAccGrpHorzTrialEntity> accGrpHorzTrialEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccGrpNormalEntity>(shared_from_this());
    MemberHelper::UnbindMember(this->accGrpHorzTrialEnt, accGrpHorzTrialEnt, thisPtr);
}

/**********************class HrAccGrpHorzTrialEntity**********************/
HrAccGrpHorzTrialEntity::HrAccGrpHorzTrialEntity()
: HrAccountGroupEntity(InvalidTableId)
{}

HrAccGrpHorzTrialEntity::HrAccGrpHorzTrialEntity(TableId id)
: HrAccountGroupEntity(id)
{}

HrAccGrpHorzTrialEntity::~HrAccGrpHorzTrialEntity()
{}

HrNs::AccountGroupMln HrAccGrpHorzTrialEntity::GetAccGrpMln() const
{
    return accGrpNormalEnt->GetAccGrpMln();
}

void HrAccGrpHorzTrialEntity::SetAccGrpMln(HrNs::AccountGroupMln accGrpMln)
{
    assert(false);
}

shared_ptr<HrAccGrpNormalEntity> HrAccGrpHorzTrialEntity::GetAccGrpNormal() const
{
    return accGrpNormalEnt;
}

void HrAccGrpHorzTrialEntity::SetAccGrpNormal(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccGrpHorzTrialEntity>(shared_from_this());
    MemberHelper::SetMember(this->accGrpNormalEnt, accGrpNormalEnt, thisPtr);
}

/* private functions */
/* Bind, Unbind */
void HrAccGrpHorzTrialEntity::Bind(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt)
{
    assert(this->accGrpNormalEnt == nullptr);
    auto thisPtr = dynamic_pointer_cast<HrAccGrpHorzTrialEntity>(shared_from_this());
    MemberHelper::BindMember(this->accGrpNormalEnt, accGrpNormalEnt, thisPtr);
}

void HrAccGrpHorzTrialEntity::Unbind(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt)
{
    assert(this->accGrpNormalEnt != nullptr);
    auto thisPtr = dynamic_pointer_cast<HrAccGrpHorzTrialEntity>(shared_from_this());
    MemberHelper::UnbindMember(this->accGrpNormalEnt, accGrpNormalEnt, thisPtr);
}

/**********************class HrAccGrpOpmTrialEntity**********************/
HrAccGrpOpmTrialEntity::HrAccGrpOpmTrialEntity()
: HrAccountGroupEntity(InvalidTableId)
{}

HrAccGrpOpmTrialEntity::HrAccGrpOpmTrialEntity(TableId id)
: HrAccountGroupEntity(id)
{}

HrAccGrpOpmTrialEntity::~HrAccGrpOpmTrialEntity()
{}

HrNs::AccountGroupMln HrAccGrpOpmTrialEntity::GetAccGrpMln() const
{
    return accGrpNormalEnt->GetAccGrpMln();
}

void HrAccGrpOpmTrialEntity::SetAccGrpMln(HrNs::AccountGroupMln accGrpMln)
{
    assert(false);
}

shared_ptr<HrAccGrpNormalEntity> HrAccGrpOpmTrialEntity::GetAccGrpNormal() const
{
    return accGrpNormalEnt;
}

void HrAccGrpOpmTrialEntity::SetAccGrpNormal(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccGrpOpmTrialEntity>(shared_from_this());
    MemberHelper::SetMember(this->accGrpNormalEnt, accGrpNormalEnt, thisPtr);
}

/* private functions */
/* Bind, Unbind */
void HrAccGrpOpmTrialEntity::Bind(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccGrpOpmTrialEntity>(shared_from_this());
    MemberHelper::BindMember(this->accGrpNormalEnt, accGrpNormalEnt, thisPtr);
}

void HrAccGrpOpmTrialEntity::Unbind(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrAccGrpOpmTrialEntity>(shared_from_this());
    MemberHelper::BindMember(accGrpNormalEnt, accGrpNormalEnt, thisPtr);
}

/**********************class HrEntity**********************/
/* public functions */
HrEntity::HrEntity(TableId id, shared_ptr<RobotCfgState> robotCfgState, shared_ptr<CoinType> coinType,
    shared_ptr<int64_t> horzMarginAdjustInvl, shared_ptr<int64_t> vertMarginAdjustInvl,
    shared_ptr<int64_t> opmAdjustInvl, shared_ptr<double> minTradingUnit, shared_ptr<double> stopLossAdjustSize,
    shared_ptr<int64_t> order2ndTimeOut, shared_ptr<int64_t> stopLossOrderTimeOut)
    : RobotEntity(id, robotCfgState), coinType(coinType),
    horzMarginAdjustInvl(horzMarginAdjustInvl), vertMarginAdjustInvl(vertMarginAdjustInvl),
    opmAdjustInvl(opmAdjustInvl), minTradingUnit(minTradingUnit), stopLossAdjustSize(stopLossAdjustSize),
    order2ndTimeOut(order2ndTimeOut), stopLossOrderTimeOut(stopLossOrderTimeOut)
{}

HrEntity::HrEntity()
    : HrEntity(InvalidTableId, nullptr /* robotCfgState */, nullptr /* robotCfgState */,
      nullptr /* horzMarginAdjustInvl */, nullptr /* vertMarginAdjustInvl */,
      nullptr /* opmAdjustInvl */, nullptr /* minTradingUnit */, nullptr /* stopLossAdjustSize */,
      nullptr /* order2ndTimeOut */, nullptr /* stopLossOrderTimeOut */)
{}

HrEntity::HrEntity(TableId id, RobotCfgState robotCfgState, CoinType coinType,
    seconds horzMarginAdjustInvl, seconds vertMarginAdjustInvl,
    seconds opmAdjustInvl, CoinNumber minTradingUnit, Rate stopLossAdjustSize,
    seconds order2ndTimeOut, seconds stopLossOrderTimeOut)
    : HrEntity(id, make_shared<RobotCfgState>(robotCfgState), make_shared<CoinType>(coinType),
    make_shared(horzMarginAdjustInvl), make_shared(vertMarginAdjustInvl),
    make_shared(opmAdjustInvl), make_shared(minTradingUnit), make_shared(stopLossAdjustSize),
    make_shared(order2ndTimeOut), make_shared(stopLossOrderTimeOut))
{
    assert(IsHorzMarginAdjustInvlValid(horzMarginAdjustInvl.count()));
    assert(IsVertMarginAdjustInvlValid(vertMarginAdjustInvl.count()));
    assert(IsOpmAdjustInvlValid(opmAdjustInvl.count()));
}

HrEntity::~HrEntity()
{}

const char* HrEntity::GetName() const
{
    return "HedgeRobot";
}

CoinType HrEntity::GetCoinType() const
{
    return *coinType;
}

void HrEntity::SetCoinType(CoinType coinType)
{
    MemberHelper::SetMember(this->coinType, coinType);
}

seconds HrEntity::GetHorzMarginAdjustInvl() const
{
    return chrono::seconds(*horzMarginAdjustInvl);
}

void HrEntity::SetHorzMarginAdjust(seconds horzMarginAdjustInvl)
{
    MemberHelper::SetMember(this->horzMarginAdjustInvl, horzMarginAdjustInvl.count());
}

seconds HrEntity::GetVertMarginAdjustInvl() const
{
    return chrono::seconds(*vertMarginAdjustInvl);
}

void HrEntity::SetVertMarginAdjustInvl(seconds vertMarginAdjustInvl)
{
    MemberHelper::SetMember(this->vertMarginAdjustInvl, vertMarginAdjustInvl.count());
}

seconds HrEntity::GetOpmAdjustInvl() const
{
    return chrono::seconds(*opmAdjustInvl);
}

void HrEntity::SetOpmAdjustInvl(seconds opmAdjustInvl)
{
    MemberHelper::SetMember(this->opmAdjustInvl, opmAdjustInvl.count());
}

CoinNumber HrEntity::GetMinTradingUnit() const
{
    return CoinNumber(*minTradingUnit);
}

void HrEntity::SetMinTradingUnit(CoinNumber minTradingUnit)
{
    MemberHelper::SetMember(this->minTradingUnit, (double)minTradingUnit);
}

Rate HrEntity::GetStopLossAdjustSize() const
{
    return Rate(*stopLossAdjustSize);
}

void HrEntity::SetStopLossAdjustSize(Rate stopLossAdjustSize)
{
    MemberHelper::SetMember(this->stopLossAdjustSize, (double)stopLossAdjustSize);
}

seconds HrEntity::GetOrder2ndTimeOut() const
{
    return chrono::seconds(*order2ndTimeOut);
}

void HrEntity::SetOrder2ndTimeOut(seconds order2ndTimeOut)
{
    MemberHelper::SetMember(this->order2ndTimeOut, order2ndTimeOut.count());
}

seconds HrEntity::GetStopLossOrderTimeOut() const
{
    return chrono::seconds(*stopLossOrderTimeOut);
}

void HrEntity::SetStopLossOrderTimeOut(seconds stopLossOrderTimeOut)
{
    MemberHelper::SetMember(this->stopLossOrderTimeOut, stopLossOrderTimeOut.count());
}

const list<shared_ptr<HrPlatformPairEntity>>& HrEntity::GetPlatformPairs() const
{
    return platformPairEnts;
}

list<shared_ptr<HrPlatformPairEntity>>& HrEntity::GetPlatformPairs()
{
    return platformPairEnts;
}

void HrEntity::Bind(shared_ptr<HrPlatformPairEntity> platformPairEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrEntity>(shared_from_this());
    MemberHelper::BindMember(this->platformPairEnts, platformPairEnt, thisPtr);
}

void HrEntity::Unbind(shared_ptr<HrPlatformPairEntity> platformPairEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrEntity>(shared_from_this());
    MemberHelper::UnbindMember(this->platformPairEnts, platformPairEnt, thisPtr);
}

const std::list<shared_ptr<HrAccountGroupEntity>>& HrEntity::GetAccountGroups() const
{
    return accountGroupEnts;
}

std::list<shared_ptr<HrAccountGroupEntity>>& HrEntity::GetAccountGroups()
{
    return accountGroupEnts;
}

void HrEntity::Bind(shared_ptr<HrAccountGroupEntity> accountGroupEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrEntity>(shared_from_this());
    MemberHelper::BindMember(this->accountGroupEnts, accountGroupEnt, thisPtr);
}

void HrEntity::Unbind(shared_ptr<HrAccountGroupEntity> accountGroupEnt)
{
    auto thisPtr = dynamic_pointer_cast<HrEntity>(shared_from_this());
    MemberHelper::UnbindMember(this->accountGroupEnts, accountGroupEnt, thisPtr);
}

/* auxiliary functions */
shared_ptr<HrPlatformPairEntity> HrEntity::GetPlatformPair(TableId ptmAId, TableId ptmBId)
{
    auto cmp = [](TableId ptmAId, TableId ptmBId, shared_ptr<HrPlatformPairEntity> ptmPair)
    {
        return ptmPair->GetPlatformA()->GetId() == ptmAId && ptmPair->GetPlatformB()->GetId() == ptmBId;
    };

    auto iter = find_if(platformPairEnts.begin(), platformPairEnts.end(), bind(cmp, ptmAId, ptmBId, _1));
    return *iter;
}

/* private functions */
bool HrEntity::IsHorzMarginAdjustInvlValid(int64_t horzMarginAdjustInvl)
{
#ifdef UnitTestMode
    return true;
#else
    return ((horzMarginAdjustInvl >= 3600 * 24) && (horzMarginAdjustInvl <= 3600 * 24 * 30));
#endif
}

bool HrEntity::IsVertMarginAdjustInvlValid(int64_t vertMarginAdjustInvl)
{
#ifdef UnitTestMode
    return true;
#else
    return ((vertMarginAdjustInvl >= 3600 * 24) && (vertMarginAdjustInvl <= 3600 * 24 * 30));
#endif
}

bool HrEntity::IsOpmAdjustInvlValid(int64_t opmAdjustInvl)
{
#ifdef UnitTestMode
    return true;
#else
    return ((opmAdjustInvl >= 3600 * 24) && (opmAdjustInvl <= 3600 * 24 * 30));
#endif
}

/**********************class HrOrderEntity**********************/
/* public functions */
HrOrderEntity::HrOrderEntity(TableId id, shared_ptr<OrderType> orderType,
    shared_ptr<PlatformOrderId> platformOrderId, shared_ptr<double> limitedPrice, shared_ptr<double> coinNumber,
    shared_ptr<int64_t> priceScheduleTime, shared_ptr<TableId> parentOrderId)
    : OrderEntity(id, orderType, platformOrderId, limitedPrice, coinNumber),
    priceScheduleTime(priceScheduleTime), parentOrderId(parentOrderId)
{}

HrOrderEntity::HrOrderEntity()
: HrOrderEntity(InvalidTableId, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
{}

HrOrderEntity::HrOrderEntity(const HrOrderEntity& right)
    : HrOrderEntity(right.id, make_shared<OrderType>(*right.orderType),
    make_shared<PlatformOrderId>(*right.platformOrderId),
    make_shared(*right.limitedPrice),
    make_shared(*right.coinNumber),
    nullptr /* priceScheduleTime */, nullptr /* parentOrderId */)
{
    if (right.concludedPrice != nullptr)
    {
        this->concludedPrice = make_shared<double>(*right.concludedPrice);
    }

    this->filledCoinNumber = right.filledCoinNumber;
    this->creatingTime = right.creatingTime;

    if (right.closingTime != nullptr)
    {
        this->closingTime = make_shared<int64_t>(*right.closingTime);
    }

    if (right.canceledReason != nullptr)
    {
        this->canceledReason = make_shared<OrderCanceledReason>(*right.canceledReason);
    }

    if (right.priceScheduleTime != nullptr)
    {
        this->priceScheduleTime = make_shared<int64_t>(*right.priceScheduleTime);
    }
}

HrOrderEntity::HrOrderEntity(TableId id, OrderType orderType, Money limitedPrice, CoinNumber coinNumber)
    : HrOrderEntity(id, make_shared<OrderType>(orderType), nullptr, make_shared(limitedPrice),
    make_shared(coinNumber),
    nullptr /* priceScheduleTime */, nullptr /* parentOrderId */)
{}

HrOrderEntity::~HrOrderEntity()
{}

shared_ptr<OrderEntity> HrOrderEntity::Clone() const
{
    shared_ptr<HrOrderEntity> clone = make_shared<HrOrderEntity>(*this);
    return clone;
}

CoinType HrOrderEntity::GetCoinType() const
{
    shared_ptr<HrEntity> hrEnt = dynamic_pointer_cast<HrEntity>(GetRobot());
    return hrEnt->GetCoinType();
}

void HrOrderEntity::SetCoinType(CoinType coinType)
{
    assert(false);
}

shared_ptr<RobotEntity> HrOrderEntity::GetRobot() const
{
    assert(!accountEnt.expired());
    shared_ptr<HrAccountEntity> hrAccount = dynamic_pointer_cast<HrAccountEntity>(accountEnt.lock());
    weak_ptr<HrAccountGroupEntity> hrAccountGroup = hrAccount->GetHrAccountGroup();

    assert(!hrAccountGroup.expired());
    weak_ptr<HrEntity> robotEnt = hrAccountGroup.lock()->GetRobot();

    return robotEnt.lock();
}

void HrOrderEntity::SetRobot(shared_ptr<RobotEntity>& robotEnt)
{
    assert(false);
}

shared_ptr<time_point> HrOrderEntity::GetPriceScheduleTime() const
{
    if (priceScheduleTime == nullptr)
    {
        return nullptr;
    }

    return make_shared<time_point>(system_clock::from_time_t(*priceScheduleTime));
}

void HrOrderEntity::SetPriceScheduleTime(time_point priceScheduleTime)
{
    MemberHelper::SetMember(this->priceScheduleTime, (int64_t)system_clock::to_time_t(priceScheduleTime));
}

shared_ptr<TableId> HrOrderEntity::GetParentOrderId() const
{
    return parentOrderId;
}

void HrOrderEntity::SetParentOrderId(TableId parentOrderId)
{
    MemberHelper::SetMember(this->parentOrderId, parentOrderId);
}

/* private functions */

/**********************class HrPlatformPairEntity**********************/
HrPlatformPairEntity::HrPlatformPairEntity(TableId id, shared_ptr<PlatformEntity> platformEntA,
    shared_ptr<PlatformEntity> platformEntB, shared_ptr<double> margin, shared_ptr<double> marginAdjustSize,
    shared_ptr<HrNs::OderPricingMode> opm)
    : id(id), platformEntA(platformEntA),
    platformEntB(platformEntB), margin(margin), marginAdjustSize(marginAdjustSize),
    opm(opm), opmFlag(ZeroOrderPricingMode)
{}

HrPlatformPairEntity::HrPlatformPairEntity()
: HrPlatformPairEntity(InvalidTableId, nullptr, nullptr, nullptr, nullptr, nullptr)
{}

HrPlatformPairEntity::HrPlatformPairEntity(TableId id, shared_ptr<PlatformEntity> platformEntA,
    shared_ptr<PlatformEntity> platformEntB, Rate margin, Rate marginAdjustSize,
    HrNs::OderPricingMode opm)
    : HrPlatformPairEntity(id, platformEntA, platformEntB,
    make_shared(margin), make_shared(marginAdjustSize),
    make_shared<HrNs::OderPricingMode>(opm))
{}

HrPlatformPairEntity::~HrPlatformPairEntity()
{}

TableId HrPlatformPairEntity::GetId() const
{
    return id;
}

void HrPlatformPairEntity::SetId(TableId id)
{
    this->id = id;
}

std::shared_ptr<PlatformEntity> HrPlatformPairEntity::GetPlatformA() const
{
    return platformEntA;
}

void HrPlatformPairEntity::SetPlatformA(std::shared_ptr<PlatformEntity> platformEntA)
{
    this->platformEntA = platformEntA;
}

std::shared_ptr<PlatformEntity> HrPlatformPairEntity::GetPlatformB() const
{
    return platformEntB;
}

void HrPlatformPairEntity::SetPlatformB(std::shared_ptr<PlatformEntity> platformEntB)
{
    this->platformEntB = platformEntB;
}

Rate HrPlatformPairEntity::GetMargin() const
{
    return Rate(*margin);
}

void HrPlatformPairEntity::SetMargin(Rate margin)
{
    MemberHelper::SetMember(this->margin, (double)margin);
}

Rate HrPlatformPairEntity::GetMarginAdjustSize() const
{
    return Rate(*marginAdjustSize);
}

void HrPlatformPairEntity::SetMarginAdjustSize(Rate marginAdjustSize)
{
    MemberHelper::SetMember(this->marginAdjustSize, (double)marginAdjustSize);
}

HrNs::OderPricingMode HrPlatformPairEntity::GetOpm() const
{
    return *opm;
}

void HrPlatformPairEntity::SetOpm(HrNs::OderPricingMode opm)
{
    MemberHelper::SetMember(this->opm, opm);
}

HrNs::OrderPricingModeFlag HrPlatformPairEntity::GetOpmFlag() const
{
    return opmFlag;
}

void HrPlatformPairEntity::SetOpmFlag(HrNs::OrderPricingModeFlag opmFlag)
{
    this->opmFlag = opmFlag;
}

shared_ptr<HrEntity> HrPlatformPairEntity::GetRobot() const
{
    return hrEnt.lock();
}

void HrPlatformPairEntity::SetRobot(shared_ptr<HrEntity> hrEnt)
{
    MemberHelper::SetMember(this->hrEnt, hrEnt, shared_from_this());
}

/* private functions */
/* Bind, Unbind */
void HrPlatformPairEntity::Bind(weak_ptr<HrEntity> hrEnt)
{
    MemberHelper::BindMember(this->hrEnt, hrEnt, shared_from_this());
}

void HrPlatformPairEntity::Unbind(weak_ptr<HrEntity> hrEnt)
{
    MemberHelper::UnbindMember(this->hrEnt, hrEnt, shared_from_this());
}