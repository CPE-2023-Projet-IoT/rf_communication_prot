#include "MicroBit.h"
#include "bme280.h"
#include "tsl256x.h"
#include "ssd1306.h"
#include "protocole.h"
#include <string>
#include <map>

MicroBit uBit;

// I2C
MicroBitI2C i2c(I2C_SDA0,I2C_SCL0);
MicroBitPin P1(MICROBIT_ID_IO_P1, MICROBIT_PIN_P1, PIN_CAPABILITY_DIGITAL_OUT);
ssd1306 screen(&uBit, &i2c, &P1);

// Session management
bool isSessionOk = false;
ManagedString key2;
std::string session;
std::string order = "TLHP";

// Réception de la clé 2
void onData(MicroBitEvent) {
    key2 = uBit.radio.datagram.recv();
    
    uBit.serial.printf("Meteo key 2 received: %s\r\n", key2.toCharArray());
    isSessionOk = true;
}

// Réception de l'ordre d'affichage
void onReceive(MicroBitEvent) {
    ManagedString s = uBit.radio.datagram.recv();
    std::string decryptedData = decrypt(s.toCharArray(), session);

    std::string rcvKey = decryptedData.substr(0, 11);

    // Test si sessionKey OK
    if(rcvKey == session) {
        uBit.serial.printf("Meteo order received: %s\r\n", s.toCharArray());
        order = s.toCharArray();
    } else {
        uBit.serial.printf("Meteo data received but wrong key: %s\r\n", s.toCharArray());
    }
}


// Affiche les données sur l'écran et les envoie par RF
void display_rf_loop(bme280 bme, tsl256x tsl, std::string order) {

    // INIT BME
    uint32_t pressure = 0;
    int32_t temp = 0;
    uint16_t humidite = 0;

    // Init TSL
    uint16_t comb =0;
    uint16_t ir = 0;
    uint32_t lux = 0;

    // Récupération des valeurs
    bme.sensor_read(&pressure, &temp, &humidite);
    int tempInt = bme.compensate_temperature(temp);
    int pressInt = bme.compensate_pressure(pressure)/100;
    int humiInt = bme.compensate_humidity(humidite);
    tsl.sensor_read(&comb, &ir, &lux);

    // Récupération de l'ordre
    int tempOrder = strchr(order.c_str(), 'T') + 1;
    int humOrder = strchr(order.c_str(), 'H') + 1;
    int pressOrder = strchr(order.c_str(), 'P') + 1;
    int lumOrder = strchr(order.c_str(), 'L') + 1;

    // Affichage BME
    ManagedString line = "Temp:" + ManagedString(tempInt/100) + "." + ManagedString(tempInt%100) +" C\r\n";
    screen.display_line(tempOrder,0,line.toCharArray());

    line = "Humi:" + ManagedString(humiInt/100) + "." + ManagedString(humiInt%100)+" rH\r\n";
    screen.display_line(humOrder,0,line.toCharArray());

    line = "Press:" + ManagedString(pressInt) +" hPa\r\n";
    screen.display_line(pressOrder,0,line.toCharArray());

    // Affichage TSL
    line = "Lux:" + ManagedString((int)lux) + "\r\n";
    screen.display_line(lumOrder,0,line.toCharArray());

    // Send data
    char tempChar = 'T';
    char lumChar = 'L';
    char humChar = 'H';
    char pressChar = 'P';

    map<char, std::string> data;

    data[tempChar] = to_string(tempInt);
    data[lumChar] = to_string(lux);
    data[pressChar] = to_string(pressInt);
    date[humChar] = to_string(humiInt);
    uBit.serial.printf("Send data\r\n");
    sendRf(&uBit,session, data);

    // Update screen
    screen.update_screen();

}

int main() {

    // Init Micro:bit et capteurs
    uBit.init();
    uBit.radio.enable();
    bme280 bme(&uBit,&i2c);
    tsl256x tsl(&uBit,&i2c);

    // Génère la clé
    int key1 = keyGen(&uBit);
    std::string key1Str = to_string(key1);
    uBit.serial.printf("Meteo key 1 generated: %d\r\n", key1);

    uBit.radio.datagram.send(key1Str.c_str());
    uBit.serial.printf("Meteo key 1 sent\r\nWaiting for key 2...\r\n");

    // Attend la clé pour initier la connection
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);

    while(!isSessionOk) {
        uBit.serial.printf("Meteo still waiting...\r\n");
        uBit.sleep(1000);
    }

    // Connection ok
    uBit.serial.printf("Meteo connection ok\r\n");
    std::string key2Str(key2.toCharArray());
    session = computeKey(&uBit, key1Str, key2Str);
    uBit.serial.printf("Meteo session key: %s\r\n", session.c_str());

    // Boucle de traitement
    while(1) {

        // Affichage
        uBit.serial.printf("Meteo refresh screen\r\n");
        display_rf_loop(bme, tsl, order);
    
        // Reception d'une info de la passerelle
        uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onReceive);

        uBit.sleep(1000);
    }

    release_fiber();
}