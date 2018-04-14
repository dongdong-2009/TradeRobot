#include "XmlWrapper.h"

/* libxml */
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

/* ODB */
#include <odb/database.hxx>    //odb::database
#include <odb/session.hxx>
#include <odb/transaction.hxx>

/* Foundation */
#include "Include/Module/Module.h"
#include "Include/Foundation/SystemInclude.h"
#include "Include/Foundation/Database.h"
#include "Include/Foundation/XmlHelper.h"
#include "Include/Foundation/Debug.h"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"
#include "Include/Entity/EntityQueryType.h"

/* Functions */
#include "Include/Functions/EnumHelperInterface.h"
#include "Include/Functions/TableIndexHelperInterface.h"

#ifdef _WIN32
#	pragma comment(lib, "libxml2.lib")
#endif

using namespace std;
using namespace std::placeholders;

using std::chrono::seconds;
using std::chrono::milliseconds;

XmlWrapperInterface& XmlWrapperInterface::GetInstance()
{
    /* In C++11, the following is guaranteed to perform thread-safe initialisation: */
    static XmlWrapper instance;
    return instance;
}

/**********************class XmlWrapper**********************/
void XmlWrapper::Read(const char *xmlPath)
{
    EnumHelperInterface& enumHelper = EnumHelperInterface::GetInstance();

    assert((access(xmlPath, 0)) == 0);
    std::shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
	std::shared_ptr<odb::session> session = Database::GetInstance().GetSession();

    auto docDeleter = [](xmlDoc* ptr){ xmlFreeDoc(ptr); };
    shared_ptr<xmlDoc> doc(xmlParseFile(xmlPath), docDeleter);
    assert(doc != nullptr);

    xmlNodePtr node = xmlDocGetRootElement(doc.get());
    auto xpathCtxDeleter = [](xmlXPathContext* ptr){ xmlXPathFreeContext(ptr); };
    shared_ptr<xmlXPathContext> xpathCtx(xmlXPathNewContext(doc.get()), xpathCtxDeleter);
    assert(xpathCtx != nullptr);

    /* "Root" node */
    string strAttr = GetXmlAttrValue<std::string>(node, (const xmlChar*)"ClearAllTables");
    bool allTablesCleared = (strAttr.compare("true") == 0);
    if (allTablesCleared)
    {
        ClearAllTables();
    }

    /* Platforms node */
    xmlChar *xpathExpr = (xmlChar*)"/Root/Platforms/Platform";
    auto xpathObjectDeleter = [](xmlXPathObject* ptr){ xmlXPathFreeObject(ptr); };
    shared_ptr<xmlXPathObject> xpathObj(xmlXPathEvalExpression(xpathExpr, xpathCtx.get()), xpathObjectDeleter);
    xmlNodeSetPtr nodes = xpathObj->nodesetval;
    assert(nodes != nullptr);

    map<string, shared_ptr<PlatformEntity>> ptmEnts;
    for (int i = 0; i < nodes->nodeNr; ++i)
    {
        node = nodes->nodeTab[i];

        TableId id = GetXmlAttrValue<TableId>(node, (const xmlChar*)"Id");
        string name = GetXmlAttrValue<string>(node, (const xmlChar*)"Name");

        node = xmlFirstElementChild(node); /* MinMarketQueryInterval */
        milliseconds minMarketQueryInvl(GetXmlContent<uint32_t>(node));

        node = xmlNextElementSibling(node); /* MinOrderQueryInterval */
        milliseconds minOrderQueryInvl(GetXmlContent<uint32_t>(node));

        node = xmlNextElementSibling(node); /* MinTradeInterval */
        milliseconds minTradeInvl(GetXmlContent<uint32_t>(node));

        node = xmlNextElementSibling(node); /* CashWithdrawalFeeRate */
        Rate cashWithdrawalFeeRate(GetXmlContent<double>(node));

        node = xmlNextElementSibling(node); /* ApiAverageResponseTime */
        milliseconds apiAverageResponseTime(GetXmlContent<uint32_t>(node));

        auto ptmEnt = make_shared<PlatformEntity>(id, name.c_str(), minMarketQueryInvl, minOrderQueryInvl, minTradeInvl,
            cashWithdrawalFeeRate, apiAverageResponseTime);

        node = xmlNextElementSibling(node); /* ApiRules */
        for (xmlNodePtr apiRule = xmlFirstElementChild(node); apiRule != nullptr; apiRule = xmlNextElementSibling(apiRule))
        {
            TableId id = GetXmlAttrValue<TableId>(apiRule, (const xmlChar*)"Id");
            ApiType apiType = enumHelper.GetApiType(GetXmlAttrValue<string>(apiRule, (const xmlChar*)"ApiType").c_str());

            xmlNodePtr child = xmlFirstElementChild(apiRule);
            string url = GetXmlContent<string>(child);
            child = xmlNextElementSibling(child);
            seconds curlTimeout(GetXmlContent<uint32_t>(child));

            auto apiRuleEnt = make_shared<PlatformApiRuleEntity>(id, apiType, url.c_str(), curlTimeout);
            ptmEnt->Bind(apiRuleEnt);
        }

        node = xmlNextElementSibling(node); /* CoinRules */
        for (xmlNodePtr coinRule = xmlFirstElementChild(node); coinRule != nullptr; coinRule = xmlNextElementSibling(coinRule))
        {
            TableId id = GetXmlAttrValue<TableId>(coinRule, (const xmlChar*)"Id");
            CoinType coinType = enumHelper.GetCoinType(GetXmlAttrValue<string>(coinRule, (const xmlChar*)"CoinType").c_str());

            xmlNodePtr child = xmlFirstElementChild(coinRule);
            CoinNumber minTradingUnit(GetXmlContent<double>(child));

            child = xmlNextElementSibling(child);
            Rate transactionFeeRate(GetXmlContent<double>(child));

            child = xmlNextElementSibling(child);
            ChargeType coinWithdrawalParaChargeType = enumHelper.GetChargeType(GetXmlContent<string>(child).c_str());

            child = xmlNextElementSibling(child);
            double coinWithdrawalPara = GetXmlContent<double>(child);

            child = xmlNextElementSibling(child);
            Money minPriceSize(GetXmlContent<double>(child));

            auto coinRuleEnt = make_shared<PlatformCoinRuleEntity>(id, coinType, minTradingUnit,
                transactionFeeRate, coinWithdrawalParaChargeType, coinWithdrawalPara, minPriceSize);
            ptmEnt->Bind(coinRuleEnt);
        }

        ptmEnts.insert(make_pair(ptmEnt->GetName(), ptmEnt));
    }

    /* HedgeRobots node */
    xpathExpr = (xmlChar*)"/Root/HedgeRobots/HedgeRobot";
    xpathObj.reset(xmlXPathEvalExpression(xpathExpr, xpathCtx.get()), xpathObjectDeleter);
    nodes = xpathObj->nodesetval;
    assert(nodes != nullptr);

	list<shared_ptr<HrEntity>> hrEnts;
    for (int i = 0; i < nodes->nodeNr; ++i)
    {
        node = nodes->nodeTab[i];
        TableId id = GetXmlAttrValue<TableId>(node, (const xmlChar*)"Id");
        RobotCfgState robtCfgState = enumHelper.GetRobotCfgState(GetXmlAttrValue<string>(node, (const xmlChar*)"CfgState").c_str());
        CoinType coinType = enumHelper.GetCoinType(GetXmlAttrValue<string>(node, (const xmlChar*)"CoinType").c_str());

        node = xmlFirstElementChild(node);
        CoinNumber minTradingUnit(GetXmlContent<double>(node));

        node = xmlNextElementSibling(node);
        Rate stopLossAdjustSize(GetXmlContent<double>(node));

        node = xmlNextElementSibling(node);
        seconds horzMarginAdjustInvl(GetXmlContent<uint32_t>(node));

        node = xmlNextElementSibling(node);
        seconds vertMarginAdjustInvl(GetXmlContent<uint32_t>(node));

        node = xmlNextElementSibling(node);
        seconds opmAdjustInvl(GetXmlContent<uint32_t>(node)); /* OpmAdjustInvl */

        node = xmlNextElementSibling(node);
        seconds order2ndTimeOut(GetXmlContent<uint32_t>(node)); /* Order2ndTimeOut */

        node = xmlNextElementSibling(node);
        seconds stopLossOrderTimeOut(GetXmlContent<uint32_t>(node)); /* StopLossOrderTimeOut */

        auto hrEnt = make_shared<HrEntity>(id, robtCfgState, coinType,
            horzMarginAdjustInvl, vertMarginAdjustInvl,
            opmAdjustInvl, minTradingUnit, stopLossAdjustSize,
            order2ndTimeOut, stopLossOrderTimeOut);
		hrEnts.push_back(hrEnt);

        node = xmlNextElementSibling(node); /* HedgeRobotPlatformPairs */
        for (xmlNodePtr ptmPair = xmlFirstElementChild(node); ptmPair != nullptr; ptmPair = xmlNextElementSibling(ptmPair))
        {
            TableId id = GetXmlAttrValue<TableId>(ptmPair, (const xmlChar*)"Id");
            string ptmAStr = GetXmlAttrValue<std::string>(ptmPair, (const xmlChar*)"PlatformA");
            string ptmBStr = GetXmlAttrValue<std::string>(ptmPair, (const xmlChar*)"PlatformB");

            xmlNodePtr child = xmlFirstElementChild(ptmPair);
            Rate margin(GetXmlContent<double>(child));

            child = xmlNextElementSibling(child);
            Rate marginAdjustSize(GetXmlContent<double>(child));

            child = xmlNextElementSibling(child);
            HrNs::OderPricingMode opm = enumHelper.GetOderPricingMode(GetXmlContent<std::string>(child).c_str());

            shared_ptr<PlatformEntity> ptmEntA = ptmEnts.find(ptmAStr)->second;
            shared_ptr<PlatformEntity> ptmEntB = ptmEnts.find(ptmBStr)->second;

            auto hrPtmPairEnt = make_shared<HrPlatformPairEntity>(id, ptmEntA, ptmEntB,
                margin, marginAdjustSize, opm);
			hrEnt->Bind(hrPtmPairEnt);
        }

		list<shared_ptr<HrAccGrpNormalEntity>> hrAccGrpEnts;
        node = xmlNextElementSibling(node); /* HedgeRobotAccountGroups */
        for (xmlNodePtr hrAccGrp = xmlFirstElementChild(node); hrAccGrp != nullptr; hrAccGrp = xmlNextElementSibling(hrAccGrp))
        {
            TableId id = GetXmlAttrValue<TableId>(hrAccGrp, (const xmlChar*)"Id");

            xmlNodePtr chNode = xmlFirstElementChild(hrAccGrp);
            HrNs::AccountGroupMln accountGroupMln = GetXmlContent<HrNs::AccountGroupMln>(chNode);

            auto hrAccGrpEnt = make_shared<HrAccGrpNormalEntity>(id, accountGroupMln);
			hrAccGrpEnts.push_back(hrAccGrpEnt);

            for (xmlNodePtr hrAcc = xmlNextElementSibling(chNode); hrAcc != nullptr; hrAcc = xmlNextElementSibling(hrAcc))
            {
                TableId id = GetXmlAttrValue<TableId>(hrAcc, (const xmlChar*)"Id");
                string userName = GetXmlAttrValue<string>(hrAcc, (const xmlChar*)"UserName");
                string ptmStr = GetXmlAttrValue<std::string>(hrAcc, (const xmlChar*)"Platorm");

                xmlNodePtr chNode = xmlFirstElementChild(hrAcc);
                string publicKey = GetXmlContent<string>(chNode);

                chNode = xmlNextElementSibling(chNode);
                string privateKey = GetXmlContent<string>(chNode);

                chNode = xmlNextElementSibling(chNode);
                Money investedMoney(GetXmlContent<double>(chNode));

                chNode = xmlNextElementSibling(chNode);
                CoinNumber investedCoinNumber(GetXmlContent<double>(chNode));

                auto hrAccEnt = make_shared<HrAccountEntity>(id, userName.c_str(),
                    publicKey.c_str(), privateKey.c_str(),
                    investedMoney, investedCoinNumber);

                shared_ptr<PlatformEntity> ptmEnt = ptmEnts.find(ptmStr)->second;
				ptmEnt->Bind(hrAccEnt);

                hrAccGrpEnt->Bind(hrAccEnt);
			} /* for (xmlNodePtr hrAcc = xmlNextElementSibling(chNode); hrAcc != nullptr; hrAcc = xmlNextElementSibling(hrAcc)) */

            hrEnt->Bind(hrAccGrpEnt);
		} /* for (xmlNodePtr hrAccGrp = xmlFirstElementChild(node); hrAccGrp != nullptr; hrAccGrp = xmlNextElementSibling(hrAccGrp)) */
    } /* for (int i = 0; i < nodes->nodeNr; ++i)   "HedgeRobots" */

    StartTransaction(db, session, MaxTransactionRetryNumber);
    for (auto ptmEntIter = ptmEnts.begin(); ptmEntIter != ptmEnts.end(); ++ptmEntIter)
    {
        auto ptmEnt = ptmEntIter->second;
        db->persist(ptmEnt);

        for (auto apiRuleEntIter = ptmEnt->GetApiRules().begin(); apiRuleEntIter != ptmEnt->GetApiRules().end(); ++apiRuleEntIter)
        {
            auto apiRuleEnt = *apiRuleEntIter;
            db->persist(apiRuleEnt);
        }

        for (auto coinRuleEntIter = ptmEnt->GetCoinRules().begin();
            coinRuleEntIter != ptmEnt->GetCoinRules().end();
            ++coinRuleEntIter)
        {
            auto coinRuleEnt = *coinRuleEntIter;
            db->persist(coinRuleEnt);
        }
    }

    for (auto hrEntIter = hrEnts.begin(); hrEntIter != hrEnts.end(); ++hrEntIter)
    {
        auto hrEnt = *hrEntIter;
        db->persist(hrEnt);

        for (auto hrPmtPairEntIter = hrEnt->GetPlatformPairs().begin();
            hrPmtPairEntIter != hrEnt->GetPlatformPairs().end();
            ++hrPmtPairEntIter)
        {
            auto hrPmtPairEnt = *hrPmtPairEntIter;
            db->persist(hrPmtPairEnt);
        }

        for (auto hrAccGrpEntIter = hrEnt->GetAccountGroups().begin();
            hrAccGrpEntIter != hrEnt->GetAccountGroups().end();
            ++hrAccGrpEntIter)
        {
            auto hrAccGrpEnt = *hrAccGrpEntIter;
            db->persist(hrAccGrpEnt);

            auto hrAccEnt0 = hrAccGrpEnt->GetHrAccount0();
            auto hrAccEnt1 = hrAccGrpEnt->GetHrAccount1();

            db->persist(hrAccEnt0);
            db->persist(hrAccEnt1);
        }
    }
    EndTransaction();

    if (allTablesCleared)
    {
        InitTableIdTable();
    }

    xmlCleanupParser();
}

