// #####################################################################################################################
// ######################################### ESP8266 TELEGRAM MESSENGER ################################################
// ##################################################################################################################### 
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266TelegramBOT.h>

extern "C"
{ 
    #include "user_interface.h" 
}

#define inputPin 0

#define botName "Doorbell"
#define botUserName "something that ends with bot"
#define botToken "your:token"
#define adminID "yourID" // Check my website to figure out how to get it

static char SSID[] = "your network name";
static char password[] = "your password";
static char hostname[] = "Doorbell"; // That's the name you will see in your network.

TelegramBOT bot(botToken, botName, botUserName);
// #####################################################################################################################
// ######################################### SETUP #####################################################################
// ##################################################################################################################### 
void setup() 
{
	// Set the hostname (otherwise the controller will be named esp8266) and init the esp for WiFi use
	wifi_station_set_hostname(hostname);
	WiFi.begin(SSID, password);
	// Wait until it is connected to the network
	while (WiFi.status() != WL_CONNECTED) 
	    delay(500);
	// Start the bot and send a message to you, so you know the ESP has booted.
	bot.begin();
	bot.sendMessage(adminID, F("ESP8266 online."), "");
}
// #####################################################################################################################
// ######################################### LOOP ######################################################################
// ##################################################################################################################### 
void loop() 
{
	// Doorbell button has been pushed
	if (!digitalRead(inputPin))
	{
		bot.sendMessage(adminID, F("Ding Dong"), "");
		delay(10000); 	// Halt for 10 sec, if someone pushes the button more often you'll get
						// many messages.
						// You could also use a timer, would, in fact, look more pretty.
	}
	// React on an incomming message:
	bot.getUpdates(bot.message[0][1]);
	for (int i = 1; i < bot.message[0][0].toInt() + 1; i++)
	{
		bot.message[i][5] = bot.message[i][5].substring(1, bot.message[i][5].length());
		if (bot.message[i][5] == "info")
		{
			bot.sendMessage(bot.message[i][4], String(F("Hello.")), "");
			bot.sendMessage(bot.message[i][4], String(F("I am your doorbell.")), "");
		}
		else if ((bot.message[i][5] == "get"))
		{
			unsigned long runtime = millis() / 1000;
			bot.sendMessage(bot.message[i][4], F("Runtime: ") + String(runtime) + F("sec."), "");
		}
		else 
		{
			bot.sendMessage(bot.message[i][4], String(F("Command unknown.")), "");
		}
	}
	bot.message[0][0] = "";
	delay(1000);
}
// #####################################################################################################################
// ######################################### END OF CODE ###############################################################
// #####################################################################################################################
