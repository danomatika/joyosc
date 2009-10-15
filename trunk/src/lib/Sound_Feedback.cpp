/*  Sound_Feedback.cpp

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
#include "Sound_Feedback.h"

Sound_Feedback::Sound_Feedback()
{
    //ctor
}

Sound_Feedback::~Sound_Feedback()
{
    //dtor
}

int Sound_Feedback::play(string filename)
{
    //cout << "alsaplayer -i text -o jack " + filename << endl;
    sound_player.setApp("alsaplayer -i text -o jack " + filename);
    sound_player.launch();

    return 0;
}
