
#include "Test_Sound_Feedback.h"


int test_Sound_Feedback()
{
    cout << "Begin Sound Feedback test" << endl;

    Sound_Feedback sounds;

    string path = "sounds/";

    string sound_list[3] = {path + "dontcopythatfloppy.wav", path + "nic_loud.wav", path + "boomboom.wav"};

    for(int c = 0; c < 3; c++)
    {
        cout << "playing : " << sound_list[c] << endl;
        sounds.play(sound_list[c]);
        sleep(5);
    }

    cout << "End Sound Feedback test" << endl;

    return 0;
}
