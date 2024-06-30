// TX TX TX TX TX TX TX TX TX TX TX TX TX
#include "Arduino.h"

#include <SPI.h>

//#include <RF24.h>
#include "RF24-STM.h"

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
RF24 radio(PB0, PA4);

struct package
{
	uint32_t	id;
	float		temperature;
	char		titleStr[8];
	char		dataStr[16];
};


typedef struct package Package;

// Create an instance of the data container
Package myData;

#define LED_BUILTIN1 PC13

// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup() {
	// TX TX TX TX TX TX TX TX TX TX TX TX TX
	pinMode(LED_BUILTIN1, OUTPUT);
	digitalWrite(LED_BUILTIN1,HIGH);

	Serial.begin(115200);
	Serial.println("TX: THIS IS THE TRANSMITTER CODE - YOU NEED THE OTHER ARDIUNO TO SEND BACK A RESPONSE");

	// Create enum type with some default data
	myData.id = 0;
	myData.temperature = 50.37;
	myData.titleStr[0] = 0;
	myData.dataStr[0] = 0;


	// Initiate the radio object
	radio.begin();

	// Set the transmit power to lowest available to prevent power supply related issues
	radio.setPALevel(RF24_PA_LOW);

	// Set the speed of the transmission to the quickest available
	radio.setDataRate(RF24_2MBPS);

	// Use a channel unlikely to be used by Wifi, Microwave ovens etc 124
	radio.setChannel(115);

	// Give receiver a chance
	radio.setRetries(200, 50);

	// Open a writing and reading pipe on each radio, with opposite addresses
	radio.openReadingPipe(1, 0xB3B4B5B601);
	radio.openWritingPipe(0xB3B4B5B609);

	// Auto ackknowledgment of a transmission
	radio.setAutoAck(true);

	// Debugging information
	radio.printDetails();

	//RF24_PrintState ();

	// Random number seeding (we're going to be sending a single random number)
	randomSeed(analogRead(14));
  digitalWrite(LED_BUILTIN1,LOW);
}

// -----------------------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// -----------------------------------------------------------------------------
void loop() {

	// TX TX TX TX TX TX TX TX TX TX TX TX TX

	// Ensure we have stopped listening (even if we're not) or we won't be able to transmit
	radio.stopListening();

	// Did we manage to SUCCESSFULLY transmit that (by getting an acknowledgement back from the other Arduino)?
	// Even we didn't we'll continue with the sketch, you never know, the radio fairies may help us
	Serial.println("TX: Sent out data");
	myData.id++;
	myData.temperature += 0.01;
	snprintf(myData.titleStr, sizeof(myData.titleStr), "STM32F103");
	snprintf(myData.dataStr, sizeof(myData.dataStr), "#%03d#stm32F103", myData.id);
	if (!radio.write(&myData, sizeof(myData))) {
		Serial.println("TX: No ACK");
	} else {
		Serial.println("TX: ACK");
	}

	// Now listen for a response
	radio.startListening();

	// But we won't listen for long
	unsigned long started_waiting_at = millis();

	// Loop here until we get indication that some data is ready for us to read (or we time out)
	while (!radio.available()) {

		// Oh dear, no response received within our timescale
		if (millis() - started_waiting_at > 250) {
			Serial.print("TX: Got no reply");
			delay(2000);
			return;
		}
	}

	// Now read the data that is waiting for us in the nRF24L01's buffer
	radio.read(&myData, sizeof(myData));
	delay(250);

	// Show user what we sent and what we got back
	Serial.print("Id:");Serial.print(myData.id, DEC);
	Serial.print(", Temperature:");Serial.print(myData.temperature);
	Serial.print(", TitleStr:");Serial.print(myData.titleStr);
	Serial.print(", DataStr:");Serial.println(myData.dataStr);

	// Wash, Rinse, Repeat... a bit later
	BlinkLed(50,100);
}

void BlinkLed(int t1, int t2)
{
	digitalWrite(LED_BUILTIN1, LOW);   // turn the LED off by making the voltage LOW
	delay(t1);
	digitalWrite(LED_BUILTIN1, HIGH);  // turn the LED on (HIGH is the voltage level)
	delay(t2);                      // wait for a second
}


