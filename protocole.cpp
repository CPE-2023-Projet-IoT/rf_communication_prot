#include "MicroBit.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <string>
#include <map>

#define PI 3,1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679

using namespace std;

/**
 * Génère un nombre aléatoire entre 1 et 1000 qui servira de terme pour l'addition afin de convenir d'une clé de session commune
 * @return int
*/
int keyGen() {
    srand(time(0));
    int key = rand() % 1000;

    return (key);
}

/**
 * Envoie le nombre généré aléatoirement par RF
 * @return void
*/
void sendKey(int key) {
    string charKey = to_string(key);
    uBit.radio.datagram.send(charKey);
}

/**
* Utilise les termes générés aléatoirement pour les additionner et les hasher
* @return string
*/
string computeKey(string key1, string key2) {
    int key1Size = key1.size();
    int key2Size = key2.size();
    string concatKey="";
    
    for (int i=0; i<min(key1Size,key2Size); i++){
        concatKey[2i] += key1[i];
        concatKey[2i+1] += key2[i];
    }

    int newKey;
    sscanf(concatKey, "%d", &newKey);
    newKey = ((newKey*(PI * 100)/4)^9)%1000000000;
    string computedKey = to_string(newKey);
    computedKey = "$" + computedKey;
    computedKey[2] = "&";
    computedKey[4] = "a";
    computedKey[5] = "e";

    return computedKey;
}

/**
 * Chiffre les données à envoyer
 * @return string
*/
string encrypt(string plainText) {
    const string KEY = "3nCrYp710N";
    string cipherText = plainText;

    // TODO

    return cipherText;
}

/**
 * Déchiffre les données reçues
 * @return string
*/
string decrypt(string cipherText) {
    const string KEY = "3nCrYp710N";
    string plainText = cipherText;

    // TODO

    return plainText
}

/**
 * Envoie les données à partir d'une string de données non chiffrées
 * @return void
*/
void sendData(char code, string data) {

    // Chiffre les données
    string encryptedCode = encrypt(string(1, code));
    string encryptedData = encrypt(data);

    // Envoie les données
    uBit.radio.datagram.send(encryptedCode);
    uBit.radio.datagram.send(encryptedData);
}

/**
 * Protocole complet d'envoi de données
 * @return void
*/
void send(string sessionKey, map<char, string> data) {

    // Envoie la clé de session
    uBit.radio.datagram.send(sessionKey);

    // Envoie les données
    for(auto i = data.begin(); i != data.end(); i++) {
        sendData(i->first, i->second);
    }
}