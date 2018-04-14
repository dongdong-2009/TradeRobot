#ifndef _Entities_h_
#define _Entities_h_

/* C++ */
#include <string>
#include <memory>  /* shared_ptr, weak_ptr, enable_shared_from_this */
#include <list>    /* std::list */
#include <vector>  /* std::list */

#include <odb/lazy-ptr.hxx>

/* Base Type */
#include "Include/Type/BaseType.h"

using std::list;
using std::vector;
using std::set;
using std::shared_ptr;
using std::weak_ptr;
using std::string;

using odb::lazy_shared_ptr;
using odb::lazy_weak_ptr;

#pragma db namespace() pointer(shared_ptr)

class AccountEntity;
class MarketEntity;
class OrderEntity;
class PlatformEntity;
class PlatformApiRuleEntity;
class PlatformCoinRuleEntity;
class RobotEntity;
class TableIdEntity;

/* Start, Hedge Robot */
class HrAccountEntity;
class HrAccountGroupEntity;
class HrAccGrpNormalEntity;
class HrAccGrpOpmTrialEntity;
class HrAccGrpHorzTrialEntity;
class HrEntity;
class HrOrderEntity;
class HrPlatformPairEntity;
/* End, Hedge Robot */

/**********************class TableIdEntity**********************/
#pragma db object table("TableId")
class TableIdEntity
{
public:
    friend class odb::access;

    TableIdEntity(const string& tableName, shared_ptr<TableId> tableId);
    TableIdEntity();
    TableIdEntity(const char *tableName, TableId tableId);
    ~TableIdEntity();

    const string& GetTableName() const;
    void SetTableName(const string& tableName);

    TableId GetTableId() const;
    void SetTableId(TableId tableId);

private:
    /* disable default copy constructor */
    TableIdEntity(const TableIdEntity& right);

#pragma db id column("TableName")
    string tableName;

#pragma db not_null column("TableId")
    shared_ptr<TableId> tableId;
};

/**********************class MarketEntity**********************/
//session(false): to disable seesion support for MarketEntity object.
#pragma db object table("Market")
class MarketEntity : public std::enable_shared_from_this<MarketEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    MarketEntity();
    MarketEntity(TableId id, CoinType coinType, Money bidPrice, Money askPrice, Money lastPrice);
    ~MarketEntity();

    TableId GetId() const;
    void SetId(TableId id);

    CoinType GetCoinType() const;
    void SetCoinType(CoinType coinType);

    Money GetBidPrice() const;
    void SetBidPrice(Money bidPrice);

    Money GetAskPrice() const;
    void SetAskPrice(Money askPrice);

    Money GetLastPrice() const;
    void SetLastPrice(Money lastPrice);

    time_point GetTime() const;
    void SetTime(time_point time);

    shared_ptr<PlatformEntity> GetPlatform() const;
    void SetPlatform(shared_ptr<PlatformEntity> platformEnt);

    /* auxiliary functions */
    TableId GetPlatformId() const;

private:
    /* delegating constructor */
    MarketEntity(TableId id, shared_ptr<CoinType> coinType,
        shared_ptr<double> bidPrice, shared_ptr<double> askPrice, shared_ptr<double> lastPrice);

    /* disable default copy constructor */
    MarketEntity(const MarketEntity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<PlatformEntity> platformEnt);
    void Unbind(weak_ptr<PlatformEntity> platformEnt);

private:
#pragma db id column("Id")
    TableId id;

#pragma db not_null column("CoinType")
    shared_ptr<CoinType> coinType;

#pragma db not_null column("BidPrice")
    shared_ptr<double> bidPrice;

#pragma db not_null column("AskPrice")
    shared_ptr<double> askPrice;

#pragma db not_null column("LastPrice")
    shared_ptr<double> lastPrice;

#pragma db not_null column("Time")
    int64_t time;

#pragma db not_null column("PlatformId")
    weak_ptr<PlatformEntity> platformEnt;
};

/**********************class AccountEntity**********************/
#pragma db object table("Account") polymorphic
class AccountEntity : public std::enable_shared_from_this<AccountEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

	AccountEntity();
    AccountEntity(TableId id, const char *userName,
		const char *publicKey, const char *privateKey,
        Money investedMoney, CoinNumber investedCoinNumber);
	virtual ~AccountEntity();

	TableId GetId() const;
	void SetId(TableId id);

    const string& GetAccountUserName() const;
    void SetAccountUserName(const string& userName);

    const string& GetPublicKey() const;
    void SetPublicKey(const string& publicKey);

    const string& GetPrivateKey() const;
    void SetPrivateKey(const string& privateKey);

    Money GetInvestedMoney() const;
    void SetInvestedMoney(Money investedMoney);

    CoinNumber GetInvestedCoinNumber() const;
    void SetInvestedCoinNumber(CoinNumber investedCoinNumber);

    const string& GetDescription() const;
	void SetDescription(const string& description);

    Money GetAvailableMoney() const;
    void SetAvailableMoney(Money availableMoney);
    void IncAvailableMoney(Money money);
    void DecAvailableMoney(Money money);

    CoinNumber GetAvailableCoinNumber() const;
    void SetAvailableCoinNumber(CoinNumber availableCoinNumber);
    void IncAvailableCoinNumber(CoinNumber coinNumber);
    void DecAvailableCoinNumber(CoinNumber coinNumber);

	const list<lazy_shared_ptr<OrderEntity>>& GetOrders() const;
    list<lazy_shared_ptr<OrderEntity>>& GetOrders();
    void Bind(shared_ptr<OrderEntity> orderEnt);
    void Unbind(shared_ptr<OrderEntity> orderEnt);

    shared_ptr<PlatformEntity> GetPlatform() const;
    void SetPlatform(shared_ptr<PlatformEntity> platformEnt);

    /* auxiliary functions */
    milliseconds GetPlatformApiAverageResponseTime() const;

