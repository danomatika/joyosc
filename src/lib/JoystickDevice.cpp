/*==============================================================================

	JoystickDevice.cpp

	rc-unitd: a device event to osc bridge
  
	Copyright (C) 2007, 2010  Dan Wilcox <danomatika@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#include "JoystickDevice.h"

using namespace xml;

JoystickDevice::JoystickDevice(string oscAddress) :
	Device(oscAddress), m_joystick(NULL), m_joyIndex(-1), m_instanceID(-1),
	m_axisDeadZone(0), m_remapping(NULL), m_ignore(NULL) {}

bool JoystickDevice::open() {
	if(m_joyIndex == -1) {
		LOG_WARN << "JoystickDevice: cannot open, index not set" << endl;
		return false;
	}

	if(isOpen()) {
		LOG_WARN << "JoystickDevice: joystick with index "
				 << m_joyIndex << " already opened" << endl;
		return false;
	}

	m_joystick = SDL_JoystickOpen(m_joyIndex);
	if(!m_joystick) {
		LOG_WARN << "JoystickDevice: open failed for index " << m_joyIndex << endl;
		return false;
	}
	
	m_instanceID = SDL_JoystickInstanceID(m_joystick);
	m_devName = SDL_JoystickName(m_joystick);
	
	// try to set the address from the mapping list using the dev name
	m_oscAddress = m_config.getDeviceAddress((string) m_devName);
	if(m_oscAddress == "") {
		// not found ... set a generic name using the index
		stringstream stream;
		stream << "/js" << m_joyIndex;
		m_oscAddress = stream.str();
	}
			
	// create prev axis values
	for(int i = 0; i < SDL_JoystickNumAxes(m_joystick); ++i) {
		m_prevAxisValues.push_back(0);
	}
	
	LOG << "JoystickDevice: opened " << m_joyIndex
		<< " \"" << m_devName << "\" with address "
		<< m_oscAddress << endl;
	return true;
}

void JoystickDevice::close() {
	if(m_joystick) {
		if(isOpen()) {
			SDL_JoystickClose(m_joystick);
		}
		LOG << "JoystickDevice: closed " << m_joyIndex
			<< " \"" << m_devName << "\" with address "
			<< m_oscAddress << endl;
		m_joystick = NULL;
	}
	
	// reset variables
	m_joyIndex = -1;
	m_instanceID = -1;
	m_devName = "";
	m_prevAxisValues.clear();
}

bool JoystickDevice::handleEvents(void* data) {
	if(data == NULL) {
		return false;
	}
	
	osc::OscSender& sender = m_config.getOscSender();
	
	SDL_Event* event = (SDL_Event*) data;
	switch(event->type) {
		
		case SDL_JOYBUTTONDOWN: {	
			if(event->jbutton.which != m_instanceID) {
				break;
			}
			
			// ignore?
			if(m_ignore) {
				map<int,bool>::iterator iter = m_ignore->buttons.find(event->jbutton.button);
				if(iter != m_ignore->buttons.end()) {
					break;
				}
			}
			
			// remap?
			if(m_remapping) {				
				map<int,int>::iterator iter = m_remapping->buttons.find(event->jbutton.button);
				if(iter != m_remapping->buttons.end()) {
					event->jbutton.button = iter->second;
				}
			}
							
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/button")
				   << event->jbutton.button << event->jbutton.state
				   << osc::EndMessage();
			sender.send();

			if(m_config.bPrintEvents) {
				LOG << m_oscAddress << " \"" << m_devName << "\""
					<< " Button: " << (int) event->jbutton.button
					<< " State: " << (int) event->jbutton.state << endl;
			}
			return true;
		}
		
		case SDL_JOYBUTTONUP: {
			if(event->jbutton.which != m_instanceID) {
				break;
			}
				
			// ignore?
			if(m_ignore) {
				map<int,bool>::iterator iter = m_ignore->buttons.find(event->jbutton.button);
				if(iter != m_ignore->buttons.end()) {
					break;
				}
			}
				
			// remap?
			if(m_remapping) {
				map<int,int>::iterator iter = m_remapping->buttons.find(event->jbutton.button);
				if(iter != m_remapping->buttons.end()) {
					event->jbutton.button = iter->second;
				}
			}
							
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/button")
				   << event->jbutton.button << event->jbutton.state
				   << osc::EndMessage();
			sender.send();

			if(m_config.bPrintEvents) {
				LOG << m_oscAddress << " \"" << m_devName << "\""
					<< " Button: " << (int) event->jbutton.button
					<< " State: " << (int) event->jbutton.state << endl;
			}
			return true;
		}

		case SDL_JOYAXISMOTION: {
			if(event->jaxis.which != m_instanceID) {
				break;
			}
			
			// ignore?
			if(m_ignore) {
				map<int,bool>::iterator iter = m_ignore->axes.find(event->jaxis.axis);
				if(iter != m_ignore->axes.end()) {
					break;
				}
			}
			
			// remap?
			if(m_remapping) {
				map<int,int>::iterator iter = m_remapping->axes.find(event->jaxis.axis);
				if(iter != m_remapping->axes.end()) {
					event->jaxis.axis = iter->second;
				}
			}
			
			int value = (int) event->jaxis.value;
			 
			// handle jitter by creating a dead zone
			if(abs(value) < m_axisDeadZone) {
				value = 0;
			}
				   
			// make sure we don't report a value more then once
			if(m_prevAxisValues[event->jaxis.axis] == value) {
				return true;
			}
			
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/axis")
				   << event->jaxis.axis << value
				   << osc::EndMessage();
			sender.send();
			
			// store value
			m_prevAxisValues[event->jaxis.axis] = value;

			if(m_config.bPrintEvents) {
				LOG << m_oscAddress << " \"" << m_devName << "\""
					<< " Axis: " << (int) event->jaxis.axis
					<< " Value: " << value << endl;
			}
			return true;
		}
		
		case SDL_JOYBALLMOTION: {
			if(event->jball.which != m_instanceID) {
				break;
			}
			
			// ignore?
			if(m_ignore) {
				map<int,bool>::iterator iter = m_ignore->balls.find(event->jball.ball);
				if(iter != m_ignore->balls.end()) {
					break;
				}
			}
			
			// remap?
			if(m_remapping) {
				map<int,int>::iterator iter = m_remapping->balls.find(event->jball.ball);
				if(iter != m_remapping->balls.end()) {
					event->jball.ball = iter->second;
				}
			}
				
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/ball")
				   << event->jball.ball << event->jball.xrel << event->jball.yrel
				   << osc::EndMessage();
			sender.send();

			if(m_config.bPrintEvents) {
				LOG << m_oscAddress << " \"" << m_devName << "\""
					<< " Ball: " << (int) event->jaxis.axis
					<< " Motion: " << (int)event->jball.xrel 
					<< " " << (int) event->jball.yrel << endl;
			}
			return true;
		}
		
		case SDL_JOYHATMOTION: {
			if(event->jhat.which != m_instanceID) {
				break;
			}
			
			// ignore?
			if(m_ignore) {
				map<int,bool>::iterator iter = m_ignore->hats.find(event->jhat.hat);
				if(iter != m_ignore->hats.end()) {
					break;
				}
			}
			
			// remap?
			if(m_remapping) {
				map<int,int>::iterator iter = m_remapping->hats.find(event->jhat.hat);
				if(iter != m_remapping->hats.end()) {
					event->jhat.hat = iter->second;
				}
			}
				
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/hat")
				   << event->jhat.hat << event->jhat.value
				   << osc::EndMessage();
			sender.send();

			if(m_config.bPrintEvents) {
				LOG << m_oscAddress << " \"" << m_devName << "\""
					<< " Hat: " << (int) event->jhat.hat
					<< " Value: " << (int) event->jhat.value << endl;
			}
			return true;
		}
	}
	
	return false;
}

void JoystickDevice::print() {
	LOG << "osc addr: "  << m_oscAddress << endl
		<< "	dev name: " << m_devName << endl
		<< "	index: " << m_joyIndex << endl;
	if(m_joystick) {
		LOG << "	num buttons: " << SDL_JoystickNumButtons(m_joystick) << endl
			<< "	num axes: " << SDL_JoystickNumAxes(m_joystick) << endl
			<< "	num balls: " << SDL_JoystickNumBalls(m_joystick) << endl
			<< "	num hats: " << SDL_JoystickNumHats(m_joystick) << endl;
	}
}
	
void JoystickDevice::printRemapping() {
	if(m_joystick && m_remapping) {
		m_remapping->print();
	}
}

void JoystickDevice::printIgnores() {
	if(m_joystick && m_ignore) {
		m_ignore->print();
	}
}

bool JoystickDevice::isOpen() {
	return SDL_JoystickGetAttached(m_joystick) == SDL_TRUE;
}

SDL_JoystickID JoystickDevice::getInstanceID() {
	return m_instanceID;
}

void JoystickDevice::setAxisDeadZone(unsigned int zone) {
	m_axisDeadZone = zone;
	LOG_DEBUG << "JoystickDevice \"" << getDevName() << "\": "
			  << "set axis dead zone to " << zone << endl;
}

void JoystickDevice::setRemapping(JoystickRemapping* remapping) {
	m_remapping = remapping;
	if(m_remapping) {
		m_remapping->check(this);
	}
}

void JoystickDevice::setIgnore(JoystickIgnore* ignore) {
	m_ignore = ignore;
	if(m_ignore) {
		m_ignore->check(this);
	}
}

/* ***** JoystickRemapping ***** */

