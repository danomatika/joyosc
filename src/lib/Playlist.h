/*  Playlist.h

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
#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

/** \class  Playlist
    \brief  Loads and handles a circular play list of filenames
*/
class Playlist
{
    public:

        /** \brief  Constructor

            \param  filename of the playlist file to open, including path
        */
        Playlist(char *filename_);

        virtual ~Playlist();

        /** \brief  Load the playlist

            Ignores lines beginning with '#',
            format is one file per line, including full path

            ex.
            # this is a comment
            /home/user/awesomesong.pd

            returns 0 on success or -1 if the file cannot be loaded (i.e. found)

            Note: very dumb, just reads lines, so watch the whitespace in the playlist file
        */
        int load();

        /**  \brief  Return the current song filename

            Includes fullpath: /home/user/awesomesong.pd
        */
        inline string song() {return *pos;};

        /** \brief  Returns the current song filename

            Does not include path: awesomesong.pd
        */
        string file();

        /** \brief  Returns the current song path

            Does not include filename: /home/user/
        */
        string path();

        /** \brief  Move to the next song in the playlist */
        void next();

        /** \brief  Move to the previous song in the playlist */
        void prev();

        /** \brief  Print the playlist */
        void print();

        /** \brief  Returns the playlist file filename */
        inline char *name() {return filename;};

    protected:

    private:

        char *filename;                 // playlist filename
        vector<string> list;            // string vector of playlist items
        vector<string>::iterator pos;   // current position in the playlist
};

#endif // PLAYLIST_H