protected:
    /* delegating constructor */
    AccountEntity(TableId id, shared_ptr<string> userName,
        shared_ptr<string> publicKey, shared_ptr<string> privateKey,
        shared_ptr<double> investedMoney, shared_ptr<double> investedCoinNumber);

private:
    /* disable default copy constructor */
    AccountEntity(const AccountEntity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<PlatformEntity> platformEnt);
    void Unbind(weak_ptr<PlatformEntity> platformEnt);

private:
#pragma db id column("Id")
	TableId id;

#pragma db not_null column("UserName")
	shared_ptr<string> userName;

#pragma db not_null column("PublicKey")
	shared_ptr<string>  publicKey;

#pragma db not_null column("PrivateKey")
	shared_ptr<string>  privateKey;

#pragma db not_null column("InvestedMoney")
    shared_ptr<double> investedMoney;

#pragma db not_null column("InvestedCoinNumber")
    shared_ptr<double> investedCoinNumber;

#pragma db not_null column("Description")
    string description;

#pragma db transient
    shared_ptr<Money> availableMoney;

#pragma db transient
    shared_ptr<CoinNumber> availableCoinNumber;

#pragma db value_not_null inverse(accountEnt)
    list<lazy_shared_ptr<OrderEntity>> orderEnts;

#pragma db not_null column("PlatformId")
    weak_ptr<PlatformEntity> platformEnt;
};

/**********************class OrderEntity**********************/
#pragma db object table("Order") polymorphic
class OrderEntity : public std::enable_shared_from_this<OrderEntity>
{
public:
	friend class odb::access;
	friend class MemberHelper;

	OrderEntity();
	OrderEntity(TableId id, OrderType orderType, Money limitedPrice, CoinNumber coinNumber);
	virtual ~OrderEntity();
    virtual shared_ptr<OrderEntity> Clone() const = 0;

	TableId GetId() const;
    void SetId(TableId id);

    OrderType GetOrderType() const;
    void SetOrderType(OrderType orderType);

    virtual CoinType GetCoinType() const = 0;
    virtual void SetCoinType(CoinType coinType) = 0;

    PlatformOrderId GetPlatformOrderId() const;
    void SetPlatformOrderId(PlatformOrderId platformOrderId);

    Money GetLimitedPrice() const;
    void SetLimitedPrice(Money limitedPrice);

    Money GetConcludedPrice() const;
    void SetConcludedPrice(Money concludedPrice);

    CoinNumber GetCoinNumber() const;
    void SetCoinNumber(CoinNumber coinNumber);

    CoinNumber GetFilledCoinNumber() const;
    void SetFilledCoinNumber(CoinNumber filledCoinNumber);

	time_point GetCreatingTime() const;
	void SetCreatingTime(time_point creatingTime);

	shared_ptr<time_point> GetClosingTime() const;
    void SetClosingTime(time_point closingTime);

	shared_ptr<OrderCanceledReason> GetCanceledReason() const;
    void SetCanceledReason(OrderCanceledReason canceledReason);

    shared_ptr<AccountEntity> GetAccount() const;
    void SetAccount(shared_ptr<AccountEntity> accountEnt);

    virtual shared_ptr<RobotEntity> GetRobot() const = 0;
    virtual void SetRobot(shared_ptr<RobotEntity>& robotEnt) = 0;

    /* auxiliary functions */
    Money GetFreezingAmount() const;    //订单导致冻结的金额.
    Money GetFilledCoinAmount() const;  //实际成交金额
    CoinNumber GetUnfilledCoinNumber() const;
    shared_ptr<PlatformEntity> GetPlatform() const;

protected:
	/* delegating constructor */
	OrderEntity(TableId id, shared_ptr<OrderType> orderType, shared_ptr<PlatformOrderId> platformOrderId,
        shared_ptr<double> limitedPrice, shared_ptr<double> coinNumber);

