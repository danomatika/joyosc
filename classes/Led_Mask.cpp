/*  Led_Mask.cpp

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
#include "Led_Mask.h"

#include <cstdlib>

// packet header byte
#define COMMAND_NEW    0xF0

// command type bytes
#define COMMAND_SET        0xE0
#define COMMAND_SET_ALL    0xD0
#define COMMAND_CLEAR      0xC0
#define COMMAND_INTENSITY  0xB0
#define COMMAND_NULL       0x00

Led_Mask::Led_Mask()
{
    packet[0] = COMMAND_NEW; // packet header, dont have to change
}

Led_Mask::~Led_Mask()
{
    //dtor
}

/* Drawing Functions */

// send the buffer to all chips
void Led_Mask::updateDisplay()
{
    // update registers in queue
    while(!update_queue.empty())
    {
        int c, p;
        c = update_queue.front();
        update_queue.pop();
        p = update_queue.front();
        update_queue.pop();

        //cout << "pop " << c << " " << p << endl;
        syncReg(c, p);
    }
}

void Led_Mask::clear()
{

    packet[1] = COMMAND_CLEAR;  // cmd type
    packet[2] = 0;              // dont care
    packet[3] = 0;              // dont care
    packet[4] = 0;              // dont care

    // send it
    sendPacket();

}

void Led_Mask::drawPrimitive(char type, int d0, int d1, int d2, int d3)
{
    switch(type)
    {
        case 'p':
            pixel(d0, d1);
            break;
        case 'l':
            line(d0, d1, d2, d3);
            break;
        case 'c':
            circle(d0, d1, d2);
            break;
        case 'r':
            rect(d0, d1, d2, d3);
            break;
        case 'b':
            box(d0, d1, d2, d3);
            break;
    }
}

void Led_Mask::pixel(int x, int y)
{
    transform(x, y);    // convert virtual to physical

    // dont add to update queue if point is already on
    //if(readBuffer(c_point.chip_index, c_point.x, c_point.y) == 0)
        writeBuffer(c_point.chip_index, c_point.x, c_point.y, 1);
}

/* an improved Bresenham line drawing alogrithm
   from http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html */
void Led_Mask::line(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    pixel(x1, y1);
    if(abs(dx) > abs(dy))     // slope < 1
    {
        int m = (int) dy / (int) dx;      // compute slope
        int b = y1 - m*x1;
        dx = (dx < 0) ? -1 : 1;
        while (x1 != x2)
        {
            x1 += dx;
            pixel(x1, m*x2 + b);
        }
    }
    else if(dy != 0)   // slope >= 1
    {
        int m = (int) dx / (int) dy;      // compute slope
        int b = x1 - m*y1;
        dy = (dy < 0) ? -1 : 1;
        while(y1 != y2)
        {
            y1 += dy;
            pixel(m*y1 + b, y1);
        }
    }
}

void Led_Mask::circle(int x, int y, int r)
 {
    int f = 1 - r;
    int ddF_x = 0;
    int ddF_y = -2 * r;
    int _x = 0;
    int _y = r;

    pixel(x, y + r);
    pixel(x, y - r);
    pixel(x + r, y);
    pixel(x - r, y);

    while(_x < _y)
    {
        if(f >= 0)
        {
            _y--;
            ddF_y += 2;
            f += ddF_y;
        }
        _x++;
        ddF_x += 2;
        f += ddF_x + 1;
        pixel(x + _x, y + _y);
        pixel(x - _x, y + _y);
        pixel(x + _x, y - _y);
        pixel(x - _x, y - _y);
        pixel(x + _y, y + _x);
        pixel(x - _y, y + _x);
        pixel(x + _y, y - _x);
        pixel(x - _y, y - _x);
    }
 }

/* rectangle algorithm using lines */
void Led_Mask::rect(int x, int y, int h, int w)
{
    line(x, y, x+w, y);
    line(x, y, x, y+h);
    line(x+w, y+h, x+w, y);
    line(x+w, y+h, w+w, y+h);
}

/* filled rectangle algorithm using lines */
void Led_Mask::box(int x, int y, int h, int w)
{
    for(int _y = y; _y < y + h; _y++)
        line(x, _y, x + (w-1), _y);
}

/* Utility Functions */

int Led_Mask::getPixel(int x, int y)
{
    transform(x, y);
    return readBuffer(c_point.chip_index, c_point.x, c_point.y);
}

void Led_Mask::transform(int x, int y)
{

  // check bounds
  if((x < 0 || x > X_SIZE) ||
     (y < 0 || y > Y_SIZE))
    return;

  if(y < 5)  // top matrix, 3rd chip
  {
    if(x > 3 && x < 10)  // center group
      c_point.set(3, x-4, y);

    else if(x < 4)  // left weird group
    {
      if(y == 3 && x != 3)
         c_point.set(3, x, 6);
      else if(y == 4)
        c_point.set(3, x+3, 6);
    }
    else  // right weird group
    {
      if(y == 3)
        switch(x)
        {
          case 11: c_point.set(3, 2, 5);
                   break;
          case 12: c_point.set(3, 1, 5);
                   break;
          case 13: c_point.set(3, 0, 5);
                   break;
        }
      else if(y == 4)
        switch(x)
        {
          case 10: c_point.set(3, 6, 5);
                   break;
          case 11: c_point.set(3, 5, 5);
                   break;
          case 12: c_point.set(3, 4, 5);
                   break;
          case 13: c_point.set(3, 3, 5);
                   break;
        }
    }
  }
  else  // left or right matrices
  {
    if(x < 7) // left matrix, 2nd chip
      c_point.set(2, x, y-5);
    else // right matrix, 1st chip
      c_point.set(1, x-7, y-5);
  }
  cout << "transform vx: " << x << " vy: " << y << " ci: " << c_point.chip_index << " px: " << c_point.x << " py: " << c_point.y << endl;
  return;
}

