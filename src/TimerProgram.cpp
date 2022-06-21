#include "TimerProgram.h"

int TimerProgram::run(Program** programs, int numPrograms) {
    this->ttgo = TTGOClass::getWatch();
    this->tft = ttgo->tft;
    this->init();
    int status = this->loop();
    this->exit();
    return status;
}

void TimerProgram::init() {
    tft->fillScreen(TFT_BLACK);
	this->running = true;
	this->powerClicks = 0;
	this->startClickTime = 0;
	pinMode(AXP202_INT,INPUT_PULLUP);
	attachInterrupt(AXP202_INT,handleInterrupt,FALLING);
	ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,true);
	ttgo->power->clearIRQ();

    h = 0;
    m = 0;
    s = 0;
    state = 0;
    lastTouched = true;
    endTime = 0;
    
    this->buttons[0] = new Button(0,85,240,70,TFT_GREEN,TFT_BLACK);
    this->buttons[1] = new Button(0,0,80,85,TFT_LIGHTGREY,TFT_DARKGREY);
    this->buttons[2] = new Button(80,0,80,85,TFT_LIGHTGREY,TFT_DARKGREY);
    this->buttons[3] = new Button(160,0,80,85,TFT_LIGHTGREY,TFT_DARKGREY);
    this->buttons[4] = new Button(0,155,80,85,TFT_LIGHTGREY,TFT_DARKGREY);
    this->buttons[5] = new Button(80,155,80,85,TFT_LIGHTGREY,TFT_DARKGREY);
    this->buttons[6] = new Button(160,155,80,85,TFT_LIGHTGREY,TFT_DARKGREY);
    drawButtons();
}

void TimerProgram::drawButtons() {
    for(Button* b : buttons) {
        b->draw(tft);
    }
}

void TimerProgram::handleButtonInputs() {
    bool touched = ttgo->touched();
	if(touched && !lastTouched) {
		int16_t x = 0;
		int16_t y = 0;
		ttgo->getTouch(x,y);
		if(state == 3) {
			state = 4;
			return;
		}
		for(int i = 0;i<7;i++) {
			if(buttons[i]->inBounds(x,y)) {
				if(i == 0) {
					if(state == 0) {
						state = 1;
					} else if(state == 2) {
						state = 0;
						buttons[0]->setColor(TFT_GREEN,TFT_GREEN);
						drawButtons();
					}
				} else if(state == 0) {
					switch(i) {
						case 0:
							break;
						case 1:
							h++;
							break;
						case 2:
							m++;
							break;
						case 3:
							s++;
							break;
						case 4:
							h--;
							break;
						case 5:
							m--;
							break;
						case 6:
							s--;
							break;
					};
				}
				break;
			}
		}
		lastTouched = true;
	}
	if(!touched) {
		lastTouched = false;
	}
}

int TimerProgram::loop() {
    while(running) {
        checkPowerIRQ();
        handleButtonInputs();
        switch(state) {
            case 0: {
                if(h < 0) h = 0;
                if(h >= 24) h = 0;
                if(m < 0) m = 0;
                if(m >= 60) m = 0;
                if(s < 0) s = 0;
                if(s >= 60) s = 0;
                String timeString = "";
                if(h>=10) {
                    timeString += (String)h;
                }else{
                    timeString += "0"+(String)h;
                }
                if(m>=10) {
                    timeString += "|"+(String)m;
                }else{
                    timeString += "|0"+(String)m;
                }
                if(s>=10) {
                    timeString += "|"+(String)s;
                }else{
                    timeString += "|0"+(String)s;
                }
                tft->setTextColor(TFT_BLACK,TFT_GREEN);
                tft->setTextSize(5);
                tft->setTextDatum(CC_DATUM);
                tft->drawString(timeString,120,120);
                
                break;
            }
            
            case 1: {
                endTime = millis() + h * 3600000 + m * 60000 + s * 1000;
                buttons[0]->setColor(TFT_RED,TFT_RED);
                drawButtons();
                state++;
            }

            case 2: {
                if(endTime < millis()) {
                    state++;
                    break;
                } else {
                    long mill = endTime - millis();
                    s = fmod(mill/1000,60);
                    m = fmod(mill/1000/60,60);
                    h = fmod(mill/1000/60/60,24);
                }

                String timeString = ""; // TODO make this to a function to stop reusing it
                if(h>=10) {
                    timeString += (String)h;
                }else{
                    timeString += "0"+(String)h;
                }
                if(m>=10) {
                    timeString += "|"+(String)m;
                }else{
                    timeString += "|0"+(String)m;
                }
                if(s>=10) {
                    timeString += "|"+(String)s;
                }else{
                    timeString += "|0"+(String)s;
                }
                tft->setTextColor(TFT_WHITE,TFT_RED);
                tft->setTextSize(5);
                tft->setTextDatum(CC_DATUM);
                tft->drawString(timeString,120,120);
                break;
            }

            case 3: {
                ttgo->rtc->disableAlarm();
                ttgo->motor->onec(350);
                tft->fillScreen(TFT_RED);
                delay(500);
                tft->fillScreen(TFT_BLACK);
                ttgo->motor->onec(350);
                delay(500);
                tft->fillScreen(TFT_RED);
                ttgo->motor->onec(350);
                delay(500);
                ttgo->motor_begin(); // Because the screen brightness got lowered when vibrating.
                                    // But why tho?
                state++;
                break;
            }

            case 4: {
                state = 0;
                h = 0;
                m = 0;
                s = 0;
                endTime = 0;
                buttons[0]->setColor(TFT_GREEN,TFT_GREEN);
                drawButtons();
                break;
            }
        }
    }
    return 1;
}

void TimerProgram::checkPowerIRQ() {

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
                    esp_sleep_enable_timer_wakeup((endTime-millis())*1000);
                    enterLightSleep(ttgo);  //Currently light sleep mode for fast startup
                    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
                    attachInterrupt(AXP202_INT,handleInterrupt,FALLING);
                    isPowerIRQPressed = false;
                    ttgo->power->clearIRQ();
                }
                powerClicks = 0;
                startClickTime = 0;
            }
        }
}

void TimerProgram::exit() {
    detachInterrupt(AXP202_INT);
	ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,false);
	ttgo->power->clearIRQ();
    for(Button* b : buttons) {
        delete b;
    }
}