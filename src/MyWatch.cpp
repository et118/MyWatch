#include "MyWatch.h"
#include "WifiCapProgram/WifiCapProgram.h"
#include "ClockProgram.h"
#include "HomescreenProgram.h"
#include "StopwatchProgram.h"
#include "TimerProgram.h"
RTC_DATA_ATTR int selectedProgram;
Program* programs[]{new ClockProgram("Clock"),new HomescreenProgram("Homescreen"),new StopwatchProgram("Stopwatch"),new TimerProgram("Timer"),new WifiCapProgram("WifiCap")};
int numPrograms = 5;

void MyWatch::init() {
	
	Serial.begin(115200);
	ESP_LOGI("MyWatch","Started watch");
	ttgo = TTGOClass::getWatch();
	ttgo->begin();
	ttgo->rtc->check();
	ttgo->rtc->syncToSystem();
	ttgo->setBrightness(30);
	ttgo->openBL();
	ttgo->motor_begin();
	ttgo->lvgl_begin();

	if(!selectedProgram) selectedProgram=defaultProgram;
	for(;;) {
		ESP_LOGI("MyWatch","Loading program: %s",programs[selectedProgram]->getName().c_str());
		selectedProgram = programs[selectedProgram]->run(programs, numPrograms);
	}
}