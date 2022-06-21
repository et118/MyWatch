#include "WifiCapProgram/Packets.h"

bool PacketCaptureBundle::doesPacketTypeFit(PacketType type) {
	if(currentTargets[type] < packetTargets[type]) {
		return true;
	}
	return false;
}

bool PacketCaptureBundle::isPacketRepeat(wifi_promiscuous_pkt_t* pkt) {
	if(pkt->payload[1]>>6==0x01) {
		return true;
	} else {
		return false;
	}
}

bool PacketCaptureBundle::doesBSSIDMatch(wifi_promiscuous_pkt_t* pkt, uint8_t bssid[6]) { //TODO add comment on which one is for which packet
	return (
		(pkt->payload[4] == bssid[0] &&
		pkt->payload[5] == bssid[1] &&
		pkt->payload[6] == bssid[2] &&
		pkt->payload[7] == bssid[3] &&
		pkt->payload[8] == bssid[4] &&
		pkt->payload[9] == bssid[5])
		||
		(pkt->payload[10] == bssid[0] &&
		pkt->payload[11] == bssid[1] &&
		pkt->payload[12] == bssid[2] &&
		pkt->payload[13] == bssid[3] &&
		pkt->payload[14] == bssid[4] &&
		pkt->payload[15] == bssid[5])
		||
		(pkt->payload[16] == bssid[0] &&
		pkt->payload[17] == bssid[1] &&
		pkt->payload[18] == bssid[2] &&
		pkt->payload[19] == bssid[3] &&
		pkt->payload[20] == bssid[4] &&
		pkt->payload[21] == bssid[5])
	);
}

bool PacketCaptureBundle::doesSSIDMatch(wifi_promiscuous_pkt_t* pkt, uint8_t ssid[33]) {
	String str = (char*)ssid;
	int len = pkt->payload[25];
	
	if(len == str.length()) { //Probe Request Check
		for(int i = 0; i < len; i++) {
			if(!((char)(pkt->payload[26+i]) == str.charAt(i))) {
				return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

bool PacketCaptureBundle::isPacketValid(wifi_promiscuous_pkt_t* pkt, PacketType type) {
	if(doesPacketTypeFit(type)) {
		if(!isPacketRepeat(pkt) && (doesBSSIDMatch(pkt,targetBSSID) || doesSSIDMatch(pkt,targetSSID))) {
			return true;
		} 
	}
	return false;
}

void PacketCaptureBundle::addPacket(wifi_promiscuous_pkt_t* pkt,wifi_promiscuous_pkt_type_t promiscuousType, PacketType type) {
	if(startTime == 0) {
		startTime = micros();
	}
	wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;
	uint32_t len = ctrl.sig_len;
	if(promiscuousType == WIFI_PKT_MGMT) {
		len -= 4;
	}
	uint32_t timestamp = micros()/1000000;
	uint32_t microseconds = micros() - startTime;
	uint32_t origLen = len;
	uint32_t inclLen = len;
	if(inclLen > snaplen) inclLen = snaplen;
	Packet* p = new Packet(timestamp,microseconds,origLen,inclLen,pkt->payload);
	packets.push_back(p);
	currentTargets[type] += 1;
}

uint8_t* PacketCaptureBundle::getCaptureStatus() {
	for(int i = 0; i < 8; i++) {
		uint8_t d = packetTargets[i]-currentTargets[i];
		memcpy(&returnTargetStatus[i],&d,1);
	}
	return returnTargetStatus;
}

std::vector<Packet*> PacketCaptureBundle::getPackets() {
	return packets;
}
