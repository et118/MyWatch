#include "ClockProgram.h"

int ClockProgram::run(Program** programs, int numPrograms) {
	this->ttgo = TTGOClass::getWatch();
	this->tft = this->ttgo->tft;
	this->init();
	return this->loop();
}

void ClockProgram::init() {
	tft->fillScreen(TFT_BLACK);
	this->running = true;
	this->powerClicks = 0;
	this->startClickTime = 0;
	pinMode(AXP202_INT,INPUT_PULLUP);
	attachInterrupt(AXP202_INT,handleInterrupt,FALLING);
	ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,true);
	ttgo->power->clearIRQ();
}

int ClockProgram::loop() {
	while(running) {
		tft->setTextColor(TFT_WHITE,TFT_BLACK);
		tft->setTextSize(4);
		RTC_Date d = ttgo->rtc->getDateTime();
		int h = d.hour;
		int m = d.minute;
		int s = d.second;
		tft->setTextDatum(CC_DATUM);
		String time;

		if(h>=10) {time += (String)h;
		}else{time += "0"+(String)h;}
		if(m>=10) {time += ":"+(String)m;
		}else{time += ":0"+(String)m;}
		if(s>=10) {time += ":"+(String)s;}
		else{time += ":0"+(String)s;}
		
		tft->drawString(time,120,120);
		tft->setTextSize(3);
		tft->drawString(" " + (String)ttgo->power->getBattPercentage()+"%",200,30);
		checkPowerIRQ();
	}
	exit();
	return 1;
}

void ClockProgram::checkPowerIRQ() {
	if(isPowerIRQPressed) {
            ttgo->power->readIRQ();
            if(ttgo->power->isPEKShortPressIRQ()) {
                powerClicks++;
                startClickTime = millis();
                if(powerClicks == 3) {
                    running = false;
                    ESP_LOGI("ClockProgram","To homescreenprogram");
                    startClickTime = 0;
                    powerClicks = 0;
                }
            }

            ttgo->power->clearIRQ();
            isPowerIRQPressed = false;
        }
        if(startClickTime > 0) {
            if(millis() - startClickTime > doubleClickTime) {
                if(powerClicks == 1) { //Currently light sleep mode for fast startup
                    detachInterrupt(AXP202_INT);
                    enterLightSleep(ttgo);
                    attachInterrupt(AXP202_INT,handleInterrupt,FALLING);
                    isPowerIRQPressed = false;
                    ttgo->power->clearIRQ();
                }
                powerClicks = 0;
                startClickTime = 0;
            }
        }
}

void ClockProgram::exit() {
	detachInterrupt(AXP202_INT);
	ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,false);
	ttgo->power->clearIRQ();
}