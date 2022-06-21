#pragma once
#include "config.h"
#include "Program.h"
#include "GraphicsLib.h"
#include "SleepLib.h"

class StopwatchProgram : public Program {
	private:
		TTGOClass* ttgo;
		TFT_eSPI* tft;
		bool running;
		int powerClicks;
		const unsigned long doubleClickTime = 300;
		unsigned long startClickTime;

        int h;
        int m;
        int s;
        int ms;
        int state;
        long startMillis;
        long mill;
        bool lastTouched;

		void init();
		int loop();
		void exit();
		void checkPowerIRQ();

	public:
		using Program::Program;
		int run(Program** programs, int numPrograms);
};