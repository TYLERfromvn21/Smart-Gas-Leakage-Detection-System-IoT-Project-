#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "globals.h"
#include "sensor.h"
#include "telegram_bot.h"

// --- INSTANTIATE GLOBAL VARIABLES ---
volatile int gasThreshold = 50;
volatile int cleanAirValue = 0;
volatile int currentGasPercent = 0;
volatile bool isCalibrating = false;
unsigned long lastSendAlert = 0;

SemaphoreHandle_t botMutex;
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// --- TASK HANDLERS ---
TaskHandle_t SensorTask;
TaskHandle_t TelegramTask;
TaskHandle_t AlertTask;

// ==========================================
// TASK 1: SENSOR POLLING (Core 0)
// ==========================================
void TaskReadSensor(void *pvParameters) {
  for (;;) {
    if (!isCalibrating) {
      int rawValue = analogRead(SENSOR_PIN);
      int diff = rawValue - cleanAirValue;
      if (diff < 0) diff = 0;

      int mappedValue = map(diff, 0, 2000, 0, 100);
      if (mappedValue > 100) mappedValue = 100;
      
      currentGasPercent = mappedValue;
    }
    vTaskDelay(pdMS_TO_TICKS(100)); // Read every 100ms
  }
}

// ==========================================
// TASK 2: TELEGRAM POLLING (Core 0)
// ==========================================
void TaskTelegram(void *pvParameters) {
  for (;;) {
    if (!isCalibrating) {
      if (xSemaphoreTake(botMutex, portMAX_DELAY)) {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        while (numNewMessages) {
          handleNewMessages(numNewMessages);
          numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        xSemaphoreGive(botMutex);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000)); // Check messages every 1000ms
  }
}

// ==========================================
// TASK 3: ALERT MANAGER (Core 1)
// ==========================================
void TaskAlertManager(void *pvParameters) {
  for (;;) {
    if (!isCalibrating) {
      if (currentGasPercent > gasThreshold) {
        digitalWrite(LED_RED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, LOW); // Buzzer Active LOW

        // Send Telegram Alert every 5 seconds
        if (millis() - lastSendAlert > 5000) {
          String msg = "\xE2\x9A\xA0 WARNING! GAS DETECTED!\n";
          msg += "Level: " + String(currentGasPercent) + "%\n";
          msg += "Threshold: " + String(gasThreshold);
          
          if (xSemaphoreTake(botMutex, portMAX_DELAY)) {
            bot.sendMessage(CHAT_ID, msg, "");
            xSemaphoreGive(botMutex);
          }
          lastSendAlert = millis();
        }
      } else {
        digitalWrite(LED_RED_PIN, LOW);
        digitalWrite(BUZZER_PIN, HIGH); // Mute buzzer
      }
    }
    vTaskDelay(pdMS_TO_TICKS(50)); // Fast response check (50ms)
  }
}

// ==========================================
// SYSTEM SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(BUZZER_PIN, HIGH);

  botMutex = xSemaphoreCreateMutex();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  
  client.setInsecure(); // Bypass SSL verification for faster Telegram API access

  bot.sendMessage(CHAT_ID, "System Online! Warming up sensor...", "");
  calibrateSensor();


  xTaskCreatePinnedToCore(TaskReadSensor, "SensorTask", 2048, NULL, 1, &SensorTask, 1);
  xTaskCreatePinnedToCore(TaskTelegram, "TelegramTask", 10240, NULL, 1, &TelegramTask, 1);
  xTaskCreatePinnedToCore(TaskAlertManager, "AlertTask", 10240, NULL, 2, &AlertTask, 1);
}

void loop() {
  // RTOS handles everything in the background
}