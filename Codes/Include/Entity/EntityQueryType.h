#ifndef _EntityQueryType_h_
#define _EntityQueryType_h_

#include "odb/query.hxx"

/* Entity */
#include "Include/Entity/Entities.h"
#include "Include/Entity/Entities-odb.h"

typedef odb::query<AccountEntity> AccQuery;
typedef odb::result<AccountEntity> AccResult;

typedef odb::query<MarketEntity> MarketQuery;
typedef odb::result<MarketEntity> MarketResult;

typedef odb::query<HrEntity> HrQuery;
typedef odb::result<HrEntity> HrResult;

typedef odb::query<HrAccountEntity> HrAccQuery;
typedef odb::result<HrAccountEntity> HrAccResult;

typedef odb::query<HrAccountGroupEntity> HrAccGrpQuery;
typedef odb::result<HrAccountGroupEntity> HrAccGrpResult; 

typedef odb::query<HrAccGrpNormalEntity> HrAccGrpNormalQuery;
typedef odb::result<HrAccGrpNormalEntity> HrAccGrpNormalResult;

typedef odb::query<PlatformEntity>  PtmQuery;
typedef odb::result<PlatformEntity>  PtmResult;

typedef odb::query<RobotEntity>  RobotQuery;
typedef odb::result<RobotEntity>  RobotResult;

#endif