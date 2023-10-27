#include "MicroBit.h"
#include "bme280.h"
#include "tsl256x.h"
#include "ssd1306.h"

MicroBit uBit;
MicroBitI2C i2c(I2C_SDA0,I2C_SCL0);
MicroBitPin P1(MICROBIT_ID_IO_P1, MICROBIT_PIN_P1, PIN_CAPABILITY_DIGITAL_OUT);
ssd1306 screen(&uBit, &i2c, &P1);

void affichage_oled(bme280 bme, tsl256x tsl) {

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
    screen.display_line(4,0,line.toCharArray());

    line = "Humi:" + ManagedString(humi/100) + "." + ManagedString(humi%100)+" rH\r\n";
    screen.display_line(5,0,line.toCharArray());

    line = "Press:" + ManagedString(press) +" hPa\r\n";
    screen.display_line(6,0,line.toCharArray());

    // Affichage TSL
    line = "Lux:" + ManagedString((int)lux) + "\r\n";
    screen.display_line(7,0,line.toCharArray());

    // Update screen and sleep
    screen.update_screen();
    uBit.sleep(500);
}

int main() {

    // Init Micro:bit et capteurs
    uBit.init();
    bme280 bme(&uBit,&i2c);
    tsl256x tsl(&uBit,&i2c);

    // Boucle d'affichage des données
    while(true) {
        affichage_oled(bme, tsl);
    }

    release_fiber();
}