    /* disable default copy constructor */
    OrderEntity(const OrderEntity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<AccountEntity> accountEnt);
    void Unbind(weak_ptr<AccountEntity> accountEnt);

#pragma db id column("Id")
	TableId id;

#pragma db not_null column("OrderType")
	shared_ptr<OrderType> orderType;

#pragma db not_null column("PlatformOrderId")
	shared_ptr<PlatformOrderId> platformOrderId;

#pragma db not_null column("LimitedPrice")
	shared_ptr<double> limitedPrice;

#pragma db null column("ConcludedPrice")
    shared_ptr<double> concludedPrice;

#pragma db not_null column("CoinNumber")
    shared_ptr<double> coinNumber;

#pragma db not_null column("FilledCoinNumber")
    double filledCoinNumber;

#pragma db not_null column("CreatingTime")
	int64_t creatingTime;

#pragma db null column("ClosingTime")
	shared_ptr<int64_t> closingTime;

#pragma db null column("CanceledReason")
	shared_ptr<OrderCanceledReason> canceledReason;

#pragma db not_null column("AccountId")
    weak_ptr<AccountEntity> accountEnt;
};

/**********************class PlatformEntity**********************/
#pragma db object table("Platform")
class PlatformEntity : public std::enable_shared_from_this<PlatformEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    PlatformEntity();
	PlatformEntity(TableId id, const char *name,
        milliseconds minMarketQueryInvl, milliseconds minOrderQueryInvl,
        milliseconds minTradeInvl, Rate cashWithdrawalFeeRate,
        milliseconds apiAverageResponseTime);
	~PlatformEntity();

	TableId GetId() const;
	void SetId(TableId id);

	const string& GetName() const;
    void SetName(const string& name);

    milliseconds GetMinMarketQueryInvl() const;
    void SetMinMarketQueryInvl(milliseconds minMarketQueryInvl);

    milliseconds GetMinOrderQueryInvl() const;
    void SetMinOrderQueryInvl(milliseconds minOrderQueryInvl);

    milliseconds GetMinTradeInvl() const;
    void SetMinTradeInvl(milliseconds minTradeInvl);

    Rate GetCashWithdrawalFeeRate() const;
    void SetCashWithdrawalFeeRate(Rate cashWithdrawalFeeRate);

    milliseconds GetApiAverageResponseTime() const;
    void SetApiAverageResponseTime(milliseconds apiAverageResponseTime);

	const string& GetDescription() const;
	void SetDescription(const string& description);

    const list<shared_ptr<PlatformApiRuleEntity>>& GetApiRules() const;
    list<shared_ptr<PlatformApiRuleEntity>>& GetApiRules();
    void Bind(shared_ptr<PlatformApiRuleEntity> apiRuleEnt);
    void Unbind(shared_ptr<PlatformApiRuleEntity> apiRuleEnt);

	const list<shared_ptr<PlatformCoinRuleEntity>>& GetCoinRules() const;
	list<shared_ptr<PlatformCoinRuleEntity>>& GetCoinRules();
    void Bind(shared_ptr<PlatformCoinRuleEntity> coinRuleEnt);
    void Unbind(shared_ptr<PlatformCoinRuleEntity> coinRuleEnt);

    const list<shared_ptr<AccountEntity>>& GetAccounts() const;
    list<shared_ptr<AccountEntity>>& GetAccounts();
    void Bind(shared_ptr<AccountEntity> accountEnt);
    void Unbind(shared_ptr<AccountEntity> accountEnt);

    const list<lazy_shared_ptr<MarketEntity>>& GetMarkets() const;
    list<lazy_shared_ptr<MarketEntity>>& GetMarkets();
    void Bind(shared_ptr<MarketEntity> marketEnt);
    void Unbind(shared_ptr<MarketEntity> marketEnt);

    /* auxiliary functions */
    shared_ptr<PlatformApiRuleEntity> GetApiRule(ApiType apiType) const;
    shared_ptr<PlatformCoinRuleEntity> GetCoinRule(CoinType coinType) const;

    shared_ptr<MarketEntity> GetLastMarket();

private:
    /* delegating constructor */
    PlatformEntity(TableId id, shared_ptr<string> name,
        shared_ptr<int64_t> minMarketQueryInvl, shared_ptr<int64_t> minOrderQueryInvl,
        shared_ptr<int64_t> minTradeInvl, shared_ptr<double> cashWithdrawalFeeRate,
        shared_ptr<int64_t> apiAverageResponseTime);

    /* disable default copy constructor */
    PlatformEntity(const PlatformEntity& right);

private:
#pragma db id column("Id")
	TableId id;

#pragma db not_null column("Name")
	shared_ptr<string> name; //platform name: "btctrade.com", "okcoin.cn"

#pragma db not_null column("MinMarketQueryInterval")
    shared_ptr<int64_t> minMarketQueryInvl;

#pragma db not_null column("MinOrderQueryInterval")
    shared_ptr<int64_t> minOrderQueryInvl;

#pragma db not_null column("MinTradeInterval")
    shared_ptr<int64_t> minTradeInvl;

#pragma db not_null column("CashWithdrawalFeeRate")
    shared_ptr<double> cashWithdrawalFeeRate;

#pragma db not_null column("ApiAverageResponseTime")
    shared_ptr<int64_t> apiAverageResponseTime;

#pragma db not_null column("Description")
	string description;

