#include "MicroBit.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;

/**
 * Génère un nombre aléatoire entre 1 et 1000 qui servira de terme pour l'addition afin de convenir d'une clé de session commune
 * @return int
*/
int keyGen() {

}

/**
 * Envoie le nombre généré aléatoirement par RF
 * @return void
*/
void sendKey(int key) {

}

/**
 * Utilise les termes générés aléatoirement pour les additionner et hasher en SHA256
 * @return string
*/
string computeKey(int key) {

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

