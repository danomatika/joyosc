/*  Sound_Feedback.h

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
#ifndef SOUND_FEEDBACK_H
#define SOUND_FEEDBACK_H

#include <string>
#include "Application.h"

/** \class  Sound_Feedback
    \brief  Plays soundfiles

    Calls alsaplayer with jack output (alsaplayer -i text -o jack)
    to play sound files
*/
class Sound_Feedback
{
    public:

        Sound_Feedback();

        virtual ~Sound_Feedback();

        /**
            \brief  Plays a soundfile using alsaplayer
            \param  filename    name of the soundfile to play
        */
        int play(string filename);

    protected:

    private:
        Application sound_player;
};



#endif // SOUND_FEEDBACK_H
