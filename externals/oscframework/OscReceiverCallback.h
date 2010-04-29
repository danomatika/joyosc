/*
 *  OscReceiverCallback.h
 *  oscframework
 *
 *  Created by Dan Wilcox on 2/20/10.
 *  Copyright 2010 Robotcowboy Industries. All rights reserved.
 *
 */

#include <lo/lo.h>

class OscReciverCallback
{

	public:
    
    	OscReciverCallback() {}
        
        static int messageHandler(const char* path, const char* types, lo_arg** argv,
		    				  int argc, void* data, void* user_data);

};
