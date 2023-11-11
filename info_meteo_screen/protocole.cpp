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
 * Chiffre/déchiffre les données à envoyer
 * @return std::string
*/
std::string encryp(std::string plainText) {
    const std::string key = "3nCrYp710N";
	int dataLen = plainText.size();
	int keyLen = key.size();
	std::string output = plainText;

	for (int i = 0; i < dataLen; ++i) {
		output[i] = plainText[i] ^ key[i % keyLen];
	}

	return output;
}

/**
 * Envoie les données à partir d'une std::string de données non chiffrées
 * @return void
*/
void sendData(MicroBit* microBit, std::string sessionKey, char code, std::string data) {

    // Chiffre les données
    std::string encryptedCode = encrypt(std::string(1, code));
    std::string encryptedData = encrypt(data);

    // Envoie le code
    microBit->radio.datagram.send(sessionKey.c_str());

    // Envoie les données
    microBit->radio.datagram.send(encryptedCode.c_str());
    microBit->radio.datagram.send(encryptedData.c_str());
}

/**
 * Protocole complet d'envoi de données
 * @return void
*/
void sendRf(MicroBit* microBit, std::string sessionKey, map<char, std::string> data) {

    // Envoie les données
    for(auto i = data.begin(); i != data.end(); i++) {
        sendData(sessionKey, microBit,i->first, i->second);
    }
}