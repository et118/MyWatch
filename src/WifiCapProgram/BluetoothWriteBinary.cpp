#include "WifiCapProgram/BluetoothWriteBinary.h"

void escape32(uint32_t n, uint8_t* buf) {
	buf[0] = n;
	buf[1] = n >>  8;
	buf[2] = n >> 16;
	buf[3] = n >> 24;
}

void escape16(uint16_t n, uint8_t* buf) {
	buf[0] = n;
	buf[1] = n >>  8;
}

void btWrite32(BluetoothSerial* bt,uint32_t n) {
	uint8_t _buf[4];
	escape32(n, _buf);
	bt->write(_buf, 4);
}

void btWrite16(BluetoothSerial* bt, uint16_t n) {
	uint8_t _buf[2];
	escape16(n, _buf);
	bt->write(_buf, 2);
}