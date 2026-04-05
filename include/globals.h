#pragma once
#include <Arduino.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

// --- SHARED GLOBAL VARIABLES ---
extern volatile int gasThreshold;
extern volatile int cleanAirValue;
extern volatile int currentGasPercent;
extern volatile bool isCalibrating;
extern unsigned long lastSendAlert;

// --- SHARED OBJECTS & MUTEX ---
extern SemaphoreHandle_t botMutex;
extern WiFiClientSecure client;
extern UniversalTelegramBot bot;