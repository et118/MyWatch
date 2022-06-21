#include "HomescreenProgram.h"

int HomescreenProgram::run(Program** programs, int numPrograms) {
	this->programs = programs;
	this->ttgo = TTGOClass::getWatch();
	this->tft = this->ttgo->tft;
	this->numPrograms = numPrograms;
	this->currentPage = 0;
	this->init();
	return this->loop();
}

void HomescreenProgram::createAppScreen() {
	tft->fillScreen(TFT_BLACK);

	for(int i = 2; i < 6; i++) {
		if(currentPage*4+i-2 >= numPrograms) {
			buttons[i]->setText("None");
		} else {
			buttons[i]->setText(programs[currentPage*4+i-2]->getName().c_str());
		}	
	}

	for(int i = 0; i < 6; i++) {
		buttons[i]->draw(tft);
	}
}

void HomescreenProgram::init() {
	createAppScreen();
}

int HomescreenProgram::loop() {
	int lastTouched = 0;
	int16_t x = 0;
	int16_t y = 0;
	for(;;) {
		bool touched = ttgo->touched();
		if(!touched && (lastTouched > 2)) { // On release
			for(int i = 0; i < 6; i++) {
				if(buttons[i]->inBounds(x,y)) {
					switch(i) {
						case 0:
							currentPage++;
							if(currentPage > ceil((float)numPrograms/4.0)-1) currentPage = 0;
							ESP_LOGI("HomescreenProgram","Changing to page: %i", currentPage);
							createAppScreen();
							break;
						case 1:
							currentPage--;
							if(currentPage < 0) currentPage = ceil((float)numPrograms/4.0)-1;
							ESP_LOGI("HomescreenProgram","Changing to page: %i", currentPage);
							createAppScreen();
							break;
						default:
							if(currentPage*4+i-2 < numPrograms) {
								ESP_LOGI("HomescreenProgram","Starting program: %s",programs[currentPage*4+i-2]->getName().c_str());
								exit();
								return currentPage*4+i-2; 
							} else {
								ESP_LOGI("HomescreenProgram","No program clicked");
							}
							break;
					}
				}
			}
		}

		if(touched) {
			ttgo->getTouch(x,y);
			lastTouched++;
		} else {
			lastTouched = 0;
		}
	}
}

void HomescreenProgram::exit() {
	// We dont need to unload the textbuttons from memory yet
	// because they don't really take up that much space
	// Neither have we created anything or started any 
	// functions such as bluetooth or wifi so we can just return
	return;
}