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

MicroBit uBit;

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    uBit.radio.enable();

    // Insert your code here!
    int key1 = keyGen();
    std::string key1Str = to_string(key1);
    ManagedString key2 = uBit.radio.datagram.recv();
    std::string key2Str = key2.toCharArray();

    uBit.radio.datagram.send(key1Str.c_str());

    std::string session = computeKey(key1Str, key2Str);

    char temp = 'T';
    char lum = 'L';

    std::string tempStr = "25";
    std::string lumStr = "48";

    map<char, std::string> data;
    data[temp] = tempStr;
    data[lum] = lumStr;

    while(1) {
        sendRf(session, data);
        uBit.sleep(100);
    }

    release_fiber();
}

