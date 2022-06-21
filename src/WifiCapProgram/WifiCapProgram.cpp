#include "WifiCapProgram/WifiCapProgram.h"

WifiCapProgram* WifiCapProgram::self = NULL;

int WifiCapProgram::run(Program** programs, int numPrograms) {
    this->ttgo = TTGOClass::getWatch();
    this->tft = ttgo->tft;
    this->init();
    int status = this->loop();
    this->exit();
    return status;
}

void WifiCapProgram::init() {
    tft->fillScreen(TFT_BLACK);
	this->running = true;
	this->powerClicks = 0;
	this->startClickTime = 0;
	pinMode(AXP202_INT,INPUT_PULLUP);
	attachInterrupt(AXP202_INT,handleInterrupt,FALLING);
	ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,true);
	ttgo->power->clearIRQ();

    mode = InitStart;
    wifiInitialized = false;
    lastTouched = true;
    scanResult = 0;
    scanLen = 0;
    scrollLevel = 0;
    self = this;
    startButton = new TextButton(60,60,120,120,2,"START",TFT_BLACK,TFT_WHITE);
    networkSelectionButtons[0] = new Button(150,80,90,80,TFT_LIGHTGREY,TFT_DARKGREY);
    networkSelectionButtons[1] = new Button(150,0,90,80,TFT_LIGHTGREY,TFT_DARKGREY);
    networkSelectionButtons[2] = new Button(150,160,90,80,TFT_LIGHTGREY,TFT_DARKGREY);
}

void WifiCapProgram::deinitWifi() {
	if(!wifiInitialized) {
		return;
	}
	ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));
	ESP_ERROR_CHECK(esp_wifi_stop());
	ESP_ERROR_CHECK(esp_wifi_deinit());
	wifiInitialized = false;
}

void WifiCapProgram::initWifi() {
    if(wifiInitialized) {
		deinitWifi();
		delay(1000);
	}
	esp_netif_init();
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
	delay(1000);
	wifiInitialized = true;
}

