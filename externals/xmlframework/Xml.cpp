/*==============================================================================
    Dan Wilcox <Daniel.Wilcox@aec.at>, 2009
==============================================================================*/
#include "Xml.h"

#include "Log.h"

#include <sstream>

namespace xml {

/* ***** READ ***** */

bool Xml::getAttrBool(const TiXmlElement* xmlPtr, std::string name, bool defaultVal)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::getAttrBool(): element is NULL" << std::endl;
        return defaultVal;
    }

    int value = 0;
    int iRet = xmlPtr->QueryIntAttribute(name.c_str(), &value);

    switch(iRet)
    {
        case TIXML_WRONG_TYPE:
            LOG_WARN << "Xml::getAttrBool(): attribute \"" << name
                     << "\" is not of type int in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        case TIXML_NO_ATTRIBUTE:
            LOG_WARN << "Xml::getAttrBool(): int attribute \"" << name
                     << "\" was not found in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        default:    // TIXML_SUCCESS:
            return (bool) value;
    }
}

uint8_t Xml::getAttrByte(const TiXmlElement* xmlPtr, std::string name, uint8_t defaultVal)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::getAttrChar(): element is NULL" << std::endl;
        return defaultVal;
    }

    int value = 0;
    int iRet = xmlPtr->QueryIntAttribute(name.c_str(), &value);

    switch(iRet)
    {
        case TIXML_WRONG_TYPE:
            LOG_WARN << "Xml::getAttrBool(): attribute \"" << name
                     << "\" is not of type int in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        case TIXML_NO_ATTRIBUTE:
            LOG_WARN << "Xml::getAttrBool(): int attribute \"" << name
                     << "\" was not found in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        default:    // TIXML_SUCCESS:
            return (uint8_t) value;
    }
}

unsigned int Xml::getAttrUInt(const TiXmlElement* xmlPtr, std::string name, unsigned int defaultVal)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::getAttrUInt(): element is NULL" << std::endl;
        return defaultVal;
    }

    int value = 0;
    int iRet  = xmlPtr->QueryIntAttribute(name.c_str(), &value);
    switch(iRet)
    {
        case TIXML_WRONG_TYPE:
            LOG_WARN << "Xml::getAttrUInt(): attribute \"" << name
                     << "\" is not of type int in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        case TIXML_NO_ATTRIBUTE:
            LOG_WARN << "Xml::getAttrUInt(): int attribute \"" << name
                     << "\" was not found in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        default:    // TIXML_SUCCESS:
            return (unsigned int) value;
    }
}

int Xml::getAttrInt(const TiXmlElement* xmlPtr, std::string name, int defaultVal)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::getAttrInt(): element is NULL" << std::endl;
        return defaultVal;
    }

	int value = 0;
    int iRet  = xmlPtr->QueryIntAttribute(name.c_str(), &value);
    switch(iRet)
    {
        case TIXML_WRONG_TYPE:
            LOG_WARN << "Xml::getAttrInt(): attribute \"" << name
                     << "\" is not of type int in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        case TIXML_NO_ATTRIBUTE:
            LOG_WARN << "Xml::getAttrInt(): int attribute \"" << name
                     << "\" was not found in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        default:    // TIXML_SUCCESS:
            return value;
    }
}

float Xml::getAttrFloat(const TiXmlElement* xmlPtr, std::string name, float defaultVal)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::getAttrFloat(): element is NULL" << std::endl;
        return defaultVal;
    }

	float value = 0;
    int iRet = xmlPtr->QueryFloatAttribute(name.c_str(), &value);
    switch(iRet)
    {
        case TIXML_WRONG_TYPE:
            LOG_WARN << "Xml::getAttrFloat(): attribute \"" << name
                     << "\" is not of type float in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        case TIXML_NO_ATTRIBUTE:
            LOG_WARN << "Xml::getAttrFloat(): float attribute \"" << name
                     << "\" was not found in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        default:    // TIXML_SUCCESS:
            return value;
    }
}