#pragma db value_not_null inverse(platformEnt)
    list<shared_ptr<PlatformApiRuleEntity>> apiRuleEnts;

#pragma db value_not_null inverse(platformEnt)
    list<shared_ptr<PlatformCoinRuleEntity>> coinRuleEnts;

#pragma db value_not_null inverse(platformEnt)
    list<shared_ptr<AccountEntity>> accountEnts;

#pragma db value_not_null inverse(platformEnt)
    list<lazy_shared_ptr<MarketEntity>> marketEnts;
};

/**********************class PlatformApiRuleEntity**********************/
#pragma db object table("PlatformApiRule")
class PlatformApiRuleEntity : public std::enable_shared_from_this<PlatformApiRuleEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    PlatformApiRuleEntity();
    PlatformApiRuleEntity(TableId id, ApiType apiType, const char *url, seconds curlTimeout);
    ~PlatformApiRuleEntity();

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<PlatformEntity> GetPlatform() const;
    void SetPlatform(shared_ptr<PlatformEntity> platformEnt);

    ApiType GetApiType() const;
    void SetApiType(ApiType apiType);

    const string& GetUrl() const;
    void SetUrl(const string& url);

    seconds GetCurlTimeout() const;
    void SetCurlTimeout(seconds curlTimeout);

private:
    /* delegating constructor */
    PlatformApiRuleEntity(TableId id, shared_ptr<ApiType> apiType, shared_ptr<string> url,
        shared_ptr<int64_t> curlTimeout);

    /* disable default copy constructor */
    PlatformApiRuleEntity(const PlatformApiRuleEntity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<PlatformEntity> platformEnt);
    void Unbind(weak_ptr<PlatformEntity> platformEnt);

private:
#pragma db id column("Id")
    TableId id;

#pragma db not_null column("PlatformId")
    weak_ptr<PlatformEntity> platformEnt;

#pragma db not_null column("ApiType")
    shared_ptr<ApiType> apiType;

#pragma db not_null column("Url")
    shared_ptr<string> url;

#pragma db not_null column("CurlTimeout")
    shared_ptr<int64_t> curlTimeout;  /* curl timeout */
};

/**********************class PlatformCoinRuleEntity**********************/
#pragma db object table("PlatformCoinRule")
class PlatformCoinRuleEntity : public std::enable_shared_from_this<PlatformCoinRuleEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    PlatformCoinRuleEntity();
    PlatformCoinRuleEntity(TableId id, CoinType coinType, CoinNumber minTradingUnit,
        Rate transactionFeeRate, ChargeType coinWithdrawalChargeType, double coinWithdrawalPara,
        Money minPriceSize);
    ~PlatformCoinRuleEntity();

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<PlatformEntity> GetPlatform() const;
    void SetPlatform(shared_ptr<PlatformEntity> platformEnt);

    CoinType GetCoinType() const;
    void SetCoinType(CoinType coinType);

    CoinNumber GetMinTradingUnit() const;
    void SetMinTradingUnit(CoinNumber minTradingUnit);

    Rate GetTransactionFeeRate() const;
    void SetTransactionFeeRate(Rate transactionFeeRate);

    ChargeType GetCoinWithdrawalChargeType() const;
    void SetCoinWithdrawalChargeType(ChargeType coinWithdrawalChargeType);

    double GetCoinWithdrawalPara() const;
    void SetCoinWithdrawalPara(double coinWithdrawalPara);

    Money GetMinPriceSize() const;
    void SetMinPriceSize(Money minPriceSize);

private:
    /* delegating constructor */
    PlatformCoinRuleEntity(TableId id, shared_ptr<CoinType> coinType,
        shared_ptr<double> minTradingUnit, shared_ptr<double> transactionFeeRate,
        shared_ptr<ChargeType> coinWithdrawalChargeType, shared_ptr<double> coinWithdrawalPara,
        shared_ptr<double> minPriceSize);

    /* disable default copy constructor */
    PlatformCoinRuleEntity(const PlatformCoinRuleEntity& right);

    /* odb::access special functions */
    /* Bind, Unbind */
    void Bind(weak_ptr<PlatformEntity> platformEnt);
    void Unbind(weak_ptr<PlatformEntity> platformEnt);

private:
#pragma db id column("Id")
    TableId id;

#pragma db not_null column("PlatformId")
    weak_ptr<PlatformEntity> platformEnt;

#pragma db not_null column("CoinType")
    shared_ptr<CoinType> coinType;

#pragma db not_null column("MinTradingUnit")
    shared_ptr<double> minTradingUnit;

#pragma db not_null column("TransactionFeeRate")
    shared_ptr<double> transactionFeeRate;

#pragma db not_null column("CoinWithdrawalChargeType")
    shared_ptr<ChargeType> coinWithdrawalChargeType;

#pragma db not_null column("CoinWithdrawalPara")
    shared_ptr<double> coinWithdrawalPara;

#pragma db not_null column("MinPriceSize")
    /* minPriceSize: 如果按照 AskPrice 下买单, 为了立即成交, 我们使用 AskPrice + minPriceSize 作为买价.
     * 通常, minPriceSize 应该 <= 成交价格 * 0.0001
     */
    shared_ptr<double> minPriceSize;
};

