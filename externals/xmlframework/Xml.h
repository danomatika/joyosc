/*==============================================================================
    Dan Wilcox <Daniel.Wilcox@aec.at>, 2009
==============================================================================*/
#ifndef XML_XML_H
#define XML_XML_H

#include "tinyxml/tinyxml.h"

#include <stdint.h>

#include "XmlObject.h"

namespace xml {

/**
    \class  Xml
    \brief  convenience wrapper for some of the TinyXml functions
**/
class Xml
{
    public:

        /** \name Reading Member Functions **/

        /// attribute access by type, returns true if value found & set, false if not
        /// does not change var if if attribute not found or wrong type
        static bool 		getAttrBool(const TiXmlElement* xmlPtr, std::string name, bool defaultVal=true);
        static uint8_t 		getAttrByte(const TiXmlElement* xmlPtr, std::string name, uint8_t defaultVal=0);	// unsigned
        static unsigned int getAttrUInt(const TiXmlElement* xmlPtr, std::string name, unsigned int defaultVal=0);
        static int 			getAttrInt(const TiXmlElement* xmlPtr, std::string name, int defaultVal=0);
        static float 		getAttrFloat(const TiXmlElement* xmlPtr, std::string name, float defaultVal=0.0f);
        static double 		getAttrDouble(const TiXmlElement* xmlPtr, std::string name, double defaultVal=0.0);
        static std::string 	getAttrString(const TiXmlElement* xmlPtr, std::string name, std::string defaultVal="");    // converts numbers to text
        
        /// attribute access using XmlObject type enum
        static bool getAttr(const TiXmlElement* xmlPtr, std::string name, XmlType type, void* var);

        /// return the element text as a string
        static std::string getText(const TiXmlElement* xmlPtr, std::string defaultVal="");

        /// return element and attributes as a string, does not descend to children
        static std::string element2String(const TiXmlElement* xmlPtr, std::string indent);

        /// find child element by name and index (if in a list), returns NULL if element not found
        static TiXmlElement* getElement(TiXmlElement* xmlPtr, std::string name, int index=0);

        /** \name Writing Member Functions **/

        static void setAttrString(TiXmlElement* xmlPtr, std::string name, std::string s);
        static void setAttrInt(TiXmlElement* xmlPtr, std::string name, int i);
        static void setAttrUInt(TiXmlElement* xmlPtr, std::string name, unsigned int i);
        static void setAttrDouble(TiXmlElement* xmlPtr, std::string name, double d);
        static void setAttrFloat(TiXmlElement* xmlPtr, std::string name, float f);
        static void setAttrBool(TiXmlElement* xmlPtr, std::string name, bool b);
        static void setAttrByte(TiXmlElement* xmlPtr, std::string name, uint8_t b);

        /// set attribute using XmlObject type enum
        static void setAttr(TiXmlElement* xmlPtr, std::string name, XmlType type, void* var);

        /// set the element text
        static void setText(TiXmlElement* xmlPtr, std::string text);

        /// finds child element at specific index in a list of same elements (0 for first),
        /// creates and adds to end if not found
        static TiXmlElement* obtainElement(TiXmlElement* xmlPtr, std::string name, int index=0);

        /** \name Utility Member Functions **/

        /// returns the current error as a string with line and col of error
        static std::string getErrorString(const TiXmlDocument* xmlDoc);
};

} // namespace

#endif // XML_XML_H
