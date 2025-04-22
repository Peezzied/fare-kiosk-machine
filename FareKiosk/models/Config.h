// Config.h
#pragma once

#define DEV_MODE

#define COIN_FULL_BIT     (1 << 0)  // Bit 0: 0x01
#define COIN_EMPTY_BIT    (1 << 1)  // Bit 1: 0x02
#define NEED_CHANGE_BIT   (1 << 2)  // Bit 2: 0x04
#define DATA_RECEIVED_BIT (1 << 3)  // Bit 3: 0x08

extern EventGroupHandle_t machineEventGroup;