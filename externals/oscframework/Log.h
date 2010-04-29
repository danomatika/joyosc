/*==============================================================================
    Dan Wilcox <danomatika@gmail.com>, 2010
==============================================================================*/
#ifndef OSC_LOG_H
#define OSC_LOG_H

#include <iostream>

#define LOG std::cout
#define LOG_DEBUG std::cout << "DEBUG: "
#define LOG_WARN std::cerr << "WARN: "
#define LOG_ERROR std::cerr << "ERROR: "

#endif // OSC_LOG_H
