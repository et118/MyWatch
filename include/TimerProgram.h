#pragma once
#include "config.h"
#include "Program.h"
#include "GraphicsLib.h"
#include "SleepLib.h"

class TimerProgram : public Program {
	private:
		TTGOClass* ttgo;
		TFT_eSPI* tft;
		bool running;
		int powerClicks;
		const unsigned long doubleClickTime = 300;
		unsigned long startClickTime;

        Button* buttons[7];
        int h;
        int m;
        int s;
        int state;
        bool lastTouched;
        unsigned long endTime;

        void drawButtons();
        void handleButtonInputs();

		void init();
		int loop();
		void exit();
		void checkPowerIRQ();

	public:
		using Program::Program;
		int run(Program** programs, int numPrograms);
};