/**********************class RobotEntity**********************/
#pragma db object table("Robot") polymorphic
class RobotEntity : public std::enable_shared_from_this<RobotEntity>
{
public:
    friend class odb::access;

	RobotEntity();
	RobotEntity(TableId id, RobotCfgState robotCfgState);
	virtual ~RobotEntity();

	TableId GetId() const;
	void SetId(TableId id);

	virtual const char* GetName() const = 0;

    RobotCfgState GetRobotCfgState() const;
    void SetRobotCfgState(RobotCfgState robotCfgState);

    const string& GetDescription() const;
    void SetDescription(const string& description);

protected:
    /* delegating constructor */
    RobotEntity(TableId id, shared_ptr<RobotCfgState> robotCfgState);

    /* disable default copy constructor */
    RobotEntity(const RobotEntity& right);

private:
#pragma db id column("Id")
	TableId id;

#pragma db not_null column("RobotCfgState")
	shared_ptr<RobotCfgState> robotCfgState;

#pragma db not_null column("Description")
    string description;
};

/***********************************************************************
************************  Hedge Robot Entity   *************************
***********************************************************************/

/**********************class HrAccountEntity**********************/
#pragma db object table("HedgeRobotAccount")
class HrAccountEntity : public AccountEntity
{
public:
    friend class odb::access;
    friend class MemberHelper;

	HrAccountEntity();
    HrAccountEntity(TableId id, const char *userName,
        const char *publicKey, const char *privateKey,
        Money investedMoney, CoinNumber investedCoinNumber);
	~HrAccountEntity();

    shared_ptr<HrAccountGroupEntity> GetHrAccountGroup() const;
    void SetHrAccountGroup(shared_ptr<HrAccountGroupEntity> hrAccountGroupEnt);

    /* overwritted functions, 基类的Bind()被屏蔽了， 为了确保Bind(shared_ptr<OrderEntity> orderEnt)
     * 能够被成功调用， 在此显式定义被屏蔽了的函数。（否则），调用者需要在调用函数前将变量类型从
     * 转换为(HrAccountEntity*), (AccountEntity*)。
    */
    void Bind(shared_ptr<OrderEntity> orderEnt) { AccountEntity::Bind(orderEnt); }
    void Unbind(shared_ptr<OrderEntity> orderEnt) { AccountEntity::Unbind(orderEnt); }

    /* auxiliary functions */

private:
    /* delegating constructor */
    HrAccountEntity(TableId id, shared_ptr<string> userName,
        shared_ptr<string> publicKey, shared_ptr<string> privateKey,
        shared_ptr<double> investedMoney, shared_ptr<double> investedCoinNumber);

    /* disable default copy constructor */
    HrAccountEntity(const HrAccountEntity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<HrAccountGroupEntity> hrAccountGroupEnt);
    void Unbind(weak_ptr<HrAccountGroupEntity> hrAccountGroupEnt);

private:
#pragma db not_null column("HedgeRobotAccountGroupId")
    weak_ptr<HrAccountGroupEntity> hrAccountGroupEnt;
};

/**********************class HrAccountGroupEntity**********************/
#pragma db object table("HedgeRobotAccountGroup") polymorphic
class HrAccountGroupEntity : public std::enable_shared_from_this<HrAccountGroupEntity>
{
public:
	friend class odb::access;
    friend class MemberHelper;

    HrAccountGroupEntity(TableId id);
    HrAccountGroupEntity();
    virtual ~HrAccountGroupEntity();

    TableId GetId() const;
    void SetId(TableId id);

    virtual HrNs::AccountGroupMln GetAccGrpMln() const = 0;
    virtual void SetAccGrpMln(HrNs::AccountGroupMln accGrpMln) = 0;

    const string& GetState() const;
    void SetState(const string& state);

    const string& GetDescription() const;
    void SetDescription(const string& description);

	shared_ptr<HrEntity> GetRobot() const;
    void SetRobot(shared_ptr<HrEntity> hrEnt);

    /* 为了简化 HrAccGrpAlgorithm 的实现, 我们需要将2个HrAccountEntity 排序，
       hrAccountEnt0.GetPlatformApiAverageResponseTime() < hrAccountEnt1.GetPlatformApiAverageResponseTime()。
       注：不能将这2个HrAccountEntity 放入list等有序容器中， 因为HrAccountEntity在数据库中是按照id排序的。
       我们也不能将这2个HrAccountEntity 放入set， map等odb无序容器中， 因为database::load() 很多时候
       会先将对象insert到set， 然后再修改对象的值， 最终导致set内的对象处于无序的状态。
    */
    shared_ptr<HrAccountEntity> GetHrAccount0() const;
    shared_ptr<HrAccountEntity> GetHrAccount1() const;
    void Bind(shared_ptr<HrAccountEntity> hrAccountEnt);
    void Unbind(shared_ptr<HrAccountEntity> hrAccountEnt);

private:
    /* disable default copy constructor */
    HrAccountGroupEntity(const HrAccountGroupEntity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<HrEntity> hrEnt);
    void Unbind(weak_ptr<HrEntity> hrEnt);

private:
#pragma db id column("Id")
    TableId id;

