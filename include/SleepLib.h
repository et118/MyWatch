#pragma once
#include "config.h"

extern bool isPowerIRQPressed;
void IRAM_ATTR handleInterrupt();
void enterLightSleep(TTGOClass* ttgo);
