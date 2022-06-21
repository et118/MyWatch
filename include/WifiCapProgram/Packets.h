#pragma once
#include "config.h"
#include <vector>
#include "esp_wifi.h"

enum PacketType {
	EAPOL,
	BEACON,
	PROBE_REQUEST,
	PROBE_RESPONSE,
	ASSOCIATION_REQUEST,
	ASSOCIATION_RESPONSE,
	AUTHENTICATION,
	DATA,
	NONE
};

class Packet {
	public:
		Packet(uint32_t timestamp,uint32_t microseconds,int inclLen,int origLen,uint8_t* buf) {
			this->timestamp = timestamp;
			this->microseconds = microseconds;
			this->inclLen = inclLen;
			this->origLen = origLen;
			this->buf = (uint8_t*)malloc(origLen);
			memcpy(this->buf,buf,inclLen);
		};
		~Packet() {
			delete this->buf;
		}
		uint32_t timestamp;
		uint32_t microseconds;
		int inclLen;
		int origLen;
		uint8_t *buf;
};

class PacketCaptureBundle { // ADD CHECK FOR IF ALL PARTS OF THE SAME HANDSHAKE IS CAPTURED 
	private:
		const uint32_t snaplen = 2500;
		bool doesPacketTypeFit(PacketType type);
		bool isPacketRepeat(wifi_promiscuous_pkt_t* pkt);
		bool doesBSSIDMatch(wifi_promiscuous_pkt_t* pkt, uint8_t bssid[6]);
		bool doesSSIDMatch(wifi_promiscuous_pkt_t* pkt, uint8_t ssid[33]);
		uint8_t packetTargets[8];
		uint8_t currentTargets[8] = {0,0,0,0,0,0,0,0};
		uint8_t returnTargetStatus[8];
		uint8_t targetBSSID[6];
		uint8_t targetSSID[33];
		uint32_t startTime;
		std::vector<Packet*> packets;
	public:
		PacketCaptureBundle(uint8_t packetTargets[8], uint8_t targetBSSID[6],uint8_t targetSSID[33]) {
			memcpy(this->packetTargets,packetTargets,8);
			memcpy(this->targetBSSID,targetBSSID,6);
			memcpy(this->targetSSID,targetSSID,33);
			packets = std::vector<Packet*>();
			startTime = 0;
		};
		~PacketCaptureBundle() {
			std::vector<Packet*>().swap(packets);
		};

		bool isPacketValid(wifi_promiscuous_pkt_t* pkt, PacketType type);
		void addPacket(wifi_promiscuous_pkt_t* pkt, wifi_promiscuous_pkt_type_t promiscuousType,PacketType type);
		uint8_t* getCaptureStatus();
		std::vector<Packet*> getPackets();


};