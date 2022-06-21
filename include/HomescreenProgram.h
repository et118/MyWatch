#pragma once
#include "config.h"
#include "Program.h"
#include "GraphicsLib.h"
#include "math.h"

class HomescreenProgram : public Program {
	private:
		Program** programs;
		TTGOClass* ttgo;
		TFT_eSPI* tft;
		TextButton* buttons[6] = {
			new TextButton(0,0,120,60,1,"Previous",TFT_LIGHTGREY,TFT_BLACK),
			new TextButton(120,0,120,60,1,"Next",TFT_LIGHTGREY,TFT_BLACK),

			new TextButton(0,60,120,90,1,"None1",TFT_DARKGREY,TFT_BLACK),
			new TextButton(120,60,120,90,1,"None2",TFT_DARKGREY,TFT_BLACK),
			new TextButton(0,150,120,90,1,"None3",TFT_DARKGREY,TFT_BLACK),
			new TextButton(120,150,120,90,1,"None4",TFT_DARKGREY,TFT_BLACK)
		};
		int numPrograms;
		int currentPage;
		void createAppScreen();

		void init();
		int loop();
		void exit();

	public:
		using Program::Program;
		int run(Program** programs, int numPrograms);
};