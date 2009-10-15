/*  Config.cpp

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
#include "Config.h"

Config::Config(const char *filename_)
{
    filename = filename_;
}

Config::~Config()
{
    //dtor
}

int Config::load()
{
    ifstream fin(filename, ios::in);

    if(!fin)    // open failed
    {
        cerr << "Config error: error opening file \"" << filename << "\"" << endl;
        return -1;
    }

    //cout << "Config: loading " << filename << endl;

    string s;
    while(getline(fin, s))
    {
        if(s.size() >= 1 && s[0] == '#') {}
      //      cout << "   ignoring comment: " << s << endl;
        else if(s.size() >= 2)
        {
            istringstream ss(s);
            string key, val;
            ss >> key;
            ss.ignore();    // remove preceeding space
            getline(ss, val);


            //cout << "   key: \"" << key << "\" val: \"" << val << "\"" << endl;
            config_map.insert(make_pair(key, val));
        }
    }

    //cout << "Config: ready" << endl;
    fin.close();

    return 0;
}

int Config::loadJoy()
{
    ifstream fin(filename, ios::in);

    if(!fin)    // open failed
    {
        cerr << "Config error: error opening file \"" << filename << "\"" << endl;
        return -1;
    }

    //cout << "Config: loading " << filename << endl;

    string s;
    while(getline(fin, s))
    {
        if(s.size() >= 1 && s[0] == '#') {}
      //      cout << "   ignoring comment: " << s << endl;
        else if(s.size() >= 2)
        {
            string key, val;

            key = s.substr(s.find_first_of("\"")+1, s.find_last_of("\"")-1);
            val = s.substr(s.find_last_of("\"")+2, s.size());

            //cout << "   key: \"" << key << "\" val: \"" << val << "\"" << endl;
            config_map.insert(make_pair(key, val));
        }
    }

    //cout << "Config: ready" << endl;
    fin.close();

    return 0;
}

string Config::get(const char* key)
{
    return config_map[key];
}

void Config::print()
{
        int i = 0;

        map<string, string>::iterator c;
        for(c = config_map.begin(); c != config_map.end(); c++)
        {
            cout << "   " << i << " " << c->first << " = " << c->second << endl;
            i++;
        }
}
