#include "SleepLib.h"
bool isPowerIRQPressed = false;

void IRAM_ATTR handleInterrupt() {
    isPowerIRQPressed = true;
}

void enterLightSleep(TTGOClass* ttgo) {
    ttgo->closeBL();
    ttgo->displaySleep();
    gpio_wakeup_enable((gpio_num_t)AXP202_INT,GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();
    esp_light_sleep_start(); //Code breaks here
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_GPIO);
    gpio_wakeup_disable((gpio_num_t)AXP202_INT);
    ttgo->openBL();
    ttgo->displayWakeup();
}
