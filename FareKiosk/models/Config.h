// Config.h
#pragma once

#define DEV_MODE

// Coin
#define COIN_ISR_INCOMING (1 << 0)
#define COIN_FULL (1 << 1)
#define COIN_READY (1 << 1)



extern EventGroupHandle_t machineEventGroup;