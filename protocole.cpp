#include "MicroBit.h"
#include <cstdlib>
#include <ctime>
#include <string>

/**
* Génère un nombre aléatoire entre 1 et 1000 qui servira de terme pour l'addition afin de convenir d'une clé de session commune
* @return int
*/
int keyGen() {
    srand(time(0));
    int key = rand() % 1000;
}

/**
* Envoie le nombre généré aléatoirement par RF
* @return void
*/
void sendKey(int key) {
    std::string charKey = std::to_string(key);
    uBit.radio.datagram.send(charKey);
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
}


void sendData(char code, string data) {
uBit.radio.datagram.send("T");

}

void send() {

}

