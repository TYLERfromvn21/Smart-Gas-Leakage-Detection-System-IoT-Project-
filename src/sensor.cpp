#include "sensor.h"
#include "config.h"
#include "globals.h"

// ==========================================
// SENSOR CALIBRATION ALGORITHM
// ==========================================
void calibrateSensor() {
  isCalibrating = true; 
  Serial.println("Calibrating...");

  // Warm-up phase with blinking LED
  for(int i = 0; i < 20; i++) {
    digitalWrite(LED_RED_PIN, !digitalRead(LED_RED_PIN));
    vTaskDelay(pdMS_TO_TICKS(500));
  }
  digitalWrite(LED_RED_PIN, LOW);

  // Baseline sampling
  long sum = 0;
  for(int i = 0; i < 100; i++) {
    sum += analogRead(SENSOR_PIN);
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  cleanAirValue = sum / 100;

  String msg = "Calibration Complete!\nBaseline Value: " + String(cleanAirValue);
  
  // Send result via Telegram using Mutex protection
  if (xSemaphoreTake(botMutex, portMAX_DELAY)) {
    bot.sendMessage(CHAT_ID, msg, "");
    xSemaphoreGive(botMutex);
  }
  
  isCalibrating = false; 
}