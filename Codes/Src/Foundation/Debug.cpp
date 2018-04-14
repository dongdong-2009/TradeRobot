#include "Include/Foundation/Debug.h"

/* C++ */
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

using namespace std;

std::string ConvertUtf8ToString(const char *src)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;

    string dst;
    char *dataTo = nullptr;
    char *dataToEnd = nullptr;
    char *dataToNext = nullptr;
    try
    {
        //utf-8 -> unicode
        wstring wStr = conv.from_bytes((const char *)src);

        /* we use vc to analysis performance , and find out it is  time-consuming to construct
        a std::locale, so we define it as static variable.
        */
        static std::locale sysLocale("");
        const wchar_t* dataFrom = wStr.c_str();
        const wchar_t* dataFromEnd = wStr.c_str() + wStr.length() + 1;
        const wchar_t* dataFromNext = 0;

        int wcharSize = 4;
        dataTo = new char[(wStr.size() + 1) * wcharSize];
        dataToEnd = dataTo + (wStr.size() + 1) * wcharSize;
        dataToNext = 0;

        memset(dataTo, 0, (wStr.size() + 1) * wcharSize);

        typedef std::codecvt<wchar_t, char, mbstate_t> convertFacet;
        mbstate_t outState = std::mbstate_t();
        auto result = std::use_facet<convertFacet>(sysLocale).out(
            outState, dataFrom, dataFromEnd, dataFromNext,
            dataTo, dataToEnd, dataToNext);
        if (result == convertFacet::ok)
        {
            dst.append(dataTo);
        }
    }
    catch (...)
    {
        dst.append(src);
    }

    if (dataTo != nullptr)
        delete[] dataTo;
    return dst;
}