double Xml::getAttrDouble(const TiXmlElement* xmlPtr, std::string name, double defaultVal)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::getAttrDouble(): element is NULL" << std::endl;
        return defaultVal;
    }

	double value = 0;
    int iRet = xmlPtr->QueryDoubleAttribute(name.c_str(), &value);
    switch(iRet)
    {
        case TIXML_WRONG_TYPE:
            LOG_WARN << "Xml::getAttrDouble(): attribute \"" << name
                     << "\" is not of type double in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        case TIXML_NO_ATTRIBUTE:
            LOG_WARN << "Xml::getAttrDouble(): double attribute \"" << name
                     << "\" was not found in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
            return defaultVal;

        default:    // TIXML_SUCCESS:
            return value;
    }
}

std::string Xml::getAttrString(const TiXmlElement* xmlPtr, std::string name, std::string defaultVal)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::getAttrString(): element is are NULL" << std::endl;
        return defaultVal;
    }

    // try to grab value, will be NULL if attr does not exist
    const char* value = xmlPtr->Attribute(name.c_str());
    if(value == NULL)
    {
        LOG_WARN << "Xml::getAttrString(): string attribute \"" << name
                     << "\" was not found in element \"" << xmlPtr->ValueStr()
                     << "\"" << std::endl;
        return defaultVal;
    }
    
    return std::string(value);
}

bool Xml::getAttr(const TiXmlElement* xmlPtr, std::string name, XmlType type, void* var)
{
    if(xmlPtr == NULL || var == NULL)
    {
        LOG_ERROR << "Xml::getAttr(): element and/or variable are NULL" << std::endl;
        return false;
    }

    int ret = TIXML_SUCCESS;
    switch(type)
    {
        case XML_TYPE_BOOL:
        {
        	bool* pVar = (bool*) var;
            (*pVar) = getAttrBool(xmlPtr, name);
            break;
        }
            
        case XML_TYPE_BYTE:
        {
            uint8_t* pVar = (uint8_t*) var;
            (*pVar) = getAttrByte(xmlPtr, name);
            break;
		}
        
		case XML_TYPE_UINT:
        {
            unsigned int* pVar = (unsigned int*) var;
            (*pVar) = getAttrUInt(xmlPtr, name);
            break;
		}

        case XML_TYPE_INT:
        {
            int* pVar = (int*) var;
            (*pVar) = getAttrInt(xmlPtr, name);
            break;
		}

        case XML_TYPE_FLOAT:
        {
            float* pVar = (float*) var;
            (*pVar) = getAttrFloat(xmlPtr, name);
            break;
		}

        case XML_TYPE_DOUBLE:
        {
            double* pVar = (double*) var;
            (*pVar) = getAttrDouble(xmlPtr, name);
            break;
		}

        case XML_TYPE_STRING:
        {
            std::string* pVar = (std::string*) var;
            pVar->clear();
            pVar->append(getAttrString(xmlPtr, name));
            break;
		}
        
        default:
            break;
    }

    return ret;
}

std::string Xml::getText(const TiXmlElement* xmlPtr, std::string defaultVal)
{
    if(xmlPtr == NULL)
    {
        return defaultVal;
    }

    const char* text = xmlPtr->GetText();
    if(text == NULL)
        return defaultVal;

    return (std::string) text;
}

std::string Xml::element2String(const TiXmlElement* xmlPtr, std::string indent)
{
    if(xmlPtr == NULL)
    {
        return "";
    }

	const TiXmlAttribute* xmlAttr = xmlPtr->FirstAttribute();

	std::stringstream stream;
	stream << "Elem: " << xmlPtr->Value() << std::endl;

    // loop through attributes
	while(xmlAttr != NULL)
	{
        stream << indent << "Attr: " << xmlAttr->Name() << " " << xmlAttr->Value();
		xmlAttr = xmlAttr->Next();
	}

	return stream.str();
}

TiXmlElement* Xml::getElement(TiXmlElement* xmlPtr, std::string name, int index)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::getElement(): element is NULL" << std::endl;
        return NULL;
    }

    TiXmlHandle h(xmlPtr);
    return h.ChildElement(name, index).Element();
}

/* ***** WRITE ***** */

