/*==============================================================================
    Dan Wilcox <Daniel.Wilcox@aec.at>, 2009
==============================================================================*/
#include "XmlObject.h"

#include "Log.h"

#include <map>
#include <algorithm>

#include "Xml.h"

#define DEBUG_XML_OBJECT

namespace xml {

XmlObject::XmlObject(std::string elementName) :
    m_bDocLoaded(false), m_elementName(elementName) {}

XmlObject::~XmlObject()
{
    removeAllXmlElements();
    closeXmlFile();
}

bool XmlObject::loadXml(TiXmlElement* e)
{
    if(e == NULL)
    {
        return false;
    }

    // check if the element is correct
    if(e->ValueStr() != m_elementName)
    {
        LOG_WARN << "Xml \"" << m_elementName << "\": wrong xml element name \""
        		 << e->ValueStr() << "\" for object with element name \""
                 << m_elementName << "\"" << std::endl;
        return false;
    }

    #ifdef DEBUG_XML_OBJECT
    LOG_DEBUG << "loading xml " << m_elementName << std::endl;
    #endif

    TiXmlElement* child;

    // load attached elements
    for(unsigned int i = 0; i < m_elementList.size(); ++i)
    {
        Element* elem = m_elementList.at(i);

        #ifdef DEBUG_XML_OBJECT
        LOG_DEBUG << "elem: " << elem->name << std::endl;
        #endif

        // check if this element is the same as the root
        if(e->ValueStr() == elem->name)
        {
            child = e;
        }
        else    // try to find a child with the same element name
        {

            child = Xml::getElement(e, elem->name);
        }

        if(child != NULL)
        {
            // load the elements text
            if(elem->text != NULL)
            {
                elem->text->clear();
                elem->text->append(Xml::getText(child));
            }

            // load the attached attributes
            for(unsigned int j = 0; j < elem->attributeList.size(); ++j)
            {
                Attribute* attr = elem->attributeList.at(j);

                #ifdef DEBUG_XML_OBJECT
                LOG_DEBUG << "    attr: " << attr->name << std::endl;
                #endif

                Xml::getAttr(child, attr->name, attr->type, attr->var);
            }
        }
    }

    // keep track of how many elements with the same name
    std::map<std::string, int> elementMap;

    // load attached objects
    std::vector<XmlObject*>::iterator objectIter;
    for(objectIter = m_objectList.begin(); objectIter != m_objectList.end();)
    {
        // remove this object if it dosent exist anymore
        if((*objectIter) == NULL)
        {
            objectIter = m_objectList.erase(objectIter);
            LOG_WARN << "Xml \"" << m_elementName << "\" load: removed NULL xml object" << std::endl;
        }
        else // exists
        {
            // try to find element name in map
            std::map<std::string, int>::iterator iter = elementMap.find((*objectIter)->getXmlName());
            if(iter == elementMap.end())
            {
                // not found, so add element name to map
                elementMap.insert(make_pair((*objectIter)->getXmlName(), 0));
                iter = elementMap.find((*objectIter)->getXmlName());
            }
            else
                iter->second++; // found another

            #ifdef DEBUG_XML_OBJECT
            LOG_DEBUG << "object: " << (*objectIter)->getXmlName() << " " << iter->second << std::endl;
            #endif

            // try to find an element with same name as the object by index (if multiples)
            child = Xml::getElement(e, (*objectIter)->getXmlName(), iter->second);
            if(child != NULL)
            {
                (*objectIter)->loadXml(child);  // found element
            }
            else
            {
                #ifdef DEBUG_XML_OBJECT
                LOG_DEBUG << "  element not found for object" << std::endl;
                #endif
            }

            ++objectIter; // increment iter
        }
    }

    // process user callback
    return readXml(e);
}

bool XmlObject::loadXmlFile(std::string filename)
{
    // close if loaded
    if(m_bDocLoaded)
        closeXmlFile();
     
    // if not set, try using previous filename
    if(filename == "")
    {
    	filename = m_filename;
    }

    // try to load the file
    m_xmlDoc = new TiXmlDocument;
    if(!m_xmlDoc->LoadFile(filename.c_str(), TIXML_ENCODING_UTF8))
    {
        LOG_ERROR << "Xml \"" << m_elementName << "\": could not load \""
                  << filename << "\": " << Xml::getErrorString(m_xmlDoc)  << std::endl;
        closeXmlFile();
        return false;
    }

    // get the root element
    TiXmlElement* root = m_xmlDoc->RootElement();

    // check if the root is correct
    if(root->ValueStr() != m_elementName)
    {
        LOG_ERROR << "Xml \"" << m_elementName << "\": xml file \"" << filename
                  << "\" does not have \"" << m_elementName << "\" as the root element"
                  << std::endl;
        closeXmlFile();
        return false;
    }

    m_filename = filename;
    m_bDocLoaded = true;

    // load everything
    return loadXml(root);
}

bool XmlObject::saveXml(TiXmlElement* e)
{
    if(e == NULL)
    {
        return false;
    }

    // check if the element is correct
    if(e->ValueStr() != m_elementName)
    {
        LOG_WARN << "Xml \"" << m_elementName << "\": xml element value is not \""
                 << m_elementName << "\"" << std::endl;
        return false;
    }

    #ifdef DEBUG_XML_OBJECT
    LOG_DEBUG << "saving xml " << m_elementName << std::endl;
    #endif

    TiXmlElement* child;

    // save attached elements
    for(unsigned int i = 0; i < m_elementList.size(); ++i)
    {
        Element* elem = m_elementList.at(i);

        #ifdef DEBUG_XML_OBJECT
        LOG_DEBUG << "elem: " << elem->name << std::endl;
        #endif

        // check if this element is the same as the root
        if(e->ValueStr() == elem->name)
        {
            child = e;
        }
        else
        {
            // find element, add if it dosen't exit
            child = Xml::obtainElement(e, elem->name);
        }

        // set the element's text if any
        if(elem->text != NULL)
        {
            TiXmlText* text = new TiXmlText(*elem->text);
            child->LinkEndChild(text);
        }

        // save the element's attached attributes
        for(unsigned int j = 0; j < elem->attributeList.size(); ++j)
        {
            Attribute* attr = elem->attributeList.at(j);

            #ifdef DEBUG_XML_OBJECT
            LOG_DEBUG << "    attr: " << attr->name << std::endl;
            #endif

            if(!attr->bReadOnly)
                Xml::setAttr(child, attr->name, attr->type, attr->var);
        }
    }

    // keep track of how many elements with the same name
    std::map<std::string, int> elementMap;

    // save all attached objects
    bool ret = true;
    std::vector<XmlObject*>::iterator objectIter;
    for(objectIter = m_objectList.begin(); objectIter != m_objectList.end();)
    {
        // remove this object if it dosent exist anymore
        if((*objectIter) == NULL)
        {
            m_objectList.erase(objectIter);
            LOG_WARN << "Xml \"" << m_elementName << "\" save: removed NULL xml object" << std::endl;
            ++objectIter; // increment iter
        }
        else // exists
        {
            // try to find element name in map
            std::map<std::string, int>::iterator iter = elementMap.find((*objectIter)->getXmlName());
            if(iter == elementMap.end())
            {
                // not found, so add element name to map
                elementMap.insert(make_pair((*objectIter)->getXmlName(), 0));
                iter = elementMap.find((*objectIter)->getXmlName());
            }
            else
                iter->second++; // found another

            #ifdef DEBUG_XML_OBJECT
            LOG_DEBUG << "object: " << (*objectIter)->getXmlName() << " " << iter->second << std::endl;
            #endif

            // find an element with same name at a specific index, add if it dosen't exist
            child = Xml::obtainElement(e, (*objectIter)->getXmlName(), iter->second);

            // save object
            (*objectIter)->saveXml(child);

            ++objectIter; // increment iter
        }
    }

    // process user callback
    return writeXml(e) || ret;
}

bool XmlObject::saveXmlFile(std::string filename)
{
     TiXmlElement* root;

    // setup new doc if not loaded
    if(!m_bDocLoaded)
    {
        // new doc
        m_xmlDoc = new TiXmlDocument;

        // add the declaration
        m_xmlDoc->LinkEndChild(new TiXmlDeclaration("1.0", "", ""));

        // add root element
        root = new TiXmlElement(getXmlName());
        m_xmlDoc->LinkEndChild(root);

        m_bDocLoaded = true;
    }
    else    // already loaded
    {
        root = m_xmlDoc->RootElement();
    }

    // use the current filename?
    if(filename == "")
    {
        filename = m_filename;
    }

    // load data into the elements
    bool ret = saveXml(root);

    // try saving
    if(!m_xmlDoc->SaveFile(filename))
    {
        LOG_ERROR << "Xml \"" << m_elementName << "\": could not save to \""
                  << filename << "\"" << std::endl;
        ret = false;
    }

    return ret;
}

void XmlObject::closeXmlFile()
{
    if(m_bDocLoaded)
        delete m_xmlDoc;

    m_bDocLoaded = false;
}

void XmlObject::addXmlObject(XmlObject* object)
{
    if(object == NULL)
    {
        LOG_ERROR << "Xml: Cannot add NULL object" << std::endl;
        return;
    }

    m_objectList.push_back(object);
}

void XmlObject::removeXmlObject(XmlObject* object)
{
    if(object == NULL)
    {
        LOG_ERROR << "Xml: Cannot remove NULL object" << std::endl;
        return;
    }

    std::vector<XmlObject*>::iterator iter;
    iter = find(m_objectList.begin(), m_objectList.end(), object);
    if(iter != m_objectList.end())
    {
        m_objectList.erase(iter);
    }
}

bool XmlObject::addXmlElement(std::string name, std::string* text, bool readOnly)
{
    if(name == "")
    {
        LOG_ERROR << "Xml \"" << m_elementName << "\": cannot add element \"" << name
                  << "\", name is empty" << std::endl;
        return false;
    }

    Element* element;

    // bail if element already exists
    if((element = findElement(name)) != NULL)
    {
        LOG_WARN << "Xml \"" << m_elementName << "\": cannot add element \"" << name
                  << "\", element already exists" << std::endl;
        return false;
    }

    // add
    element = new Element;
    element->name = name;
    element->text = text;
    element->bReadOnly = readOnly;
    m_elementList.push_back(element);

    return true;
}

bool XmlObject::removeXmlElement(std::string name)
{
    std::vector<Element*>::iterator iter;
    for(iter = m_elementList.begin(); iter != m_elementList.end(); ++iter)
    {
        if((*iter)->name == name)
        {
            delete (*iter);
            m_elementList.erase(iter);
            return true;
        }
    }

    LOG_WARN << "Xml \"" << m_elementName << "\": cannot remove element \"" << name
                  << "\", not found" << std::endl;

    return false;
}

void XmlObject::removeAllXmlElements()
{
    for(unsigned int i = 0; i < m_elementList.size(); ++i)
    {
        Element* e = m_elementList.at(i);

        for(unsigned int j = 0; j < e->attributeList.size(); ++j)
        {
            Attribute* attr = e->attributeList.at(j);
            delete attr;
        }

        delete e;
    }
    m_elementList.clear();
}

bool XmlObject::addXmlAttribute(std::string name, std::string elementName, XmlType type, void* var, bool readOnly)
{
    if(name == "" || elementName == "")
    {
        LOG_ERROR << "Xml \"" << m_elementName << "\": cannot add attribute \"" << name
                  << "\" to element \"" << elementName << "\", name and/or element name are empty"
                  << std::endl;
        return false;
    }

    if(var == NULL)
    {
        LOG_ERROR << "Xml \"" << m_elementName << "\": attribute \"" << name
                  << "\" variable is NULL" << std::endl;
        return false;
    }

    // check if the requested element exists, if not add it
    Element* e = findElement(elementName);
    if(e == NULL)    // dosent exist
    {
        addXmlElement(elementName);
        e = m_elementList.back();
    }

    Attribute* attribute = new Attribute;
    attribute->name = name;
    attribute->type = type;
    attribute->var = var;
    attribute->bReadOnly = readOnly;

    e->attributeList.push_back(attribute);

    return true;
}

bool XmlObject::removeXmlAttribute(std::string name, std::string elementName)
{
    Element* e = findElement(elementName);
    if(e == NULL)
        return false;

    std::vector<Attribute*>::iterator iter;
    for(iter = e->attributeList.begin(); iter != e->attributeList.end(); ++iter)
    {
        delete (*iter);
        e->attributeList.erase(iter);
        return true;
    }

    LOG_WARN << "Xml \"" << m_elementName << "\": cannot remove attribute \"" << name
             << "\", not found" << std::endl;
    return false;
}

TiXmlElement* XmlObject::getXmlRootElement()
{
    if(m_bDocLoaded)
        return m_xmlDoc->RootElement();
    else
        return NULL;
}

} // namespace