    /* 当前 Hr Account Group 的状态, 此字段仅用于单元测试 */
#pragma db transient
    string state;

#pragma db not_null column("Description")
    string description;

#pragma db not_null column("HedgeRobotId")
    weak_ptr<HrEntity> hrEnt;

#pragma db not_null column("HrAccount0Id")
    shared_ptr<HrAccountEntity> hrAccountEnt0;

#pragma db not_null column("HrAccount1Id")
    shared_ptr<HrAccountEntity> hrAccountEnt1;
};

/**********************class HrAccGrpNormalEntity**********************/
#pragma db object table("HedgeRobotAccountGroupNormal")
class HrAccGrpNormalEntity : public HrAccountGroupEntity
{
public:
	friend class odb::access;
    friend class MemberHelper;

    HrAccGrpNormalEntity(TableId id, shared_ptr<HrNs::AccountGroupMln> accountGroupMln);
    HrAccGrpNormalEntity();
    HrAccGrpNormalEntity(TableId id, HrNs::AccountGroupMln accGrpMln);
    ~HrAccGrpNormalEntity();

	HrNs::AccountGroupMln GetAccGrpMln() const;
    void SetAccGrpMln(HrNs::AccountGroupMln accGrpMln);

    shared_ptr<HrAccGrpOpmTrialEntity> GetAccGrpOpmTrial() const;
    void SetAccGrpOpmTrial(shared_ptr<HrAccGrpOpmTrialEntity> accGrpOpmTrialEnt);

    shared_ptr<HrAccGrpHorzTrialEntity> GetAccGrpHorzTrial() const;
    void SetAccGrpHorzTrial(shared_ptr<HrAccGrpHorzTrialEntity> accGrpHorzTrialEnt);

    /* overwritted functions, 基类的Bind()被屏蔽了， 为了确保Bind(shared_ptr<HrAccountEntity> hrAccountEnt)
    * 能够被成功调用， 在此显式定义被屏蔽了的函数。（否则），调用者需要在调用函数前将变量类型从
    * 转换为(HrAccountEntity*), (AccountEntity*)。
    */
    void Bind(shared_ptr<HrAccountEntity> hrAccountEnt);
    void Unbind(shared_ptr<HrAccountEntity> hrAccountEnt);

private:
    /* disable default copy constructor */
    HrAccGrpNormalEntity(const HrAccGrpNormalEntity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<HrAccGrpOpmTrialEntity> accGrpOpmTrialEnt);
    void Unbind(weak_ptr<HrAccGrpOpmTrialEntity> accGrpOpmTrialEnt);

    void Bind(weak_ptr<HrAccGrpHorzTrialEntity> accGrpHorzTrialEnt);
    void Unbind(weak_ptr<HrAccGrpHorzTrialEntity> accGrpHorzTrialEnt);

private:
#pragma db not_null column("AccountGroupMln")
    shared_ptr<HrNs::AccountGroupMln> accGrpMln;

#pragma db null inverse(accGrpNormalEnt)
    weak_ptr<HrAccGrpOpmTrialEntity> accGrpOpmTrialEnt;

#pragma db null inverse(accGrpNormalEnt)
    weak_ptr<HrAccGrpHorzTrialEntity> accGrpHorzTrialEnt;
};

/**********************class HrAccGrpHorzTrialEntity**********************/
#pragma db object table("HedgeRobotAccountGroupHorzTrial")
class HrAccGrpHorzTrialEntity : public HrAccountGroupEntity
{
public:
	friend class odb::access;
    friend class MemberHelper;

    HrAccGrpHorzTrialEntity();
    HrAccGrpHorzTrialEntity(TableId id);
    ~HrAccGrpHorzTrialEntity();

	HrNs::AccountGroupMln GetAccGrpMln() const;
    void SetAccGrpMln(HrNs::AccountGroupMln accGrpMln);

    shared_ptr<HrAccGrpNormalEntity> GetAccGrpNormal() const;
    void SetAccGrpNormal(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt);

private:
    /* disable default copy constructor */
    HrAccGrpHorzTrialEntity(const HrAccGrpHorzTrialEntity& right);

    /* Bind, Unbind */
    void Bind(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt);
    void Unbind(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt);

private:
#pragma db not_null column("AccountGroupNormalId")
    shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt;
};

/**********************class HrAccGrpOpmTrialEntity**********************/
/* Opm: Order Pricing Mode */
#pragma db object table("HedgeRobotAccountGroupOpmTrial")
class HrAccGrpOpmTrialEntity : public HrAccountGroupEntity
{
public:
	friend class odb::access;
    friend class MemberHelper;

    HrAccGrpOpmTrialEntity();
    HrAccGrpOpmTrialEntity(TableId id);
    ~HrAccGrpOpmTrialEntity();

	HrNs::AccountGroupMln GetAccGrpMln() const;
    void SetAccGrpMln(HrNs::AccountGroupMln accGrpMln);