void JoystickRemapping::check(JoystickDevice* joystick) {
	if(!joystick){
		return;
	}

	map<int,int>::iterator iter;
	int numButtons = SDL_JoystickNumButtons(joystick->m_joystick);
	for(iter = buttons.begin(); iter != buttons.end(); ++iter) {
		if(iter->first > numButtons || iter->second > numButtons) {
			LOG_WARN << "JoystickRemapping for \"" << joystick->getDevName() << "\": "
					 << "removing invalid button remap: "
					 << iter->first << " -> " << iter->second;
		}
	}
	
	int numAxes = SDL_JoystickNumAxes(joystick->m_joystick);
	for(iter = axes.begin(); iter != axes.end(); ++iter) {
		if(iter->first > numAxes || iter->second > numAxes) {
			LOG_WARN << "JoystickRemapping for \"" << joystick->getDevName() << "\": "
					 << "removing invalid axis remap: "
					 << iter->first << " -> " << iter->second;
		}
	}
	
	int numBalls = SDL_JoystickNumBalls(joystick->m_joystick);
	for(iter = balls.begin(); iter != balls.end(); ++iter) {
		if(iter->first > numBalls || iter->second > numBalls) {
			LOG_WARN << "JoystickRemapping for \"" << joystick->getDevName() << "\": "
					 << "removing invalid ball remap: "
					 << iter->first << " -> " << iter->second;
		}
	}
	
	int numHats = SDL_JoystickNumButtons(joystick->m_joystick);
	for(iter = hats.begin(); iter != hats.end(); ++iter) {
		if(iter->first > numHats || iter->second > numHats) {
			LOG_WARN << "JoystickRemapping for \"" << joystick->getDevName() << "\": "
					 << "removing invalid hat remap: "
					 << iter->first << " -> " << iter->second;
		}
	}
}

