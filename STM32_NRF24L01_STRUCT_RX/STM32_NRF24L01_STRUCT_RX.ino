// RX RX RX RX RX RX RX RX RX RX RX RX
#include "Arduino.h"
#include <SPI.h>
#include <RF24-STM.h>

struct package
{
	uint32_t	id;
	float		temperature;
	char		titleStr[8];
	char		dataStr[16];
};


typedef struct package Package;

// Create an instance of the data container
Package myDataRx;

/*
 * NRF24L01     Arduino_ Uno  Blue_Pill(stm32f01C)
 * ___________________________________________________
 * VCC  RED    |    3.3v   |      3.3v
 * GND  BROWN  |    GND    |      GND
 * CSN  YELOW  |   Pin10   | A4 NSS1   (PA4)	\
 * CE   ORANGE |   Pin9    | B0 digital(PB0)    |	    NB
 * SCK  GREEN  |   Pin13   | A5 SCK1   (PA5)	|- All these pins
 * MOSI BLUE   |   Pin11   | A7 MOSI1  (PA7)	|  are 3.3v tolerant
 * MISO PURPLE |   Pin12   | A6 MISO1  (PA6) 	/
 *
 *    Always use the adapter plate for 5v!
 */
RF24 radio(PB0, PA4); // CE, CSN

#define LED_BUILTIN1 PC13

// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup() {
	pinMode(LED_BUILTIN1, OUTPUT);
	digitalWrite(LED_BUILTIN1,HIGH);

	// RX RX RX RX RX RX RX RX RX RX RX RX
	Serial.begin(115200);
	Serial.println("THIS IS THE RECEIVER CODE - YOU NEED THE OTHER ARDUINO TO TRANSMIT");

	// Initiate the radio object
	radio.begin();

	// Set the transmit power to lowest available to prevent power supply related issues
	radio.setPALevel(RF24_PA_LOW);

	// Set the speed of the transmission to the quickest available
	radio.setDataRate(RF24_2MBPS);

	// Use a channel unlikely to be used by Wifi, Microwave ovens etc 124
	radio.setChannel(115);

	radio.setRetries(255, 5);

	// Open a writing and reading pipe on each radio, with opposite addresses
	radio.openReadingPipe(1, 0xB3B4B5B609);
	radio.openWritingPipe(0xB3B4B5B601);

	// Start the radio listening for data
	radio.startListening();

	// Auto ackknowledgment of a transmission
	radio.setAutoAck(true);

	// Debugging information
	radio.printDetails();
	digitalWrite(LED_BUILTIN1,LOW);
}

// -----------------------------------------------------------------------------
// We are LISTENING on this device only (although we do transmit a response)
// -----------------------------------------------------------------------------
void loop() {
	// RX RX RX RX RX RX RX RX RX RX RX RX

	// Is there any data for us to get?
	if (radio.available()) {
		// Go and read the data and put it into that variable
		//while (radio.available()) {
		radio.read(&myDataRx, sizeof(Package));

		delay(100);
		Serial.println("RX: Received");

		// First, stop listening so we can talk
		radio.stopListening();

		// Show user what we sent and what we got back
		Serial.print("Id:");Serial.print(myDataRx.id, DEC);
		Serial.print(", Temperature:");Serial.print(myDataRx.temperature);
		Serial.print(", TitleStr:");Serial.print(myDataRx.titleStr);
		Serial.print(", DataStr:");Serial.println(myDataRx.dataStr);

		// Change something
		myDataRx.temperature += 0.01;
		snprintf(myDataRx.titleStr, sizeof(myDataRx.titleStr), "Stm32F103");
		snprintf(myDataRx.dataStr, sizeof(myDataRx.dataStr), "#%03d#stm32F103", myDataRx.id);

		// Tell the user what we sent back (the random numer + 1)
		Serial.println("RX: Replied");

		if (!radio.write(&myDataRx, sizeof(Package))) {
			Serial.println("RX: No ACK");
		} else {
			Serial.println("RX: ACK");
		}

		// Now, resume listening so we catch the next packets.
		radio.startListening();
		BlinkLed(50,100);
	}
}

void BlinkLed(int t1, int t2)
{
	digitalWrite(LED_BUILTIN1, LOW);   // turn the LED off by making the voltage LOW
	delay(t1);
	digitalWrite(LED_BUILTIN1, HIGH);  // turn the LED on (HIGH is the voltage level)
	delay(t2);                      // wait for a second
}
