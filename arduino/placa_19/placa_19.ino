// PLACA 19
// SENSOR TEROS 12; ID:0001

#include <SDI12.h>
#include <MKRWAN.h>

// Hardware Pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
const byte SDI12_JACK1 = 5; // SDI-12 SENSORS

// Lorawan Config
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
LoRaModem modem;
String appEui = "4585684625446682"; 
String appKey = "1373C88818C1531BE2659C37B3B3D6EC"; 

// SDI12 CONFIG
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
SDI12 SDI12_1(SDI12_JACK1);

// Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
int TEROS12_ADDRESS = 0;
char inByte = 0;
String sdiResponse = "";
String myCommand = "";
String SDI_TEROS_COMMAND = "M!";
int16_t teros12results[3];

// Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void sdi12_1_func(int sensorAddress, String command, int16_t results[]);
uint16_t teros12_humidity(int RAW);

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

    // SDI 12 config
    Serial.println("Opening SDI-12 bus...");

    // SDI12_2.begin();
    SDI12_1.begin();

    delay(500); // allow things to settle
}

void loop()
{

    // -=-=-=-=-=-=-=TEROS 12-=-=-=-=-=-=-= (4)
    sdi12_1_func(TEROS12_ADDRESS, SDI_TEROS_COMMAND, teros12results);

    // Get the humidity (RAW -> HUMIDITY)
    teros12results[0] = teros12_humidity(teros12results[0]);

    int16_t teros_humidity = teros12results[0];
    int16_t teros_temperature = teros12results[1];
    int16_t teros_ec = teros12results[2];

    Serial.println("-=-=-=-=-=-=-=-=TEROS-12-=-=-=-=-=-=-=-=-=-=-=-=");
    Serial.print("Humdity = ");
    Serial.println(teros_humidity);
    Serial.print("Temperature = ");
    Serial.println(teros_temperature);
    Serial.print("CE = ");
    Serial.println(teros_ec);

    Serial.println("Enviando mensaje...");

    char msg[6];
    msg[0] = ((teros_humidity) >> 8) & 0xFF;
    msg[1] = (teros_humidity)&0xFF;
    msg[2] = ((teros_temperature) >> 8) & 0xFF;
    msg[3] = (teros_temperature)&0xFF;
    msg[4] = ((teros_ec) >> 8) & 0xFF;
    msg[5] = (teros_ec)&0xFF;

    int err;
    

    modem.beginPacket();
    // modem.print(total);
    modem.write(msg, 6);
    err = modem.endPacket(false);
    Serial.println("Mensaje enviado, 20 segundos de delay.");
    Serial.println("");

    delay(20000);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// SDI12 FUNCTIONS
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Get the value from response
String getValue(String data, char separator1, char separator2, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator1 || data.charAt(i) == separator2 || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


// This function return the sensor values
// If there is an error, the function return 231
void sdi12_1_func(int sensorAddress, String command, int16_t results[])
{
    String sdiResponse = "";
    String myCommand = "";

    // first command to take a measurement
    myCommand = String(sensorAddress) + command;

    SDI12_1.sendCommand(myCommand);
    delay(30); // wait a while for a response

    while (SDI12_1.available())
    {
        char c = SDI12_1.read();
        if ((c != '\n') && (c != '\r'))
        {
            sdiResponse += c;
            delay(10); // 1 character ~ 7.5ms
        }
    }

    SDI12_1.clearBuffer();

    delay(1000);
    sdiResponse = "";

    // next command to request data from last measurement
    myCommand = String(sensorAddress) + "D0!";

    SDI12_1.sendCommand(myCommand);
    delay(30); // wait a while for a response

    while (SDI12_1.available())
    {
        char c = SDI12_1.read();
        if ((c != '\n') && (c != '\r'))
        {
            sdiResponse += c;
            delay(10); // 1 character ~ 7.5ms
        }
    }

    String response1STR = getValue(sdiResponse, '+', '-', 1);
    int16_t response1 = response1STR.toFloat();
    String response2STR = getValue(sdiResponse, '+', '-', 2);
    int16_t response2 = response2STR.toFloat() * 10;
    String response3STR = getValue(sdiResponse, '+', '-', 3);
    int16_t response3 = response3STR.toFloat();
    Serial.print("response = ");
    Serial.println(sdiResponse);

    //  Check for Errors

    results[0] = response1;
    results[1] = response2;
    results[2] = response3;

    SDI12_1.clearBuffer();
}

// The result is the humidity * 100
// 22.05% --> 2205
uint16_t teros12_humidity(int RAW)
{
    return ((((3.879 * pow(10, -4)) * RAW) - (0.6956)) * 10000);
}
