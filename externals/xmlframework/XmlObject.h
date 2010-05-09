/*==============================================================================

	XmlObject.h
    
    xmlframework: object based xml classes for TinyXml
  
	Copyright (C) 2009, 2010  Dan Wilcox <danomatika@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#ifndef XML_XML_OBJECT_H
#define XML_XML_OBJECT_H

#include <vector>

#include "tinyxml/tinyxml.h"

namespace xml {

enum XmlType
{
    XML_TYPE_UNDEF = 0,
    XML_TYPE_BOOL,
    XML_TYPE_BYTE,
    XML_TYPE_INT,
    XML_TYPE_UINT,
    XML_TYPE_FLOAT,
    XML_TYPE_DOUBLE,
    XML_TYPE_STRING
};

class XmlObject
{
    public:

		/// set the xml element name, if the element is empty the element name of the parent is used
        XmlObject(std::string elementName);
        virtual ~XmlObject();

        /* ***** LOAD ***** */

        /// load from an xml element, checks if the element name is correct
        bool loadXml(TiXmlElement* e);

        /// load from an xml file, leave empty to use previous filename
        bool loadXmlFile(std::string filename="");

        /* ***** SAVE ***** */

        /// save to an xml element, checks if the element name is correct
        bool saveXml(TiXmlElement* e);

        /// save to a new xml file
        bool saveXmlFile(std::string filename="");

        /// close the current file (does not save, call load to open again)
        void closeXmlFile();

        /* ***** ADD OTHER OBJECTS ***** */

        /// attach/remove an XmlObject to this one
        void addXmlObject(XmlObject* object);
        void removeXmlObject(XmlObject* object);

        /* ***** ELEMENTS ***** */

        /// add/remove an element, element names are singular
        bool addXmlElement(std::string name, std::string* text=NULL, bool readOnly=false);
        bool removeXmlElement(std::string name);   /// also removes attached attributes
        void removeAllXmlElements();   /// alsa removes attached attributes

        /* ***** ATTRIBUTES ***** */

        /// add/remove an attribute, element names are singular
        /// if element does not exist, it will be created
        /// if it exists, attribute will be atatched to it
        /// if element name is the same as the object name, the attribute will be added to the root tag
        bool addXmlAttribute(std::string name, std::string elementName, XmlType type, void* var, bool readOnly=false);
        bool removeXmlAttribute(std::string name, std::string elementName);

        /* ***** UTIL ***** */

        /// get/set the filename
        inline std::string& getXmlFilename() {return m_filename;}
        inline void setXmlFilename(std::string name) {m_filename = name;}

        /// get/set this objects xml element name
        inline std::string& getXmlName() {return m_elementName;}
        inline void setXmlName(std::string name) {m_elementName = name;}

        /// get the root element (for direct access)
        TiXmlElement* getXmlRootElement();

    protected:

        /* ***** USER CALLBACKS ***** */
        /// derive these callbacks for direct access to loading/saving
        /// these are called after attached elements, then objects are processed

        /// callback to process xml children when loaded, returns true if element handled
        /// param   e   root element of this object aka <getXmlName()> ...
        virtual bool readXml(TiXmlElement* e) {return false;}

        /// callback to save to xml when saved, returns true if successful
        /// param   e   root element of this object aka <getXmlName()> ...
        virtual bool writeXml(TiXmlElement* e) {return false;}

    private:

        struct Attribute
        {
            Attribute() : type(XML_TYPE_UNDEF) {}

            std::string name;
            XmlType type;
            void* var;
            bool bReadOnly;
        };

        struct Element
        {
            std::string name;
            std::string* text;
            bool bReadOnly;
            std::vector<Attribute*> attributeList;
        };

        // find an element in the list by its name, returns NULL if not found
        Element* findElement(std::string name)
        {
            std::vector<Element*>::iterator iter;
            for(iter = m_elementList.begin(); iter != m_elementList.end(); ++iter)
            {
                if((*iter)->name == name)
                {
                    return (*iter);
                }
            }

            return NULL;
        }

        bool m_bDocLoaded;       /// is this doc loaded?
        std::string m_filename;  /// current filename
        TiXmlDocument* m_xmlDoc; /// the xml document

        std::string m_elementName;   /// name of the root element
        std::vector<Element*> m_elementList;     /// attached elements/attributes
        std::vector<XmlObject*> m_objectList;    /// attached xml objects to process
};

} // namespace

#endif // XML_XML_OBJECT_H
