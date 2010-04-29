/*==============================================================================
    Dan Wilcox <Daniel.Wilcox@aec.at>, 2009
==============================================================================*/
#ifndef VISUAL_OSC_OBJECT_H
#define VISUAL_OSC_OBJECT_H

#include <vector>

#include "OscTypes.h"

namespace osc {

/**
    \class  OscObject
    \brief  derive this class to add to an OscListener

    set the processing function to match messages
**/
class OscObject
{
    public:

        OscObject(std::string rootAddress="") : oscRootAddress("/"+rootAddress) {}

        /* ***** PROCESS OSC MESSAGES ***** */

        /// process attached objects, then call processOscMessage
        /// returns true if message handled
        bool processOsc(const ReceivedMessage& message, const MessageSource& source);

        /* ***** ADD OTHER OBJECTS ***** */

        /// attach/remove an OscObject to this one
        void addOscObject(OscObject* object);
        void removeOscObject(OscObject* object);

        /* ***** UTIL ***** */

        /// get/set the root address of this object
        inline void setOscRootAddress(std::string rootAddress) {oscRootAddress = rootAddress;}
        inline std::string& getOscRootAddress() {return oscRootAddress;}
        inline void prependOscRootAddress(std::string prepend) {oscRootAddress = prepend + oscRootAddress;}

    protected:

        /// callback to implement, returns true if message handled
        virtual bool processOscMessage(const ReceivedMessage& message, const MessageSource& source) {return false;}

        /// the root address of this object, aka something like "/root/test1/string2"
        std::string oscRootAddress;

    private:

        std::vector<OscObject*> _objectList;
};

} // namespace

#endif // VISUAL_OSC_OBJECT_H
