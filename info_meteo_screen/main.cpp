#include "MicroBit.h"
#include "bme280.h"
#include "veml6070.h"
#include "tsl256x.h"
#include "ssd1306.h"

MicroBit uBit;
MicroBitI2C i2c(I2C_SDA0,I2C_SCL0);
MicroBitPin P1(MICROBIT_ID_IO_P1, MICROBIT_PIN_P1, PIN_CAPABILITY_DIGITAL_OUT); //pin de reset
ssd1306 screen(&uBit, &i2c, &P1);
int conf_affichage = 0;

void onData(MicroBitEvent)
{
    PacketBuffer p = uBit.radio.datagram.recv();
    int t[3];
    memcpy(&t,p.getBytes(),3*sizeof(int));
    
    if(t[0]==7)
    {
        if(t[1]==2)
        {
            conf_affichage = t[2];  
        }
    }
}

int main()
{
    int temp_2[7];

    // Initialise the micro:bit runtime.
    uBit.init();
    bme280 bme(&uBit,&i2c);
    uint32_t pressure = 0;
    int32_t temp = 0;
    uint16_t humidite = 0;

    veml6070 veml(&uBit,&i2c);
    uint16_t uv = 0;

    tsl256x tsl(&uBit,&i2c);
    uint16_t comb =0;
    uint16_t ir = 0;
    uint32_t lux = 0;

    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();
    

    while(true)
    {
        //BME
        bme.sensor_read(&pressure, &temp, &humidite);
        int tmp = bme.compensate_temperature(temp);
        int pres = bme.compensate_pressure(pressure)/100;
        int hum = bme.compensate_humidity(humidite);
        veml.sensor_read(&uv);
        tsl.sensor_read(&comb, &ir, &lux);

        temp_2[0]=25;        //change le protocole Mr CS2
        temp_2[1]=1;
        temp_2[2]=tmp;
        temp_2[3]=pres;
        temp_2[4]=hum;
        temp_2[5]=uv;
        temp_2[6]=lux;

        ManagedString display = "Temp:" + ManagedString(tmp/100) + "." + (tmp > 0 ? ManagedString(tmp%100): ManagedString((-tmp)%100))+" C\r\n";
        //ManagedString display = "Temp:" + ManagedString(tmp);
        screen.display_line(conf_affichage,0,display.toCharArray());
        //uBit.serial.printf(display.toCharArray());

        display = "Humi:" + ManagedString(hum/100) + "." + ManagedString(tmp%100)+" rH\r\n";
        //uBit.serial.printf(display.toCharArray());
        screen.display_line(conf_affichage+1,0,display.toCharArray());

        display = "Pres:" + ManagedString(pres)+" hPa\r\n";
        //uBit.serial.printf(display.toCharArray());
        screen.display_line(conf_affichage+2,0,display.toCharArray());

        //VEML
        
        display = "UV:" + ManagedString(uv) + "\r\n";
        //uBit.serial.printf(display.toCharArray());
        screen.display_line(conf_affichage+3,0,display.toCharArray());

        //TSL

        
        display = "Lux:" + ManagedString((int)lux) + "\r\n";
        //uBit.serial.printf(display.toCharArray());
        screen.display_line(conf_affichage+4,0,display.toCharArray());


        display = "trame:" + ManagedString((int)conf_affichage)+ "\r\n";
        screen.display_line(conf_affichage+5,0,display.toCharArray());
        
        PacketBuffer packet(7*sizeof(int));
        memcpy(packet.getBytes(),temp_2,7*sizeof(int));
        uBit.radio.datagram.send(packet);

        screen.update_screen();

        uBit.sleep(1000);
    }
    release_fiber();
    

}