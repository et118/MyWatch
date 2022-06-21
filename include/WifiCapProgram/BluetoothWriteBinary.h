#pragma once
#include "config.h"
#include "BluetoothSerial.h"
const uint32_t magic_number = 0xa1b2c3d4;
const uint16_t version_major = 2;
const uint16_t version_minor = 4;
const uint32_t thiszone = 0;
const uint32_t sigfigs = 0;
const uint32_t snaplen = 2500;
const uint32_t network = 105;
/* converts a 32 bit integer into 4 bytes */
void escape32(uint32_t n, uint8_t* buf);
/* converts a 16 bit integer into 2 bytes */
void escape16(uint16_t n, uint8_t* buf);
/* writes a 32 bit integer to Serial */
void btWrite32(BluetoothSerial* bt, uint32_t n);
/* writes a 16 bit integer to Serial */
void btWrite16(BluetoothSerial* bt, uint16_t n);