void JoystickRemapping::print() {

	if(!buttons.empty()) {
		map<int,int>::iterator iter = buttons.begin();
		for(; iter != buttons.end(); ++iter) {
			LOG << "	button remap: " << iter->first
				<< " -> " << iter->second << endl;
		}
	}
	
	if(!axes.empty()) {
		map<int,int>::iterator iter = axes.begin();
		for(; iter != axes.end(); ++iter) {
			LOG << "	axis remap: " << iter->first
				<< " -> " << iter->second << endl;
		}
	}
	
	if(!balls.empty()) {
		map<int,int>::iterator iter = balls.begin();
		for(; iter != balls.end(); ++iter) {
			LOG << "	ball remap: " << iter->first
				<< " -> " << iter->second << endl;
		}
	}
	
	if(!hats.empty()) {
		map<int,int>::iterator iter = hats.begin();
		for(; iter != hats.end(); ++iter) {
			LOG << "	hat remap: " << iter->first
				<< " -> " << iter->second << endl;
		}
	}
}

bool JoystickRemapping::readXml(TiXmlElement* e) {

	bool loaded = false;
	string devName = Xml::getAttrString(e->Parent()->ToElement(), "name", "unknown");
	
	// load the device mappings
	TiXmlElement* child = e->FirstChildElement();
	while(child != NULL) {
		int from  = Xml::getAttrInt(child, "from", -1);
		int to = Xml::getAttrInt(child, "to", -1);
		
		if(from > -1 && to > -1) {
			if(child->ValueStr() == "button") {
				pair<map<int,int>::iterator, bool> ret;
				ret = buttons.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "mapping for button " << from
							 << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "remapped button " << from << " to " << to << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "axis") {
				pair<map<int,int>::iterator, bool> ret;
				ret = axes.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "mapping for axis " << from
							 << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "remapped axis " << from << " to " << to << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "ball") {
				pair<map<int,int>::iterator, bool> ret;
				ret = balls.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "mapping for ball " << from
							 << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "remapped ball " << from << " to " << to << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "hat") {
				pair<map<int,int>::iterator, bool> ret;
				ret = balls.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "mapping for hat " << from
							 << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "remapped hat " << from << " to " << to << endl;
				loaded = true;
			}
		}
		else {
			LOG_WARN << "Joystick \"" << devName << "\": "
					 << "ignoring invalid remapping";
		}

		child = child->NextSiblingElement();
	}
	
	return loaded;
}

