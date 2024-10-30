#pragma once

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

		static unsigned int GetTime();
		static double GetCurrentTime();
		static void CalculateFPS();

		static void Frame();
};