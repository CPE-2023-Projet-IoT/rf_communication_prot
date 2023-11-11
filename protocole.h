#include "MicroBit.h"
#include <string>
#include <map>

/**
 * Génère un nombre aléatoire entre 1 et 1000 qui servira de terme pour l'addition afin de convenir d'une clé de session commune
 * @return int
*/
int keyGen(MicroBit* microBit);

/**
 * Envoie le nombre généré aléatoirement par RF
 * @return void
*/
void sendKey(MicroBit* microBit, int key);

/**
 * Utilise les termes générés aléatoirement pour créer une clé commmune
 * @return std::string
*/
std::string computeKey(MicroBit* microBit, std::string key1, std::string key2);

/**
 * Chiffre/déchiffre les données à envoyer
 * @return std::string
*/
std::string encrypt(std::string plainText);

/**
 * Envoie les données à partir d'une std::string de données non chiffrées
 * @return void
*/
void sendData(MicroBit* microBit, std::string sessionKey, char code, std::string data) 

/**
 * Protocole complet d'envoi de données
 * @return void
*/
void sendRf(MicroBit* microBit,std::string sessionKey, map<char, std::string> data);
