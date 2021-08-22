#ifndef KMODS_H
#define KMODS_H

#include <jni.h>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>
#include <android/log.h>
#include "StructsCommon.h"
#include "Log.h"

bool isESP = false;
//Player
bool isPlayerName = false;
bool isPlayerDist = false;
bool isPlayerHealth = false;
bool isTeamMateShow = false;
bool isPlayerBox = false;
bool isPlayerLine = false;
bool isPlayer360 = false;
bool isNearEnemy= false;
bool isPlayerSkel= false;
float playerTextSize = 15;
//Items
bool isVehicle = false;
bool isLootBox = false;
bool isAirDrop = false;
bool isLootItems = false;
bool isItemName = false;
bool isItemDist = false;
float itemTextSize = 15;
bool  isIgnoreBots = false;

bool  isAimbot = false;
bool  isRecoil = false;
bool  isBlacksky = false;
bool  isNofog = false;
bool  isBypass = false;
bool  isMagicbullet = false;
bool  isFlash = false;
bool isHeadshot = false;


bool  isAimbotActive = false;
bool  isRecoilActive = false;
bool  isBSActive  = false;
bool  isNFActive  = false;
bool  isBypassActive = false;
bool  isMagicBulletAcive = false;
bool  isFlashActive = false;
bool  isHeadshotActive = false;



int startClient();
bool isConnected();
void stopClient();
bool initServer();
bool stopServer();
Response getData(int screenWidth, int screenHeight);

#endif