void XmlWrapper::ClearAllTables()
{
    std::shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
	std::shared_ptr<odb::session> session = Database::GetInstance().GetSession();

	StartTransaction(db, session, MaxTransactionRetryNumber);
    db->execute("delete from `HedgeRobotPlatformPair`;");
    db->execute("delete from `HedgeRobotOrder`;");
    db->execute("delete from `HedgeRobot`;");
    db->execute("delete from `HedgeRobotAccountGroupOpmTrial`;");
    db->execute("delete from `HedgeRobotAccountGroupHorzTrial`;");
    db->execute("delete from `HedgeRobotAccountGroupNormal`;");
    db->execute("delete from `HedgeRobotAccountGroup`;");
    db->execute("delete from `HedgeRobotAccount`;");
    db->execute("delete from `Robot`;");
    db->execute("delete from `PlatformCoinRule`;");
    db->execute("delete from `PlatformApiRule`;");
    db->execute("delete from `Platform`;");
    db->execute("delete from `Order`;");
    db->execute("delete from `Account`;");
    db->execute("delete from `Market`;");
    db->execute("delete from `TableId`;");
    EndTransaction();
}

void XmlWrapper::InitTableIdTable()
{
	std::shared_ptr<odb::database> db = Database::GetInstance().GetDatabase();
	std::shared_ptr<odb::session> session = Database::GetInstance().GetSession();

	StartTransaction(db, session, MaxTransactionRetryNumber);
    /* TableIdxEntity */
    const char *tableNames[] =
    {
        "HedgeRobotPlatformPair", "HedgeRobotAccountGroup", "Robot",
        "PlatformCoinRule", "PlatformApiRule", "Platform", "Order", "Account", "Market"
    };
    for (size_t i = 0; i < sizeof(tableNames) / sizeof(const char*); ++i)
    {
        ostringstream os;
        os << "insert into `TableId`(`TableName`, `TableId`)";
        os << "  select '" << tableNames[i] << "' as TableName,";
        os << "    ifnull(max(Id), 0) + 1 as TableId";
        os << "  from `" << tableNames[i] << "`;";
        db->execute(os.str());
    }
    EndTransaction();
}