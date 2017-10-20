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

void setup() 
{
	// Setup the pins
	pinMode(inputPin, INPUT_PULLUP);

	// Start serial com
	Serial.begin(115200);
	Serial.print("\n  Connecting to " + String(SSID) + " ");

	// Set the hostname (otherwise the controller will be named esp8266) and init the esp for WiFi use
	wifi_station_set_hostname(hostname);
	WiFi.begin(SSID, password);

	// Wait until it is connected to the network
	while (WiFi.status() != WL_CONNECTED) 
	{
	    delay(500);
	    Serial.print(".");
	}

	Serial.println("\n\nWiFi connected.");  
	Serial.print("  IP address: " + WiFi.localIP().toString() + "\n");
	Serial.print("  Host name:  " + String(hostname) + "\n");
	Serial.print("- - - - - - - - - - - - - - - - - - -\n\n");
	delay(3000);

	// Start the bot and send a message to you, so you know the ESP has booted.
	bot.begin();
	bot.sendMessage(adminID, "ESP8266 online.", "");
}

void loop() 
{
	// Doorbell button has been pushed
	if (!digitalRead(inputPin))
	{
		bot.sendMessage(adminID, "Ding Dong", "");
		delay(10000); 	// Halt for 10 sec, if someone pushes the button more often you'll get
						// many messages.
						// You could also use a timer, would, in fact, look more pretty.
	}
	// React on an incomming message:
	/* 	
		GENERAL
			bot.message[0][0] ... number of received messages
			bot.message[0][1] ... code of last message
		MESSAGE
			bot.message[i][0] ... update ID
			bot.message[i][1] ... user ID
			bot.message[i][2] ... first name
			bot.message[i][3] ... last name
			bot.message[i][4] ... user ID
			bot.message[i][5] ... message
	*/
	bot.getUpdates(bot.message[0][1]);
	for (int i = 1; i < bot.message[0][0].toInt() + 1; i++)
	{
		// Only the admin is allowed to talk to the bot
		if (bot.message[i][4] == adminID)
		{
			// extract the message
			bot.message[i][5] = bot.message[i][5].substring(1, bot.message[i][5].length());
			if (bot.message[i][5] == "info")
			{
				bot.sendMessage(bot.message[i][4], "Hello.", "");
				bot.sendMessage(bot.message[i][4], "I am your doorbell.", "");
			}
			else if ((bot.message[i][5] == "get"))
			{
				unsigned long runtime = millis() / 1000;
				bot.sendMessage(bot.message[i][4], "Runtime: " + String(runtime) + "sec.", "");
			}
			else 
			{
				bot.sendMessage(bot.message[i][4], "Command unknown.", "");
			}
		}
		// If any other person as the admin sends something to your bot, let them know they are not allowed to do so.
		else 
		{
			String senderID = bot.message[i][4];
			bot.sendMessage(bot.message[i][4], "Access denied. Your ID: " + senderID, "");
		}
	}
	bot.message[0][0] = "";
	delay(1000);
}
