#ifndef _XmlHelper_h_
#define _XmlHelper_h_

#include "SystemInclude.h"

/* libxml */
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
typedef std::shared_ptr<xmlChar> SharedXmlChar;

template<typename T>
inline T  GetXmlAttrValue(xmlNodePtr node, const xmlChar *attrName)
{
    SharedXmlChar attrValue(xmlGetProp(node, attrName), [](xmlChar *ptr){ xmlFree(ptr); });
    char *ptr = (char *)attrValue.get();
    if (ptr == nullptr || strlen(ptr) == 0)
    {
        return 0;
    }

    if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
        return (T)strtol(ptr, nullptr, 16);

    return (T)strtol(ptr, nullptr, 10);
}
    
template<>
inline SharedXmlChar GetXmlAttrValue<SharedXmlChar>(xmlNodePtr node, const xmlChar *attrName)
{
    SharedXmlChar attrValue(xmlGetProp(node, attrName), [](xmlChar *ptr){ xmlFree(ptr); });

    return attrValue;
}   

template<>
inline std::string GetXmlAttrValue<std::string>(xmlNodePtr node, const xmlChar *attrName)
{
    std::string attrValue((char*)xmlGetProp(node, attrName));

    return attrValue;
} 

template<typename T>
inline T  GetXmlContent(xmlNodePtr node)
{
    SharedXmlChar content(xmlNodeGetContent(node), [](xmlChar *ptr){ xmlFree(ptr); });
    char *ptr = (char *)content.get();
    assert(ptr != nullptr);

    if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
        return (T)strtol(ptr, nullptr, 16);

    return (T)strtol(ptr, nullptr, 10);
}

template<>
inline SharedXmlChar GetXmlContent<SharedXmlChar>(xmlNodePtr node)
{
    SharedXmlChar content(xmlNodeGetContent(node), [](xmlChar *ptr){ xmlFree(ptr); });

    return content;
}

template<>
inline double GetXmlContent<double>(xmlNodePtr node)
{
    std::string content((char*)xmlNodeGetContent(node));

    return strtod(content.c_str(), nullptr);
}

template<>
inline std::string GetXmlContent<std::string>(xmlNodePtr node)
{
    std::string content((char*)xmlNodeGetContent(node));

    return content;
}

#endif