	shared_ptr<HrAccGrpNormalEntity> GetAccGrpNormal() const;
    void SetAccGrpNormal(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt);

private:
    /* Bind, Unbind */
    void Bind(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt);
    void Unbind(shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt);

private:
#pragma db not_null column("AccountGroupNormalId")
    shared_ptr<HrAccGrpNormalEntity> accGrpNormalEnt;
};

/**********************class HrEntity**********************/
#pragma db object table("HedgeRobot")
class HrEntity : public RobotEntity
{
public:
    friend class odb::access;
    friend class MemberHelper;

	HrEntity();
	HrEntity(TableId id, RobotCfgState robotCfgState, CoinType coinType,
        seconds horzMarginAdjustInvl, seconds vertMarginAdjustInvl,
        seconds opmAdjustInvl, CoinNumber minTradingUnit, Rate stopLossAdjustSize,
        seconds order2ndTimeOut, seconds stopLossOrderTimeOut);
	~HrEntity();
    const char* GetName() const;

    CoinType GetCoinType() const;
    void SetCoinType(CoinType coinType);

    seconds GetHorzMarginAdjustInvl() const;
    void SetHorzMarginAdjust(seconds horzMarginAdjustInvl);

    seconds GetVertMarginAdjustInvl() const;
    void SetVertMarginAdjustInvl(seconds vertMarginAdjustInvl);

    seconds GetOpmAdjustInvl() const;
    void SetOpmAdjustInvl(seconds opmAdjustInvl);

    CoinNumber GetMinTradingUnit() const;
    void SetMinTradingUnit(CoinNumber minTradingUnit);

    Rate GetStopLossAdjustSize() const;
    void SetStopLossAdjustSize(Rate stopLossAdjustSize);

    seconds GetOrder2ndTimeOut() const;
    void SetOrder2ndTimeOut(seconds stopLossOrderTimeOut);

    seconds GetStopLossOrderTimeOut() const;
    void SetStopLossOrderTimeOut(seconds stopLossOrderTimeOut);

    const list<shared_ptr<HrPlatformPairEntity>>& GetPlatformPairs() const;
    list<shared_ptr<HrPlatformPairEntity>>& GetPlatformPairs();
    void Bind(shared_ptr<HrPlatformPairEntity> platformPairEnt);
    void Unbind(shared_ptr<HrPlatformPairEntity> platformPairEnt);

    const list<shared_ptr<HrAccountGroupEntity>>& GetAccountGroups() const;
    list<shared_ptr<HrAccountGroupEntity>>& GetAccountGroups();
    void Bind(shared_ptr<HrAccountGroupEntity> accountGroupEnt);
    void Unbind(shared_ptr<HrAccountGroupEntity> accountGroupEnt);

    /* auxiliary functions */
    shared_ptr<HrPlatformPairEntity> GetPlatformPair(TableId ptmAId, TableId ptmBId);
    shared_ptr<HrAccountGroupEntity> GetAccountGroup(TableId ptmAId, TableId ptmBId);

private:
    /* delegating constructor */
    HrEntity(TableId id, shared_ptr<RobotCfgState> robotCfgState, shared_ptr<CoinType> coinType,
        shared_ptr<int64_t> horzMarginAdjustInvl, shared_ptr<int64_t> vertMarginAdjustInvl,
        shared_ptr<int64_t> opmAdjustInvl, shared_ptr<double> minTradingUnit, shared_ptr<double> stopLossAdjustSize,
        shared_ptr<int64_t> order2ndTimeOut, shared_ptr<int64_t> stopLossOrderTimeOut);

    /* disable default copy constructor */
    HrEntity(const HrEntity& right);

private:
	bool IsHorzMarginAdjustInvlValid(int64_t horzMarginAdjustInvl);
    bool IsVertMarginAdjustInvlValid(int64_t vertMarginAdjustInvl);
    bool IsOpmAdjustInvlValid(int64_t opmAdjustInvl);

private:
#pragma db not_null column("CoinType")
	shared_ptr<CoinType> coinType;

#pragma db not_null column("HorzMarginAdjustInvl")
    shared_ptr<int64_t> horzMarginAdjustInvl;

#pragma db not_null column("VertMarginAdjustInvl")
    shared_ptr<int64_t> vertMarginAdjustInvl;

	/* Opm: Order Pricing Mode,   Invl: Interval*/
#pragma db not_null column("OpmAdjustInvl")
    shared_ptr<int64_t> opmAdjustInvl;

#pragma db not_null column("MinTradingUnit")
	shared_ptr<double> minTradingUnit;

#pragma db not_null column("StopLossAdjustSize")
    shared_ptr<double> stopLossAdjustSize;

    /* step1 buying/selling order 在等待 order2ndTimeOut seconds 后将变为stop loss order */
#pragma db not_null column("Order2ndTimeOut")
    shared_ptr<int64_t> order2ndTimeOut;

