#include "MicroBit.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <map>
#include <functional>


/**
 * Génère un nombre aléatoire entre 1 et 1000 qui servira de terme pour l'addition afin de convenir d'une clé de session commune
 * @return int
*/
int keyGen(MicroBit* microBit) {
    return (microBit->random(999)+1);
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
* Utilise les termes générés aléatoirement pour créer une clé de session commune
* @return std::string
*/
std::string computeKey(MicroBit* microBit, std::string key1, std::string key2) {


    std::string str1 (key1.c_str());
    std::string str2 (key2.c_str());

    std::hash<std::string> str_hash;
    std::string hashedKey1 = to_string((str_hash(str1)));
    std::string hashedKey2 = to_string((str_hash(str2)));
    microBit->serial.printf("hash1 : %s\r\n", hashedKey1.c_str());
    microBit->serial.printf("hash2 : %s\r\n", hashedKey2.c_str());


    int intHashedKey1;
    int intHashedKey2;

    sscanf(hashedKey1.c_str(), "%d", &intHashedKey1);
    sscanf(hashedKey2.c_str(), "%d", &intHashedKey2);

    int xorKey = (intHashedKey1 ^ intHashedKey2);
    std::string computedKey = to_string(xorKey);
    microBit->serial.printf("computed : %s\r\n", computedKey.c_str());

    computedKey = "R" + computedKey;
    computedKey[2] = 'T';
    computedKey[4] = 'A';
    computedKey[5] = 'E';

    return computedKey;
}

/**
* Chiffre le texte en utilisant une clé alphanumérique
* @param texte Texte à chiffrer
* @param cle Clé alphanumérique
*
* @return std::string
*/
std::string encrypt(const std::string &texte, const std::string &cle) {
    std::string texteChiffre = texte;
    int longueurCle = cle.length();
    
    for (size_t i = 0; i < texte.length(); ++i) {
        texteChiffre[i] = (texte[i] ^ cle[i % longueurCle]) % 94 + 32;
    }
    
    return texteChiffre;
}

/**
* Déchiffre le texte en utilisant une clé alphanumérique
* @param textChiffre Texte à déchiffrer
* @param cle Clé alphanumérique
*
* @return std::string
*/
std::string decrypt(const std::string &texteChiffre, const std::string &cle) {
    std::string texteDechiffre = texteChiffre;
    int longueurCle = cle.length();
    
    for (size_t i = 0; i < texteChiffre.length(); ++i) {
        texteDechiffre[i] = (texteChiffre[i] ^ cle[i % longueurCle]) % 94 + 32;
    }
    
    return texteDechiffre;
}

/**
 * Envoie les données à partir d'une std::string de données non chiffrées
 * @return void
*/
void sendData(MicroBit* microBit, std::string sessionKey, char code, std::string data) {

    // Chiffre les données

    // Concatène les données
    std::string toSend = encrypt(encryptedKey + " " + encryptedCode + " " + encryptedData);

    // Envoie les données
    microBit->radio.datagram.send(toSend.c_str());
}

/**
 * Protocole complet d'envoi de données
 * @return void
*/
void sendRf(MicroBit* microBit, std::string sessionKey, map<char, std::string> data) {

    // Envoie les données
    for(auto i = data.begin(); i != data.end(); i++) {
        sendData(microBit, sessionKey,i->first, i->second);
    }
    std::string toSend = encrypt(sessionKey + " " + "w" + " " + "\n\r");
    microBit->radio.datagram.send(toSend.c_str());
}