void Xml::setAttrString(TiXmlElement* xmlPtr, std::string name, std::string s)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::setAttrString(): element is NULL" << std::endl;
        return;
    }

    xmlPtr->SetAttribute(name, s);
}

void Xml::setAttrInt(TiXmlElement* xmlPtr, std::string name, int i)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::setAttrInt(): element is NULL" << std::endl;
        return;
    }

    xmlPtr->SetAttribute(name, i);
}

void Xml::setAttrUInt(TiXmlElement* xmlPtr, std::string name, unsigned int i)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::setAttrUInt(): element is NULL" << std::endl;
        return;
    }

    xmlPtr->SetAttribute(name, (int) i);
}

void Xml::setAttrDouble(TiXmlElement* xmlPtr, std::string name, double d)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::setAttrDouble(): element is NULL" << std::endl;
        return;
    }

    xmlPtr->SetDoubleAttribute(name.c_str(), d);
}

void Xml::setAttrFloat(TiXmlElement* xmlPtr, std::string name, float f)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::setAttrFloat(): element is NULL" << std::endl;
        return;
    }

    xmlPtr->SetDoubleAttribute(name.c_str(), f);
}

void Xml::setAttrBool(TiXmlElement* xmlPtr, std::string name, bool b)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::setAttrBool(): element is NULL" << std::endl;
        return;
    }

    xmlPtr->SetAttribute(name, (int) b);
}

void Xml::setAttrByte(TiXmlElement* xmlPtr, std::string name, uint8_t b)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::setAttrChar(): element is NULL" << std::endl;
        return;
    }

    xmlPtr->SetAttribute(name, b);
}

void Xml::setAttr(TiXmlElement* xmlPtr, std::string name, XmlType type, void* var)
{
    if(xmlPtr == NULL || var == NULL)
    {
        LOG_ERROR << "Xml::setAttr(): element and/or variable are NULL" << std::endl;
        return;
    }

    switch(type)
    {
        case XML_TYPE_BYTE:
        {
            uint8_t* b = (uint8_t*) var;
            setAttrByte(xmlPtr, name, *b);
            break;
        }

        case XML_TYPE_BOOL:
        {
            bool* b = (bool*) var;
            setAttrBool(xmlPtr, name, *b);
            break;
        }

        case XML_TYPE_INT:
        {
            int* i = (int*) var;
            setAttrInt(xmlPtr, name, *i);
            break;
        }

        case XML_TYPE_UINT:
        {
            unsigned int* ui = (unsigned int*) var;
            setAttrUInt(xmlPtr, name, *ui);
            break;
        }

        case XML_TYPE_FLOAT:
        {
            float* f = (float*) var;
            setAttrFloat(xmlPtr, name, *f);
            break;
        }

        case XML_TYPE_DOUBLE:
        {
            double* d = (double*) var;
            setAttrDouble(xmlPtr, name, *d);
            break;
        }

        case XML_TYPE_STRING:
        {
            std::string* s = (std::string*) var;
            setAttrString(xmlPtr, name, *s);
            break;
        }

        default:
            break;
    }
}

void Xml::setText(TiXmlElement* xmlPtr, std::string text)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::setText(): element is NULL" << std::endl;
        return;
    }

    xmlPtr->LinkEndChild(new TiXmlText(text));
}

TiXmlElement* Xml::obtainElement(TiXmlElement* xmlPtr, std::string name, int index)
{
    if(xmlPtr == NULL)
    {
        LOG_ERROR << "Xml::addExistingElement(): element is NULL" << std::endl;
        return NULL;
    }

    TiXmlHandle h(xmlPtr);

    // if element doesnt exit, add it
    TiXmlElement* child = h.ChildElement(name, index).Element();
    if(child == NULL)
    {
        child = new TiXmlElement(name);
        xmlPtr->LinkEndChild(child);
    }

    return child;
}

/* ***** UTIL ***** */

std::string Xml::getErrorString(const TiXmlDocument* xmlDoc)
{
    if(xmlDoc == NULL)
        return "";

    std::stringstream error;
    error << "line " <<  xmlDoc->ErrorRow() << ", " << (std::string) xmlDoc->ErrorDesc();
    return error.str();
}

} // namespace
