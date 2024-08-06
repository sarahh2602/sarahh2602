#include <Arduino.h>
#include<avr/sleep.h>
#include<avr/power.h>
int dis=0;
int togglestate=0;
long int lasttoggle=0;
int debounce=200;
int y;

#define DECODE_NEC          // Includes Apple and Onkyo. To enable all protocols , just comment/disable this line.
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp> // include the library

void setup() {
    Serial.begin(9600);
    pinMode(5,OUTPUT);
    pinMode(13,OUTPUT);
    while (!Serial){} 
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
    IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);

    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
}

void loop() {
    digitalWrite(13,HIGH);
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
            Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
            // We have an unknown protocol here, print extended info
            IrReceiver.printIRResultRawFormatted(&Serial, true);
            IrReceiver.resume(); // Do it here, to preserve raw data for printing with printIRResultRawFormatted()
        } else {
            IrReceiver.resume(); // Early enable receiving of the next IR frame
            IrReceiver.printIRResultShort(&Serial);
            IrReceiver.printIRSendUsage(&Serial);
        }
        Serial.println();
        if ((IrReceiver.decodedIRData.command == 0x44)&&(millis()-lasttoggle >debounce)) {
            togglestate=!togglestate;
            lasttoggle=millis();
        } else if (IrReceiver.decodedIRData.command == 0x40) {
              dis = min (255, dis+25);
        }else if (IrReceiver.decodedIRData.command == 0x19)
        {
            dis = max (0, dis-25);
        }
        else if (IrReceiver.decodedIRData.command == 0x45){
          IrReceiver.decodedIRData.command = 0x00;
          startSleep();
          
        }
        
    }
    if(togglestate){
      analogWrite(5,dis);
    }else{digitalWrite(5,LOW);}
}
void startSleep()
{
  IrReceiver.stop();
  attachInterrupt(digitalPinToInterrupt(3),pin2Interrupt,LOW);
Serial.println("going to sleep");
delay(100);
digitalWrite(13,LOW);

set_sleep_mode(SLEEP_MODE_PWR_DOWN);
sleep_enable();
sleep_mode();
sleep_disable();
IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);

delay(200);

}
void pin2Interrupt()
{
  detachInterrupt(digitalPinToInterrupt(3));
}






