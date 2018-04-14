/* C++ */
#include <iostream>
#include <cassert>

/* Curl*/
#include <curl/curl.h>

/* Cpp Unit */
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/* log4cplus */
#include <log4cplus/logger.h>           //class Logger
#include <log4cplus/configurator.h>     //class PropertyConfigurator
#include <log4cplus/loggingmacros.h>    //LOG4CPLUS_TRACE()

/* Foundation */
#include "Include/Foundation/Debug.h"

/* local header files */
#include "Version.h"

#if defined(_WIN32) && defined(_MSC_VER)
#    if defined(_DEBUG)
#        pragma comment(lib, "cppunitd-vc120.lib")
#        pragma comment(lib, "log4cplus.win32.d.lib")
#    else
#        error unit test project must be debug version!
#    endif
#endif

using namespace std;

int main(int argc, char **argv)
{
    log4cplus::initialize();
    log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

    LOG4_INFO("Software Svn Version: " << ExeVersion);

    CURLcode curlCode = curl_global_init(CURL_GLOBAL_ALL);
    assert(curlCode == CURLE_OK);

    // Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    // Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener(&result);

    // Add a listener that print dots as test run.
    CPPUNIT_NS::BriefTestProgressListener progress;
    controller.addListener(&progress);

    // Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;
    runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    runner.run(controller);

    // Print test in a compiler compatible format.
    CPPUNIT_NS::CompilerOutputter outputter(&result, CPPUNIT_NS::stdCOut());
    outputter.write();

    curl_global_cleanup();

    system("pause");
    return 0;
}