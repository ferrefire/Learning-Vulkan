#pragma once

#include <string>

//#define START_TIMER(name) double name; Time::StartTimer(name);
//#define STOP_TIMER(name, newline) Time::StopTimer(name, #name, newline);

#define START_TIMER(name)
#define STOP_TIMER(name, newline)

class Time
{
    private:
        

    public:
        static float deltaTime;
        static float currentFrame;
        static float lastFrame;

		static double timeLastSecond;
		static double timeLastTick;
		static double timeLastSubTick;
		static double timeLastFrameTick;

		static int frameCountThisTick;
		static float frameTimeThisTick;
		static int frameCountLastTick;
		static float frameTimeLastTick;

		static bool newSecond;
		static bool newTick;
		static bool newSubTick;
		static bool newFrameTick;

		static int currentFPS;

		static double currentTimer;

		static unsigned int GetTime();
		static double GetCurrentTime();
		static void CalculateFPS();
		static double StartTimer(double &timer);
		static double StartTimer();
		static double StopTimer(double timer, std::string message, bool newline = false);
		static double StopTimer(double timer);
		static double StopTimer(std::string message);
		static double StopTimer();

		static void Frame();
};