/* ***** JoystickIgnore ***** */

void JoystickIgnore::check(JoystickDevice* joystick) {
	if(!joystick) {
		return;
	}

	map<int,bool>::iterator iter;
	int numButtons = SDL_JoystickNumButtons(joystick->m_joystick);
	for(iter = buttons.begin(); iter != buttons.end(); ++iter) {
		if(iter->first > numButtons) {
			LOG_WARN << "JoystickIgnore for \"" << joystick->getDevName() << "\": "
					 << "removing invalid button: " << iter->first;
		}
	}
	
	int numAxes = SDL_JoystickNumAxes(joystick->m_joystick);
	for(iter = axes.begin(); iter != axes.end(); ++iter) {
		if(iter->first > numAxes ) {
			LOG_WARN << "JoystickIgnore for \"" << joystick->getDevName() << "\": "
					 << "removing invalid axis: " << iter->first;
		}
	}
	
	int numBalls = SDL_JoystickNumBalls(joystick->m_joystick);
	for(iter = balls.begin(); iter != balls.end(); ++iter) {
		if(iter->first > numBalls) {
			LOG_WARN << "JoystickIgnore for \"" << joystick->getDevName() << "\": "
					 << "removing invalid ball: " << iter->first;
		}
	}
	
	int numHats = SDL_JoystickNumButtons(joystick->m_joystick);
	for(iter = hats.begin(); iter != hats.end(); ++iter) {
		if(iter->first > numHats) {
			LOG_WARN << "JoystickIgnore for \"" << joystick->getDevName() << "\": "
					 << "removing invalid hat: " << iter->first;
		}
	}
}

void JoystickIgnore::print() {
	
	if(!buttons.empty()){
		map<int,bool>::iterator iter = buttons.begin();
		for(; iter != buttons.end(); ++iter) {
			LOG << "	ignore button: " << iter->first << endl;
		}
	}
	
	if(!axes.empty()) {
		map<int,bool>::iterator iter = axes.begin();
		for(; iter != axes.end(); ++iter) {
			LOG << "	ignore axis: " << iter->first << endl;
		}
	}
	
	if(!balls.empty()) {
		map<int,bool>::iterator iter = balls.begin();
		for(; iter != balls.end(); ++iter) {
			LOG << "	ignore ball: " << iter->first << endl;
		}
	}
	
	if(!hats.empty()) {
		map<int,bool>::iterator iter = hats.begin();
		for(; iter != hats.end(); ++iter)
		{
			LOG << "	ignore hat: " << iter->first << endl;
		}
	}
}

bool JoystickIgnore::readXml(TiXmlElement* e) {

	bool loaded = false;
	string devName = Xml::getAttrString(e->Parent()->ToElement(), "name", "unknown");
	
	// load the device mappings
	TiXmlElement* child = e->FirstChildElement();
	while(child != NULL) {
		int which  = Xml::getAttrInt(child, "id", -1);
		
		if(which > -1) {
			if(child->ValueStr() == "button") {
				pair<map<int,bool>::iterator, bool> ret;
				ret = buttons.insert(make_pair(which,true));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "already ignoring button " << which << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "ignoring button " << which << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "axis") {
				pair<map<int,bool>::iterator, bool> ret;
				ret = axes.insert(make_pair(which, true));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "already ignoring axis " << which << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "ignoring axis " << which << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "ball") {
				pair<map<int,bool>::iterator, bool> ret;
				ret = balls.insert(make_pair(which, true));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "already ignoring ball " << which << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "ignoring ball " << which << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "hat") {
				pair<map<int,bool>::iterator, bool> ret;
				ret = balls.insert(make_pair(which, true));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "already ignoring hat " << which << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "ignoring hat " << which << endl;
				loaded = true;
			}
		}
		else {
			LOG_WARN << "Joystick \"" << devName << "\": "
					 << "ignoring invalid ignore";
		}

		child = child->NextSiblingElement();
	}
	
	return loaded;
}
