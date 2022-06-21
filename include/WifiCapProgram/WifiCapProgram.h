#pragma once
#include "config.h"
#include "Program.h"
#include "GraphicsLib.h"
#include "SleepLib.h"
#include <vector>
#include "WifiCapProgram/Packets.h"
#include "BluetoothSerial.h"
#include "WifiCapProgram/BluetoothWriteBinary.h"


class WifiCapProgram : public Program {
	private:
		enum Modes {
			InitStart,
			Start,
			InitScanNetwork,
			ScanNetwork,
			InitSelectNetwork,
			SelectNetwork,
			InitCaptureHandshake,
			CaptureHandshake,
			InitTransferViaBluetooth,
			TransferViaBluetooth
		};

		TTGOClass* ttgo;
		TFT_eSPI* tft;
		bool running;
		int powerClicks;
		const unsigned long doubleClickTime = 300;
		unsigned long startClickTime;

		BluetoothSerial bluetoothSerial;
		TextButton* startButton;
		Button* networkSelectionButtons[3];
		int mode;
		bool lastTouched;
		bool wifiInitialized;
		int scrollLevel;


		wifi_ap_record_t* scanResult;
		uint16_t scanLen;
		uint8_t selectedSSID[33];
		uint8_t selectedBSSID[6];
		uint8_t selectedChannel;
		PacketCaptureBundle* packetBundle;


		void initWifi();
		void deinitWifi();
		
		void init();
		int loop();
		void exit();
		void checkPowerIRQ();

		void transferDataOverBluetooth();

	public:
		static void sniffer(void* buf, wifi_promiscuous_pkt_type_t pktType);
		static WifiCapProgram* self;
		PacketType getPacketType(wifi_promiscuous_pkt_t* pkt);
		using Program::Program;
		int run(Program** programs, int numPrograms);
};