int WifiCapProgram::loop() {
    while(running) {
        checkPowerIRQ();
        int16_t x = 0;
        int16_t y = 0;
        bool touched = ttgo->getTouch(x,y);
        switch(mode) {
            case InitStart: {
                tft->fillScreen(TFT_BLACK);
                startButton->draw(tft);
                mode++;
                break;
            }

            case Start: {
                if(touched && !lastTouched) {
                    for(int i = 0; i < 3; i++) {
                        if(startButton->inBounds(x,y)) {
                            mode++;
                            break;
                        }
                    }
                }
                break;
            }

            case InitScanNetwork: {
                tft->fillScreen(TFT_BLACK);
                if(scanResult != 0) {
                    delete[] scanResult;
                    scanResult = 0;
                }
                scanLen = 0;
                initWifi();
                mode++;
                break;
            }

            case ScanNetwork: {
                drawText(tft,120,120,3,"SCANNING",TFT_WHITE,TFT_BLACK);
                wifi_scan_config_t scanConfig;
                scanConfig.scan_type = WIFI_SCAN_TYPE_ACTIVE;
                scanConfig.show_hidden = false;
                scanConfig.scan_time.active.max = 1000;
                scanConfig.scan_time.active.min = 500;
                ESP_ERROR_CHECK(esp_wifi_scan_start(&scanConfig,true));
                ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&scanLen));
                scanResult = new wifi_ap_record_t[scanLen];
                ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&scanLen,scanResult));
                if(scanLen > 0) {
                    mode++;
                    scrollLevel = 0;
                } else {
                    tft->fillScreen(TFT_BLACK);
                    drawText(tft,120,120,2,"No networks found",TFT_WHITE,TFT_BLACK);
                    delay(1500);
                    mode = InitStart;
                }
                break;
            }

            case InitSelectNetwork: {
                tft->fillScreen(TFT_BLACK);
                for(int i = 0; i < 5;i++) {
                    tft->fillRect(0,48*i,150,48,TFT_LIGHTGREY);
                    tft->drawRect(0,48*i,150,48,TFT_DARKGREY);
                }
                for(int i = 0; i < 5; i++) {
                    if(i+scrollLevel<scanLen) {
                        drawText(tft,75,(48*i)+24,1,(char*)(scanResult[i+scrollLevel].ssid),TFT_BLACK,TFT_LIGHTGREY);
                    }
			    }
                for(Button* b : networkSelectionButtons) {
                    b->draw(tft);
                }
                mode++;
                break;
            }

            case SelectNetwork: {
                if(touched && !lastTouched) {
                    for(int i = 0; i < 3; i++) {
                        if(networkSelectionButtons[i]->inBounds(x,y)) {
                            switch(i) {
                                case 0:
                                    mode = InitScanNetwork;
                                    break;
                                case 1:
                                    scrollLevel--;
                                    mode = InitSelectNetwork;
                                    break;
                                case 2:
                                    scrollLevel++;
                                    mode = InitSelectNetwork;
                                    break;
                            };
                            if(scrollLevel < 0) {scrollLevel = 0;}
                            if(scrollLevel > scanLen - 5) {scrollLevel = scanLen - 5;}
                            break;
                        }
                    }
                    for(int i = 0; i < 5; i++) {
                        if(i+scrollLevel<scanLen) {
                            if(x > 0 && x < 150 && y > 240/5*i && y < 240/5*(i+1)) {
                                memcpy(&selectedSSID,scanResult[i+scrollLevel].ssid,33);
                                memcpy(&selectedBSSID,scanResult[i+scrollLevel].bssid,6);
                                selectedChannel = scanResult[i+scrollLevel].primary;
                                mode++;
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case InitCaptureHandshake: {
                tft->fillScreen(TFT_BLACK);
                uint8_t targets[8] = {4,1,0,0,0,0,0,0}; //We want 4 EAPOL and 1 BEACON packets
                ESP_LOGI("WifiCapProgram","Selected BSSID:%02x:%02x:%02x:%02x:%02x:%02x",selectedBSSID[0],selectedBSSID[1],selectedBSSID[2],selectedBSSID[3],selectedBSSID[4],selectedBSSID[5]);
                packetBundle = new PacketCaptureBundle(targets,selectedBSSID,selectedSSID);
                esp_wifi_set_promiscuous(true);
                esp_wifi_set_promiscuous_rx_cb(sniffer);
                wifi_second_chan_t secondChannel = (wifi_second_chan_t)NULL;
                esp_wifi_set_channel(selectedChannel,secondChannel);
                mode++;
                break;
            }

            case CaptureHandshake: {
                uint8_t* status = packetBundle->getCaptureStatus();
                bool done = true;
                for(int i = 0; i < 8; i++) {
                    drawText(tft,120,(30*i)-(30*i/5),2,(String)((int)status[i]),TFT_WHITE,TFT_BLACK);
                    if(status[i] != 0) {
                        done = false;
                    }
                }
                
                if(done) {
                    deinitWifi();
                    mode++;
                }
                break;
            }

            case InitTransferViaBluetooth: {
                tft->fillScreen(TFT_BLACK);
                bluetoothSerial.begin("MyWatch");
                drawText(tft,120,120,2,"Connect Bluetooth",TFT_WHITE,TFT_BLACK);
                mode++;
                break;
            }

            case TransferViaBluetooth: {
                transferDataOverBluetooth();
                break;
            }


        };
        if(touched){
            lastTouched = true;
        } else {
            lastTouched = false;
        }
    }
    return 1;
}

void WifiCapProgram::transferDataOverBluetooth() {
    if(bluetoothSerial.hasClient()) {
        delay(1000);
        tft->fillScreen(TFT_BLACK);
        drawText(tft,120,120,2,"Transfering Data",TFT_WHITE,TFT_BLACK);
        String fileName = (char*)selectedSSID;
        fileName += ".pcap";
        bluetoothSerial.println(fileName);
        
        std::vector<Packet*> packets = packetBundle->getPackets();

        btWrite32(&bluetoothSerial,magic_number);
        btWrite16(&bluetoothSerial,version_major);
        btWrite16(&bluetoothSerial,version_minor);
        btWrite32(&bluetoothSerial,thiszone);
        btWrite32(&bluetoothSerial,sigfigs);
        btWrite32(&bluetoothSerial,snaplen);
        btWrite32(&bluetoothSerial,network);
        for(int i = 0; i < packets.size(); i++) {
            btWrite32(&bluetoothSerial,packets.at(i)->timestamp);
            btWrite32(&bluetoothSerial,packets.at(i)->microseconds);
            btWrite32(&bluetoothSerial,packets.at(i)->inclLen);
            btWrite32(&bluetoothSerial,packets.at(i)->origLen);
            bluetoothSerial.write(packets.at(i)->buf,packets.at(i)->inclLen);
        }
        bluetoothSerial.flush();
        tft->fillScreen(TFT_BLACK);
        drawText(tft,120,120,2,"Transfer Complete",TFT_WHITE,TFT_BLACK);
        bluetoothSerial.end();
        delay(5000);

        delete packetBundle;
        delete[] scanResult;
        packetBundle = NULL;
        scanResult = 0;
        scanLen = 0;
        scrollLevel = 0;
        selectedChannel = 0;
        mode = InitStart;
    }
}

void WifiCapProgram::sniffer(void* buf, wifi_promiscuous_pkt_type_t pktType) {
	wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
	PacketType type = self->getPacketType(pkt);

	if(self->packetBundle->isPacketValid(pkt, type)) {
		self->packetBundle->addPacket(pkt,pktType,type);
	}
}

PacketType WifiCapProgram::getPacketType(wifi_promiscuous_pkt_t* pkt) {
	uint8_t* payload = pkt->payload;
	if(((payload[30]==0x88 && payload[31]==0x8e) ||
	   (payload[31]==0x88 && payload[32]==0x8e) ||
	   (payload[32]==0x88 && payload[33]==0x8e))) {
		   return EAPOL;
	}
	if(payload[0]==0x80) {return BEACON;}
	if(payload[0]==0x40) {return PROBE_REQUEST;}
	if(payload[0]==0x50) {return PROBE_RESPONSE;}
	if(payload[0]==0x00) {return ASSOCIATION_REQUEST;}
	if(payload[0]==0x10) {return ASSOCIATION_RESPONSE;}
	if(payload[0]==0xb0) {return AUTHENTICATION;}
	if(payload[0]==0x08) {return DATA;}

	return NONE;
}

void WifiCapProgram::checkPowerIRQ() {

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
                    //enterLightSleep(ttgo);  No sleep when da wifi is tuggin
                    attachInterrupt(AXP202_INT,handleInterrupt,FALLING);
                    isPowerIRQPressed = false;
                    ttgo->power->clearIRQ();
                }
                powerClicks = 0;
                startClickTime = 0;
            }
        }
}

void WifiCapProgram::exit() {
    detachInterrupt(AXP202_INT);
	ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,false);
	ttgo->power->clearIRQ();
}