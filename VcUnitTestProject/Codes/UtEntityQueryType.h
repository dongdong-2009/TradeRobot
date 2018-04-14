#ifndef _UtEntityQueryType_h_
#define _UtEntityQueryType_h_

#include "odb/query.hxx"

/* Entity */
#include "UtEntities.h"
#include "UtEntities-odb.h"

typedef odb::query<Ut1v1SharedPktEntity> Ut1v1SharedPktQuery;
typedef odb::result<Ut1v1SharedPktEntity> Ut1v1SharedPktResult;

typedef odb::query<Ut1v1WeakFktEntity> Ut1v1WeakFktQuery;
typedef odb::result<Ut1v1WeakFktEntity> Ut1v1WeakFktResult;



#endif