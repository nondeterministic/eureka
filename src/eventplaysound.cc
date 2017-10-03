#include "eventplaysound.hh"

// loop -1 is infinite loop, everything else n times.

EventPlaySound::EventPlaySound(std::string fn, int loop_how_many_times, int vol)
{
	filename = fn;
	loop = loop_how_many_times;
	volume = vol;
}

EventPlaySound::~EventPlaySound() { }

// TODO: Make this own class!

EventPlayMusic::EventPlayMusic(std::string fn, int loop_how_many_times, int vol)
{
	filename = fn;
	loop = loop_how_many_times;
	volume = vol;
}

EventPlayMusic::~EventPlayMusic() { }
