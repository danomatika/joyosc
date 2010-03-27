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
#include "Playlist.h"

Playlist::Playlist(char *filename_)
{
    filename = filename_;
    pos = list.begin();
}

Playlist::~Playlist()
{
    //dtor
}

int Playlist::load()
{
    ifstream fin(filename, ios::in);

    if(!fin)    // open failed
    {
        cout << "Playlist: error opening file \"" << filename << "\"" << endl;
        return -1;
    }

    cout << "Playlist: loading " << filename << endl;

    string s;
    int i = 0;
    while(getline(fin, s))
    {
        if(s.size() >= 1 && s[0] == '#') {}
      //      cout << "   ignoring comment: " << s << endl;
        else if(s.size() >= 2)
        {

                cout << "   Path " << i << " " << s << endl;
                list.push_back(s);
                i++;
        }
    }

    cout << "Playlist: ready" << endl;
    pos = list.begin();
    fin.close();

    return 0;
}

string Playlist::file()
{
    int loc = pos->find_last_of("/") + 1;

    return pos->substr(loc, pos->size());
}

string Playlist::path()
{
    int loc = pos->find_last_of("/");

    return pos->substr(0, loc);
}

void Playlist::next()
{
    pos++;
    if(pos == list.end())   pos = list.begin();
}

void Playlist::prev()
{
    if(pos == list.begin())
        pos = list.end()-1;
    else
        pos--;
}

void Playlist::print()
{
        int i = 0;
        for(vector<string>::iterator c = list.begin(); c < list.end(); c++)
        {
            cout << i << " " << *c << endl;
            i++;
        }
}