    /* stop loss order 在等待 order2ndTimeOut seconds 后将按照新的价格重新挂单 */
#pragma db not_null column("StopLossOrderTimeOut")
    shared_ptr<int64_t> stopLossOrderTimeOut;

#pragma db value_not_null inverse(hrEnt)
    list<shared_ptr<HrPlatformPairEntity>> platformPairEnts;

    /* use if (typeid(*d1) == typeid(D1)) to compare sub-class type */
#pragma db value_not_null inverse(hrEnt)
    list<shared_ptr<HrAccountGroupEntity>> accountGroupEnts;
};

/**********************class HrOrderEntity**********************/
#pragma db object table("HedgeRobotOrder")
class HrOrderEntity : public OrderEntity
{
public:
    friend class odb::access;
    friend class MemberHelper;

    HrOrderEntity();
    HrOrderEntity(const HrOrderEntity& right);  /* only for Unit Test */
    HrOrderEntity(TableId id, OrderType orderType, Money limitedPrice, CoinNumber coinNumber);
    ~HrOrderEntity();
    shared_ptr<OrderEntity> Clone() const;

    CoinType GetCoinType() const;                    /* virtual */
    void SetCoinType(CoinType coinType);             /* virtual */

    shared_ptr<RobotEntity> GetRobot() const;        /* virtual */
    void SetRobot(shared_ptr<RobotEntity>& robotEnt);/* virtual */

    shared_ptr<time_point> GetPriceScheduleTime() const;
    void SetPriceScheduleTime(time_point priceScheduleTime);

    shared_ptr<TableId> GetParentOrderId() const;
    void SetParentOrderId(TableId parentOrderId);

    /*auxiliary functions */


private:
    /* delegating constructor */
    HrOrderEntity(TableId id, shared_ptr<OrderType> orderType,
        shared_ptr<PlatformOrderId> platformOrderId, shared_ptr<double> limitedPrice, shared_ptr<double> coinNumber,
        shared_ptr<int64_t> priceScheduleTime, shared_ptr<TableId> parentOrderId);

private:
#pragma db null column("PriceAdjustScheduleTime")
    shared_ptr<int64_t> priceScheduleTime;

#pragma db null column("ParentOrderId")
    shared_ptr<TableId> parentOrderId;
};

/**********************class HrPlatformPairEntity**********************/
#pragma db object table("HedgeRobotPlatformPair")
class HrPlatformPairEntity : public std::enable_shared_from_this<HrPlatformPairEntity>
{
public:
    friend class odb::access;
    friend class MemberHelper;

    HrPlatformPairEntity();
    HrPlatformPairEntity(TableId id, shared_ptr<PlatformEntity> platformEntA,
        shared_ptr<PlatformEntity> platformEntB, Rate margin, Rate marginAdjustSize,
        HrNs::OderPricingMode opm);
    virtual ~HrPlatformPairEntity();

    TableId GetId() const;
    void SetId(TableId id);

    shared_ptr<PlatformEntity> GetPlatformA() const;
    void SetPlatformA(shared_ptr<PlatformEntity> platformEntA);

    shared_ptr<PlatformEntity> GetPlatformB() const;
    void SetPlatformB(shared_ptr<PlatformEntity> platformEntB);

    Rate GetMargin() const;
    void SetMargin(Rate margin);

    Rate GetMarginAdjustSize() const;
    void SetMarginAdjustSize(Rate marginAdjustSize);

    HrNs::OderPricingMode GetOpm() const;
    void SetOpm(HrNs::OderPricingMode opm);

    HrNs::OrderPricingModeFlag GetOpmFlag() const;
    void SetOpmFlag(HrNs::OrderPricingModeFlag opmFlag);

    shared_ptr<HrEntity> GetRobot() const;
    void SetRobot(shared_ptr<HrEntity> hrEnt);

    /*auxiliary functions */

private:
    HrPlatformPairEntity(TableId id, shared_ptr<PlatformEntity> platformEntA, shared_ptr<PlatformEntity> platformEntB,
        shared_ptr<double> margin, shared_ptr<double> marginAdjustSize,
        shared_ptr<HrNs::OderPricingMode> opm);

    /* disable default copy constructor */
    HrPlatformPairEntity(const HrPlatformPairEntity& right);

    /* Bind, Unbind */
    void Bind(weak_ptr<HrEntity> hrEnt);
    void Unbind(weak_ptr<HrEntity> hrEnt);

private:
#pragma db id column("Id")
    TableId id;

#pragma db not_null column("PlatformIdA")
    shared_ptr<PlatformEntity> platformEntA;

#pragma db not_null column("PlatformIdB")
    shared_ptr<PlatformEntity> platformEntB;

#pragma db not_null column("Margin")
    shared_ptr<double> margin;

#pragma db not_null column("MarginAdjustSize")
    shared_ptr<double> marginAdjustSize;

#pragma db not_null column("OderPricingMode")
    shared_ptr<HrNs::OderPricingMode> opm;

#pragma db not_null column("OderPricingModeFlag")
    HrNs::OrderPricingModeFlag opmFlag;

#pragma db not_null column("HedgeRobotId")
    weak_ptr<HrEntity> hrEnt;
};

#endif