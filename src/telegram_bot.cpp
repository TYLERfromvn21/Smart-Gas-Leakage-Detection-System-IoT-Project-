#include "telegram_bot.h"
#include "config.h"
#include "globals.h"
#include "sensor.h"

// ==========================================
// TELEGRAM COMMAND PARSER - OPTIMIZED
// ==========================================
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    // Security check: Only accept commands from authorized Chat ID
    if (chat_id != CHAT_ID) {
        continue; 
    }

    // Use trimmed text to avoid issues with hidden spaces
    text.trim();

    if (text == "/up") {
      gasThreshold += 5;
      bot.sendMessage(chat_id, "Threshold increased to: " + String(gasThreshold), "");
    } 
    else if (text == "/down") {
      gasThreshold -= 5;
      bot.sendMessage(chat_id, "Threshold decreased to: " + String(gasThreshold), "");
    } 
    else if (text.startsWith("/set ")) {
      String valueStr = text.substring(5);
      int val = valueStr.toInt();
      if (val > 0 && val <= 100) {
        gasThreshold = val;
        bot.sendMessage(chat_id, "Threshold set to: " + String(gasThreshold), "");
      } else {
        bot.sendMessage(chat_id, "Invalid value! Please use 1-100.", "");
      }
    } 
    else if (text == "/calibrate") {
      bot.sendMessage(chat_id, "Recalibrating sensor... Please wait 10s.", "");
      calibrateSensor();
    } 
    else if (text == "/status") {
      String msg = "System Status:\n";
      msg += "Gas Level: " + String(currentGasPercent) + "%\n";
      msg += "Threshold: " + String(gasThreshold) + "\n";
      msg += "Baseline (Clean Air): " + String(cleanAirValue);
      bot.sendMessage(chat_id, msg, "");
    } 
    else if (text == "/start" || text == "/help") {
      String welcome = "Hello " + from_name + ".\n";
      welcome += "Current Commands:\n";
      welcome += "/up : Increase threshold (+5)\n";
      welcome += "/down : Decrease threshold (-5)\n";
      welcome += "/set 80 : Set threshold value\n";
      welcome += "/calibrate : Recalibrate sensor\n";
      welcome += "/status : Check status";
      bot.sendMessage(chat_id, welcome, "");
    }
  }
}