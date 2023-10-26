#include "MicroBit.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <map>


/**
 * Génère un nombre aléatoire entre 1 et 1000 qui servira de terme pour l'addition afin de convenir d'une clé de session commune
 * @return int
*/
int keyGen(MicroBit* microBit) {
    return microBit->random(999)+1;
}

/**
 * Envoie le nombre généré aléatoirement par RF
 * @return void
*/
void sendKey(MicroBit* microBit, int key) {
    std::string charKey = to_string(key);
    microBit->radio.datagram.send(charKey.c_str());
}

/**
* Utilise les termes générés aléatoirement pour les additionner et les hasher
* @return std::string
*/
std::string computeKey(std::string key1, std::string key2) {
    int key1Size = key1.size();
    int key2Size = key2.size();
    std::string concatKey="";
    
    for (int i=0; i<min(key1Size,key2Size); i++){
        concatKey[2*i] += key1[i];
        concatKey[2*i+1] += key2[i];
    }

    int newKey;
    sscanf(concatKey.c_str(), "%d", &newKey);
    newKey = ((newKey*(3 * 100)/4)^9)%1000000000;
    std::string computedKey = to_string(newKey);
    computedKey = "$" + computedKey;
    computedKey[2] = '&';
    computedKey[4] = 'a';
    computedKey[5] = 'e';

    return computedKey;
}

/**
 * Chiffre les données à envoyer
 * @return std::string
*/
std::string encrypt(std::string plainText) {
    const std::string KEY = "3nCrYp710N";
    std::string cipherText = plainText;

    // TODO

    return cipherText;
}

/**
 * Déchiffre les données reçues
 * @return std::string
*/
std::string decrypt(std::string cipherText) {
    const std::string KEY = "3nCrYp710N";
    std::string plainText = cipherText;

    // TODO

    return plainText;
}

/**
 * Envoie les données à partir d'une std::string de données non chiffrées
 * @return void
*/
void sendData(MicroBit* microBit,char code, std::string data) {

    // Chiffre les données
    std::string encryptedCode = encrypt(std::string(1, code));
    std::string encryptedData = encrypt(data);

    // Envoie les données
    microBit->radio.datagram.send(encryptedCode.c_str());
    microBit->radio.datagram.send(encryptedData.c_str());
}

/**
 * Protocole complet d'envoi de données
 * @return void
*/
void sendRf(MicroBit* microBit, std::string sessionKey, map<char, std::string> data) {

    // Envoie la clé de session
    microBit->radio.datagram.send(sessionKey.c_str());

    // Envoie les données
    for(auto i = data.begin(); i != data.end(); i++) {
        sendData(microBit,i->first, i->second);
    }

    // Reset la connexion
    std::string resetCode = "RST";
    microBit->radio.datagram.send(resetCode.c_str());
}