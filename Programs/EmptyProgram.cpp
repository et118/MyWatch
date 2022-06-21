#include "EmptyProgram.h"

int EmptyProgram::run(Program** programs, int numPrograms) {
    this->ttgo = TTGOClass::getWatch();
    this->tft = ttgo->tft;
    this->init();
    int status = this->loop();
    this->exit();
    return status;
}

void EmptyProgram::init() {
    tft->fillScreen(TFT_BLACK);
	this->running = true;
	this->powerClicks = 0;
	this->startClickTime = 0;
	pinMode(AXP202_INT,INPUT_PULLUP);
	attachInterrupt(AXP202_INT,handleInterrupt,FALLING);
	ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,true);
	ttgo->power->clearIRQ();
}

int EmptyProgram::loop() {
    while(running) {
        checkPowerIRQ();
    }
    return 1;
}

void EmptyProgram::checkPowerIRQ() {

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
                if(powerClicks == 1) { 
                    detachInterrupt(AXP202_INT);
                    enterLightSleep(ttgo);  //Currently light sleep mode for fast startup
                    attachInterrupt(AXP202_INT,handleInterrupt,FALLING);
                    isPowerIRQPressed = false;
                    ttgo->power->clearIRQ();
                }
                powerClicks = 0;
                startClickTime = 0;
            }
        }
}

void EmptyProgram::exit() {
    detachInterrupt(AXP202_INT);
	ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,false);
	ttgo->power->clearIRQ();
}