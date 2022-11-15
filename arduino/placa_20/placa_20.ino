// PLACA 20
// SENSOR PLUVIOMETRO

#include <SDI12.h>
#include <MKRWAN.h>

// Hardware Pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const byte RAIN_PIN = 4;

// Lorawan Config
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
LoRaModem modem;
String appEui = "4585684625446682";
String appKey = "7FF4E146980ACBBCD03FF22CE54BB20F";

volatile float lluviaAcumulada = 0; //

// volatiles are subject to modification by IRQs
volatile unsigned long raintime, rainlast, raininterval;

// Interrupt routines (these are called by the hardware interrupts, not by the main code)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void rainIRQ()
// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
{
    raintime = millis();                // grab current time
    raininterval = raintime - rainlast; // calculate interval between this and last event
    if (raininterval > 10)              // ignore switch-bounce glitches less than 10mS after initial edge
    {
        lluviaAcumulada += 0.011;       // Each dump is 0.011" of water
    rainlast = raintime; // set up for next event
Serial.print("Lluvia acumulada = ");
    Serial.println(lluviaAcumulada);

    }
}

void setup()
{
    Serial.begin(9600);

    // -=-=-=-=-=-=-Lora config-=-=-=-=-=-=-

    while (!modem.begin(EU868))
    {
        Serial.println("Failed to start module");
        modem.restart();
        delay(5000);
    };

    Serial.print("Module version is: ");
    Serial.println(modem.version());
    Serial.print("Device EUI is: ");
    Serial.println(modem.deviceEUI());

    // set lastloradate value to actual millis. The datalogger will send the data after x minutes
    // is important that it dosent send the data at first, because the average data have to be obtain
    // Join to thethingsnetworks
    int connected = modem.joinOTAA(appEui, appKey);
    while (!connected)
    {
        Serial.println("Can not connect with a gateway!!");
        connected = modem.joinOTAA(appEui, appKey);
        delay(5000);
    }
    // Set poll interval to 60 secs.
    modem.minPollInterval(60);

    // attach external interrupt pins to IRQ functions
    attachInterrupt(RAIN_PIN, rainIRQ, FALLING);

    delay(500); // allow things to settle
}

void loop()
{
    uint16_t lluviaDesdeUltimoEnvio = lluviaAcumulada*100;
    Serial.println("-=-=-=-=-=-=-=-=PLUVIOMETRO-=-=-=-=-=-=-=-=-=-=-=-=");
    Serial.print("Lluvia desde el ultimo envio = ");
    Serial.println(lluviaDesdeUltimoEnvio);


    Serial.println("Enviando mensaje...");

    char msg[2];
    msg[0] = ((lluviaDesdeUltimoEnvio) >> 8) & 0xFF;
    msg[1] = (lluviaDesdeUltimoEnvio)&0xFF;
    

    int err;

    modem.beginPacket();
    // modem.print(total);
    modem.write(msg, 2);
    err = modem.endPacket(false);
    Serial.println("Mensaje enviado, 20 segundos de delay.");
    Serial.println("");
    lluviaAcumulada = 0;

    delay(20000);
}
