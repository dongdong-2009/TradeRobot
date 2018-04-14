// BtcRobotSvc.cpp : WinMain µÄÊµÏÖ


#include "stdafx.h"
#include "resource.h"
#include "BtcRobotSvc_i.h"

using namespace ATL;
#include <stdio.h>

/* curl*/
#include <curl/curl.h>

/* Functions */
#include "Include/Functions/ControllerInterface.h"

class CBtcRobotSvcModule : public ATL::CAtlServiceModuleT< CBtcRobotSvcModule, IDS_SERVICENAME >
{
public:
    DECLARE_LIBID(LIBID_BtcRobotSvcLib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_BTCROBOTSVC, "{6194D0D9-F52B-406C-BA73-094972DBA766}")
    HRESULT InitializeSecurity() throw()
    {
        return S_OK;
    }

    HRESULT Start(_In_ int nShowCmd) throw()
    {
        ControllerInterface& controller = GetControllerInterface();
        controller.Start();
        return __super::Start(nShowCmd);
    }

    void OnStop() throw()
    {
        ControllerInterface& controller = GetControllerInterface();
        controller.Stop();
        return __super::OnStop();
    }
};

CBtcRobotSvcModule _AtlModule;
//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    CURLcode curlCode = curl_global_init(CURL_GLOBAL_ALL);
    assert(curlCode == CURLE_OK);
	int ret = _AtlModule.WinMain(nShowCmd);
    return ret;
}

