
#include "Test_Playlist.h"


int test_Playlist()
{
    cout << "Begin Playlist test" << endl;

    Playlist play_list("playlist.txt");

    if(play_list.load() < 0)
        return 0;

    cout << "Printing Playlist \"" << play_list.name() << "\":" << endl;
    play_list.print();

    cout << "Stepping forward through playlist" << endl;
    for(int i = 0; i < 10; i++)
    {
        cout << "   " << i << " " << play_list.song() << endl;
        play_list.next();
    }

    play_list.prev();

    cout << "Stepping backward through playlist" << endl;
    for(int i = 0; i < 10; i++)
    {
        cout << "   " << i << " " << play_list.song() << endl;
        play_list.prev();
    }

    cout << "File stripping test: " << play_list.file() << endl;

    cout << "Path stripping test: " << play_list.path() << endl;

    cout << "End Playlist test" << endl;

    return 0;
}
