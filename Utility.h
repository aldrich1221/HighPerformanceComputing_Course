#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
using namespace std;

enum HoldDuration{holdDurationOff = 0, holdDurationOn = 1};
enum IgnoreRepeat{ignoreRepeatOff = 0, ignoreRepeatOn = 1};
enum TapAssistance{tapAssistanceOff = 0, tapAssistanceInit = 1, tapAssistanceFinal = 2};

struct TouchAccommodationsPara{
	HoldDuration holdDuration;
	IgnoreRepeat ignoreRepeat;
	TapAssistance tapAssistance;
	double holdDurationTimer;
	double ignoreRepeatTimer;
	double tapAssistanceTimer;
};

ostream& operator<<(ostream& os, const TouchAccommodationsPara& para);

#endif