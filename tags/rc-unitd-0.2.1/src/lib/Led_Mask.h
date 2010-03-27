/*  Led_Mask.h

  Copyright (C) 2007 Dan Wilcox

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

*/
#ifndef LED_MASK_H
#define LED_MASK_H

#include <time.h>
#include <sys/time.h>
#include <queue>

#include "Serial_Device.h"

// target settings
#define PACKET_SIZE     5
#define NUM_CHIPS       3
#define X_SIZE          13
#define Y_SIZE          14
#define NUM_PRIMITIVES  4

// physical point conversion convenience class
class conversion_point
{
    public:
        void set(int chip_index_, int x_, int y_)
        {
            chip_index = chip_index_;
            x = x_;
            y = y_;
        }

        int chip_index;
        int x;
        int y;
};

/** \class  Led Mask
    \brief  Led_Mask class inherited from Serial_Device */
class Led_Mask: public Serial_Device
{
    public:

        Led_Mask();

        virtual ~Led_Mask();

        /* Drawing Functions */

        void drawPrimitive(char type, int d0, int d1, int d2, int d3);

        void updateDisplay();

        void clear();

        void pixel(int x, int y);

        void line(int x1, int y1, int x2, int y2);

        void circle(int x, int y, int r);

        void rect(int x, int y, int h, int w);

        void box(int x, int y, int h, int w);

        /* Utility Functions */

        int getPixel(int x, int y);

        void transform(int x, int y);

        /* Low Level Functions */

        void writeBuffer(int chip_index, int x, int y, int onoff);

        int readBuffer(int chip_index, int x, int y);

        void clearBuffer();

        void setReg(int chip_index, int reg, uint8_t val);

        void setRegAll(int reg, uint8_t val);

        void syncReg(int chip_index, int reg);

        int sendPacket();

        /**
            \brief  Print all of the events?

            \param  yesno   true = print all button events
        */
        void inline printEvents(bool yesno) {print_events = yesno;};

    protected:

    private:
        queue<int> update_queue; // which registers to update to the screen
        uint8_t screen_buffer[NUM_CHIPS*8]; // screen buffer

        conversion_point c_point;       // virtual to physical point conversion class
        uint8_t packet[PACKET_SIZE];    // command packet
        bool print_events;              // printing control
};

#endif
