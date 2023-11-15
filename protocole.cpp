#include "MicroBit.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <map>
#include <functional>
#include <vector>


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
std::string encrypt(std::string plainText, std::string key) {
    /*
	int dataLen = plainText.size();
	int keyLen = key.size();
	std::string output = plainText;

	for (int i = 0; i < dataLen; ++i) {
		output[i] = plainText[i] ^ key[i % keyLen];
	}

	return output;*/
    return plainText;
}

/**
 * Déchiffre les données reçues par la carte connectée à la passerelle
 * @return std::vector<string> 
*/
std::vector<std::string> decrypt(std::string encryptedData, std::string key) {
    std::string delimiter = " ";
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = encryptedData.find(delimiter, pos_start)) != std::string::npos) {
        token = encryptedData.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(encrypt(token, key));
    }

    res.push_back (encryptedData.substr(pos_start));
    return res;

/**
 * Envoie les données à partir d'une std::string de données non chiffrées
 * @return void
*/
void sendData(MicroBit* microBit, std::string sessionKey, char code, std::string data) {

    // Chiffre les données
    std::string encryptedKey = encrypt(sessionKey, sessionKey);
    std::string encryptedCode = encrypt(std::string(1, code), sessionKey);
    std::string encryptedData = encrypt(data, sessionKey);

    // Concatène les données
    std::string toSend = encryptedKey + " " + encryptedCode + " " + encryptedData;

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
}