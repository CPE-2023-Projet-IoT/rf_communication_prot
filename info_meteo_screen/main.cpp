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

void onData(MicroBitEvent) {
    
    key2 = uBit.radio.datagram.recv();
    uBit.serial.printf("Key 2 received: %s\r\n", key2.toCharArray());
    isSessionOk = true;
}

void display_rf_loop(bme280 bme, tsl256x tsl) {

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
    int tempe = bme.compensate_temperature(temp);
    int press = bme.compensate_pressure(pressure)/100;
    int humi = bme.compensate_humidity(humidite);
    tsl.sensor_read(&comb, &ir, &lux);

    // Affichage BME
    ManagedString line = "Temp:" + ManagedString(tempe/100) + "." + ManagedString(tempe%100) +" C\r\n";
    screen.display_line(1,0,line.toCharArray());

    line = "Humi:" + ManagedString(humi/100) + "." + ManagedString(humi%100)+" rH\r\n";
    screen.display_line(2,0,line.toCharArray());

    line = "Press:" + ManagedString(press) +" hPa\r\n";
    screen.display_line(3,0,line.toCharArray());

    // Affichage TSL
    line = "Lux:" + ManagedString((int)lux) + "\r\n";
    screen.display_line(4,0,line.toCharArray());


    // Send data
    char temp = 'T';
    char lum = 'L';
    char hum = 'H';
    char press = 'P';

    std::string tempStr = to_string(tempe);
    std::string lumStr = to_string(lux);
    std::string pressStr = to_string(press);
    std::string humStr = to_string(humi);

    map<char, std::string> data;

    data[temp] = tempStr;
    data[lum] = lumStr;
    data[press] = pressStr;
    date[hum] = humStr;
    uBit.serial.printf("Meteo send data\r\n");
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
    std::string session = computeKey(&uBit, key1Str, key2Str);
    uBit.serial.printf("Meteo session key: %s\r\n", session.c_str());

    // Boucle de traitement
    while(1) {

        // Affichage
        uBit.serial.printf("Meteo refresh screen\r\n");
        display_rf_loop(bme, tsl);

        uBit.sleep(1000);
    }

    release_fiber();
}