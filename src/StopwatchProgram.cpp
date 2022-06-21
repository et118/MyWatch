#include "StopwatchProgram.h"

int StopwatchProgram::run(Program** programs, int numPrograms) {
    this->ttgo = TTGOClass::getWatch();
    this->tft = ttgo->tft;
    this->init();
    int status = this->loop();
    this->exit();
    return status;
}

void StopwatchProgram::init() {
    tft->fillScreen(TFT_BLACK);
	this->running = true;
	this->powerClicks = 0;
	this->startClickTime = 0;
	pinMode(AXP202_INT,INPUT_PULLUP);
	attachInterrupt(AXP202_INT,handleInterrupt,FALLING);
	ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,true);
	ttgo->power->clearIRQ();

    this->lastTouched = true;
    this->h = 0;
    this->m = 0;
    this->s = 0;
    this->ms = 0;
    this->state = 0;
    this->startMillis = 0;
    this->mill = 0;
    this->lastTouched = 0;
}

int StopwatchProgram::loop() {
    while(running) {
        bool touched = ttgo->touched();
        if(touched && !lastTouched) {
            state++;
            lastTouched = true;
        }
        if(!touched) {
            lastTouched = false;
        }

        switch(state) {
            case 0:
                h = 0;
                m = 0;
                s = 0;
                ms = 0;
                break;
            case 1:
                startMillis = millis();
                state++;
                break;
            case 2:
                mill = millis() - startMillis;
                ms = fmod(mill, 1000);
                s = fmod(mill/1000,60);
                m = fmod(mill/1000/60,60);
                h = fmod(mill/1000/60/60,24);
                break;
            case 3:
                break;
            case 4:
                state = 0;
                break;
        }

        tft->setTextColor(TFT_WHITE,TFT_BLACK);
        tft->setTextSize(2);
        tft->setTextDatum(CC_DATUM);
        String time;
        if(h>=10) {
            time += (String)h;
        }else{
            time += "0"+(String)h;
        }

        if(m>=10) {
            time += ":"+(String)m;
        }else{
            time += ":0"+(String)m;
        }

        if(s>=10) {
            time += ":"+(String)s;
        }else{
            time += ":0"+(String)s;
        }

        if(ms>=100) {
            time += ":"+(String)ms;
        } else if(ms >= 10) {
            time += ":0"+(String)ms;
        } else {
            time += ":00"+(String)ms;
        }
        tft->drawString(time,120,120);
        checkPowerIRQ();
    }
    return 1;
}

void StopwatchProgram::checkPowerIRQ() {

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

void StopwatchProgram::exit() {
    detachInterrupt(AXP202_INT);
	ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,false);
	ttgo->power->clearIRQ();
}