/* Low Level Functions */

void Led_Mask::writeBuffer(int chip_index, int x, int y, int onoff)
{
    // check bounds
    if(x >= 8 || y >= 8) return;

    // update val in buffer
    if(onoff > 0)
        screen_buffer[y+(8*chip_index)] |= 0x100 >> x+2;      // turn on
    else if(onoff == 0)
        screen_buffer[y+(8*chip_index)] &= ~(0x100 >> x+2);   // turn off
    else
        screen_buffer[y+(8*chip_index)] ^= (0x100 >> x+2);    // toggle

    update_queue.push(chip_index);
    update_queue.push(y);    // add to queue
    //cout << "push " << chip_index << " " << y << endl;
}

int Led_Mask::readBuffer(int chip_index, int x, int y)
{
  if(x >= 8 || y >= 8) return -1;

  return screen_buffer[y+(8*chip_index)] & 0x100 >> x+2;
}

void Led_Mask::clearBuffer()
{
    // clear buffer
    for(int c = 0; c < NUM_CHIPS*8; c++)
        screen_buffer[c] = 0;
}

void Led_Mask::setReg(int chip_index, int reg, uint8_t val)
{
    // escape bad index
    if(chip_index < 0 || chip_index > NUM_CHIPS)
        return;

    packet[1] = COMMAND_SET;    // cmd type
    packet[2] = chip_index;     // command index
    packet[3] = reg;            // reg
    packet[4] = val;            // val

    // send it
    sendPacket();
}

void Led_Mask::setRegAll(int reg, uint8_t val)
{
    packet[1] = COMMAND_SET_ALL;// cmd type
    packet[2] = 0;              // dont care
    packet[3] = reg;            // reg
    packet[4] = val;            // val

    // send it
    sendPacket();
}

void Led_Mask::syncReg(int chip_index, int reg)
{
    if(reg < 0 || reg >= 8) return;

    setReg(chip_index, reg+1, screen_buffer[reg+(8*chip_index)]);
}

int Led_Mask::sendPacket()
{
    // send it
    int ret;
    if((ret = send((unsigned char*) packet, sizeof(packet))) < 0)
        return ret;
/*
    if(print_events)
    {
        cout << endl << "led_mask sent packet: " << endl;
        printf("    header  0x%X\n",packet[0]);
        printf("    cmd    0x%X\n",packet[1]);
        printf("    chip   %d\n",  packet[2]);
        printf("    reg      %d\n", packet[3]);
        printf("    val      %d\n",  packet[4]);
        cout << endl;
    }
*/
    return ret;
}

/*
void Led_Mask::pixel(int cmd_index, int x, int y)
{
    // escape bad index
    if(cmd_index < 0 || cmd_index > MAX_CMDS)
        return;

    // escape bad vals
    if((x < 0 || x > NUM_X) ||
       (y < 0 || y > NUM_Y))
       return;

    packet[1] = cmd_index;      // command index
    packet[2] = COMMAND_PIXEL;  // type
    packet[3] = x;              // x
    packet[4] = y;              // y
    packet[5] = 0;              // dont care
    packet[6] = 0;              // dont care

    // send it
    sendPacket();
}

void Led_Mask::line(int cmd_index, int x1, int x2, int y1, int y2)
{
    // escape bad index
    if(cmd_index < 0 || cmd_index > MAX_CMDS)
        return;

    // escape bad vals
    if((x1 < 0 || x1 > NUM_X) ||
       (y1 < 0 || y1 > NUM_Y) ||
       (x2 < 0 || x2 > NUM_X) ||
       (y2 < 0 || y2 > NUM_Y))
       return;

    packet[1] = cmd_index;      // command index
    packet[2] = COMMAND_LINE;   // type
    packet[3] = x1;             // x1
    packet[4] = y1;             // y1
    packet[5] = x2;             // x2
    packet[6] = y2;             // y2

    // send it
    sendPacket();
}

void Led_Mask::rect(int cmd_index, int x, int y, int w, int h)
{
    // escape bad index
    if(cmd_index < 0 || cmd_index > MAX_CMDS)
        return;

    // escape bad vals
    if((x < 0 || x > NUM_X) ||
       (y < 0 || y > NUM_Y) ||
       (w < 0 || w > NUM_X) ||
       (h < 0 || h > NUM_Y))
       return;

    packet[1] = cmd_index;      // command index
    packet[2] = COMMAND_RECT;   // type
    packet[3] = x;              // x
    packet[4] = y;              // y
    packet[5] = w;              // width
    packet[6] = h;              // height

    // send it
    sendPacket();
}

void Led_Mask::circle(int cmd_index, int x, int y, int r)
{
    // escape bad index
    if(cmd_index < 0 || cmd_index > MAX_CMDS)
        return;

    // escape bad vals
    if((x < 0 || x > NUM_X) ||
       (y < 0 || y > NUM_Y) ||
       (r < 0 || r > NUM_X))
       return;

    packet[1] = cmd_index;      // command index
    packet[2] = COMMAND_CIRC;   // type
    packet[3] = x;              // x
    packet[4] = y;              // y
    packet[5] = r;              // radius
    packet[6] = 0;              // dont care

    // send it
    sendPacket();
}

void Led_Mask::clear(int cmd_index)
{
    // escape bad index
    if(cmd_index < 0 || cmd_index > MAX_CMDS)
        return;

    packet[1] = cmd_index;      // command index
    packet[2] = COMMAND_NULL;   // type
    packet[3] = 0;              // dont care
    packet[4] = 0;              // dont care
    packet[5] = 0;              // dont care
    packet[6] = 0;              // dont care

    // send it
    sendPacket();
}
*/
