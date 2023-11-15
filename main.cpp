/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "MicroBit.h"
#include "protocole.h"
#include <string>
#include <map>
#include <vector>

MicroBit uBit;

bool session = false;
std::string sessionKey;

void onData(MicroBitEvent)
{
   /*
    *   if (uBit.serial.isReadable()==1){
    *       ManagedString s = uBit.serial.read(sizeof(char)*3);
    *        uBit.display.scroll(s.toCharArray());
    *        uBit.radio.datagram.send(s.toCharArray());
    *   }
    */

    if (session == true){
        ManagedString s = uBit.radio.datagram.recv();

        // Dechiffrement des données
        std::string encryptedData = s.toCharArray();
        // std::vector<std::string> decryptedData = decrypt(encryptedData, sessionKey);

        // uBit.serial.printf("data 0 reçue : %s\r\n", decryptedData[0].c_str());
        // uBit.serial.printf("data 1 reçue : %s\r\n", decryptedData[1].c_str());
        // uBit.serial.printf("data 2 reçue : %s\r\n", decryptedData[2].c_str());

        // uBit.serial.printf("data reçue : %s\r\n", s.toCharArray());

        std::string rcvKey = encryptedData.substr(0, 11);
        //uBit.serial.printf("clé reçu : %s\r\n", rcvKey.c_str());

        // Test si sessionKey OK
        if(rcvKey == sessionKey) {
            // uBit.serial.send(decryptedData[1].c_str());
            // uBit.serial.send(decryptedData[2].c_str());
            std::string code = encryptedData.substr(12, 1);
            std::string data = encryptedData.substr(14);
            if (code == "w") {
                uBit.serial.printf(data.c_str());
            } else {
                uBit.serial.printf(code.c_str());
                uBit.serial.printf(data.c_str());
            }
        }
        
    } else {
        session = true;
        ManagedString s = uBit.radio.datagram.recv();

        int key1 = keyGen(&uBit);
        std::string key1Str = to_string(key1);
        // uBit.serial.printf("Passerelle key Gen : %d\n\r", key1);

        std::string key2Str(s.toCharArray());
        sessionKey = computeKey(&uBit, s.toCharArray(), key1Str);
        // uBit.serial.printf("Size %d\r\n",sessionKey.size());
        // for(unsigned int i = 0; i < sessionKey.capacity();i++)
        // {
        //     uBit.serial.send(sessionKey[i]);
        // }

        // uBit.serial.printf("Passerelle last receive\r\n");
        // uBit.serial.printf(s.toCharArray());
        // uBit.serial.printf("Passerelle sessionKey : %s\n\r", sessionKey.c_str());
        uBit.sleep(1000);
        uBit.radio.datagram.send(key1Str.c_str());
    }

}

int main()
{
    // Initialise the micro:bit runtime.

    uBit.init();
    uBit.radio.enable();

    // uBit.serial.printf("début\r\n");
    
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);

    //release_fiber();
    while (1){
        uBit.sleep(10000);
        if (uBit.serial.getRxBufferSize()>0){
            ManagedString s = uBit.serial.read(sizeof(char)*3); // ou read de la taille du buffer
            uBit.display.scroll(s.toCharArray());
            uBit.radio.datagram.send(s.toCharArray());
        }
    }
}

