/*==============================================================================
    Dan Wilcox <Daniel.Wilcox@aec.at>, 2009
==============================================================================*/
#include "OscObject.h"

#include "Log.h"

#include <algorithm>

namespace osc {

bool OscObject::processOsc(const ReceivedMessage& message, const MessageSource& source)
{
    // call any attached objects
    std::vector<OscObject*>::iterator objectIter;
    for(objectIter = _objectList.begin(); objectIter != _objectList.end();)
    {
        // try to process message
        if((*objectIter) != NULL)
        {
            if((*objectIter)->processOsc(message, source))
                return true;

            objectIter++; // increment iter
        }
        else    // bad object, so erase it
        {
            objectIter = _objectList.erase(objectIter);
            LOG_WARN << "OscObject: removed NULL object" <<std::endl;
        }
    }

    return processOscMessage(message, source);
}

void OscObject::addOscObject(OscObject* object)
{
    if(object == NULL)
    {
        LOG_WARN << "OscObject: Cannot add NULL object" << std::endl;
        return;
    }

    _objectList.push_back(object);
}

void OscObject::removeOscObject(OscObject* object)
{
    if(object == NULL)
    {
        LOG_WARN << "OscObject: Cannot remove NULL object" << std::endl;
        return;
    }

    // find object in list and remove it
    std::vector<OscObject*>::iterator iter;
    iter = find(_objectList.begin(), _objectList.end(), object);
    if(iter != _objectList.end())
    {
        _objectList.erase(iter);
    }
}

} // namespace
