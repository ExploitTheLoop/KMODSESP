#include <zconf.h>

#include <iostream>
#include "kmods.h"
#include "Socket/SocketServer.h"
#include "StructsCommon.h"
#include "FNames.h"
#include "PUBG.h"


using namespace std;

SocketServer server;

#define maxplayerCount 30
#define maxitemsCount 40

enum Mode {
    InitMode = 1,
    ESPMode = 2,
    HackMode = 3,
    StopMode = 4,
    MemMode = 5,

};

struct Request {
	int Mode;
	int ScreenWidth;
	int ScreenHeight;
	int HackNum;
};

struct PlayerData {
    wchar_t PlayerName[30];
    bool isBot;
    int TeamID;
    float Health;
    float Distance;
    Vector2 Body;
    Vector2 Root;
    Vector2 Head;
    Vector2 Neck;
    Vector2 Chest;
    Vector2 Pelvis;
    Vector2 LShoulder;
    Vector2 RShoulder;
    Vector2 LElbow;
    Vector2 RElbow;
    Vector2 LWrist;
    Vector2 RWrist;
    Vector2 LThigh;
    Vector2 RThigh;
    Vector2 LKnee;
    Vector2 RKnee;
    Vector2 LAnkle;
    Vector2 RAnkle;
};

struct ItemData {
    char Name[20];
    bool isVehicle;
    bool isLootBox;
    bool isAirDrop;
    bool isLootItem;
    float Distance;
    Vector2 Location;
};

struct Response {
    bool Success;
    int NearEnemy;
    int MyTeamID;
    int PlayerCount;
    int ItemsCount;
    PlayerData Players[maxplayerCount];
    ItemData Items[maxitemsCount];
};

static const char* vehicles[][2] = {
		{"AirDropPlane", "AirDropPlane"},
		{"BP_VH_Buggy", "Buggy"},
		{"VH_Dacia", "Dacia"},
		{"BP_VH_Tuk", "Tuk Tuk"},
		{"Rony_0", "Rony"},
		{"Mirado_close", "Mirado"},
		{"Mirado_open", "Mirado Open"},
		{"VH_UAZ", "UAZ"},
		{"VH_MiniBus", "MiniBus"},
		{"VH_Scooter", "Scooter"},
		{"VH_MotorcycleCart", "Motorcycle Cart"},
		{"VH_Motorcycle", "Motorcycle"},
		{"VH_Snowbike", "SnowBike"},
		{"VH_Snowmobile", "SnowMobile"},
		{"VH_BRDM", "BRDM"},
		{"LadaNiva_0", "LadaNiva"},
		{"PickUp_0", "PickUp Truck"},
		{"VH_PG117", "BigBoat"},
		{"AquaRail_", "AquaRail"},
};

bool FindVehicleName(const string& object, string& res) {
	for (auto &vehicle : vehicles) {
		if (isContain(object, vehicle[0])) {
			res = string(vehicle[1]);
			return true;
		}
	}
	return false;
}

static const char* impitems[][2] = {

		{"PickUp_BP_Armor_Lv3", "Armor L3"},
		{"PickUp_BP_Helmet_Lv3", "Helmet L3"},
		{"PickUp_BP_Bag_Lv3", "Bag L3"},
		{"BP_MZJ_HD_Pickup", "RedDot"},
		{"BP_MZJ_8X_Pickup", "8x"},
		{"BP_MZJ_6X_Pickup", "6x"},
		{"BP_MZJ_4X_Pickup", "4x"},
		{"BP_Ghillie_", "Ghillie"},
		/*{"SCAR", "SCAR-L"},
		{"M416", "M416"},
		{"AKM", "AKM"},
		{"DP28", "DP28"},  //Kar98k M24 Mini14 Ammo_762mm Ammo_556mm Sniper_Suppressor
		{"Kar98k", "K98"},
		{"M24", "M24"},
		{"Mini14", "M14"},
		{"Ammo_762mm", "7.62"},
		{"Ammo_556mm", "5.56"},
		{"Sniper_Suppressor", "Supressor SR"},
		{"Large_Suppressor", "Supressor AR"},
		{"Flare", "Flare Gun"},*/

};

bool FindItemName(const string& object, string& res) {
	for (auto &items : impitems) {
		if (isContain(object, items[0])) {
			res = string(items[1]);
			return true;
		}
	}
	return false;
}

bool isOutsideSafeZone(Vector2 pos) {
	if (pos.Y < 0) {
		return true;
	}
	if (pos.X > (float)SWidth) {
		return true;
	}
	if (pos.Y > (float)SHeight) {
		return true;
	}
	return pos.X < 0;
}

void createDataList(Response& response) {
    response.Success = false;
    response.PlayerCount = 0;
    response.ItemsCount = 0;

    kaddr gworld = getRealOffset(Offsets::GWorld);
    if(gworld == 0){ return; }

    kaddr uworld = getPtr(gworld);
    if(UObject::isInvalid(uworld)){ return; }

    kaddr level = World::getPersistentLevel(uworld);
    if (UObject::isInvalid(level)) { return; }

    kaddr actorList = Level::getActorList(level);
    if (actorList == 0) { return; }

    int nearEnemy = 0;
    int localTeamID = 0;
    uint32 localPKey = 0;
    MinimalViewInfo POV = MinimalViewInfo();

    for (int i = 0; i < Level::getActorsCount(level); i++) {
        kaddr actor = Level::getActor(actorList, i);
        if (UObject::isInvalid(actor)) { continue; }

        //LOGE("Actor[%i]: %x | %s", i, actor, UObject::getName(actor).c_str());

        string oname = UObject::getName(actor);

        if(isContain(oname, "BP_STExtraPlayerController")){
            localTeamID = PlayerController::getTeamID(actor);
            localPKey = PlayerController::getPlayerKey(actor);
            continue;
        }

        if(isContain(oname, "BP_PlayerCameraManager")){
            POV = PlayerCameraManager::getPOV(actor);
            continue;
        }

        if(localPKey == 0 || POV.FOV == 0){ continue; }

        kaddr RootComp = Actor::getRootComponent(actor);
        if (UObject::isInvalid(RootComp)) { continue; }

        Vector3 Location = SceneComponent::getLocation(RootComp);
        float distance = (Vector3::Distance(Location, POV.Location) / 100.0f);

        if (isContain(oname, "BP_PlayerPawn_") && !isContain(oname, "BP_PlayerPawn_Statue_")) {
            if (response.PlayerCount == maxplayerCount) {
                continue;
            }

            PlayerData* data = &response.Players[response.PlayerCount];

            if(distance > 500) { continue; }

            if(localPKey == Player::getPlayerKey(actor)){ continue;	}

            int teamID = Player::getTeamID(actor);
            if(localTeamID != teamID){
                nearEnemy++;
            }

			kaddr Mesh = Character::getMeshComponent(actor);
			if (UObject::isInvalid(Mesh)) { continue; }

			FTransform c2wTransform = SceneComponent::getComponentToWorld(Mesh);
			FMatrix c2wMatrix = TransformToMatrixWithScale(c2wTransform);

			kaddr BoneArr = SkeletalMeshComponent::getBoneArr(Mesh);
			if (BoneArr == 0) { continue; }

			FString pname = Player::getPlayerName(actor);

			wmemcpy(data->PlayerName, pname.w_str(), pname.Count);
            data->isBot = Player::IsAI(actor);
            data->TeamID = teamID;
            data->Health = Player::getHealth(actor);
            data->Distance = distance;
            data->Body = WorldToScreen(Location, POV, SWidth, SHeight);
            data->Root = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 0) - Vector3(0,0,7), POV, SWidth, SHeight);
            data->Head = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 6) + Vector3(0,0,22), POV, SWidth, SHeight);
            data->Neck = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 6) + Vector3(0,0,10), POV, SWidth, SHeight);
            data->Chest = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 5), POV, SWidth, SHeight);
            data->Pelvis = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 2), POV, SWidth, SHeight);
            data->LShoulder = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 12), POV, SWidth, SHeight);
            data->RShoulder = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 33), POV, SWidth, SHeight);
            data->LElbow = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 13), POV, SWidth, SHeight);
            data->RElbow = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 34), POV, SWidth, SHeight);
            data->LWrist = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 64), POV, SWidth, SHeight);
            data->RWrist = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 63), POV, SWidth, SHeight);
            data->LThigh = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 53), POV, SWidth, SHeight);
            data->RThigh = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 57), POV, SWidth, SHeight);
            data->LKnee = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 54), POV, SWidth, SHeight);
            data->RKnee = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 58), POV, SWidth, SHeight);
            data->LAnkle = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 55), POV, SWidth, SHeight);
            data->RAnkle = WorldToScreen(Player::getBoneLoc(c2wMatrix, BoneArr, 59), POV, SWidth, SHeight);

            response.PlayerCount++;
        } else if (isContain(oname, "PlayerDeadInventoryBox")) {
            if (response.ItemsCount == maxitemsCount) {
                continue;
            }

            Vector2 SLoc = WorldToScreen(Location, POV, SWidth, SHeight);
			if (isOutsideSafeZone(SLoc)) { continue; }

            ItemData* data = &response.Items[response.ItemsCount];

            if(distance > 500) { continue; }

            strncpy(data->Name, "LootBox", 7);
            data->isVehicle = false;
            data->isLootBox = true;
            data->isAirDrop = false;
            data->isLootItem = false;
            data->Distance = distance;
            data->Location = SLoc;

            response.ItemsCount++;
        } else if(isContain(oname, "AirDropBox")) {
            if (response.ItemsCount == maxitemsCount) {
                continue;
            }

            Vector2 SLoc = WorldToScreen(Location, POV, SWidth, SHeight);
            if (isOutsideSafeZone(SLoc)) { continue; }

            if(distance > 1000) { continue; }

            ItemData* data = &response.Items[response.ItemsCount];

            strncpy(data->Name, "AirDrop", 6);
            data->isVehicle = false;
            data->isLootBox = false;
            data->isAirDrop = true;
            data->isLootItem = false;
            data->Distance = distance;
            data->Location = SLoc;

            response.ItemsCount++;
        } else {
            string itemName;
            if(FindVehicleName(oname, itemName)){
                if (response.ItemsCount == maxitemsCount) {
                    continue;
                }

				Vector2 SLoc = WorldToScreen(Location, POV, SWidth, SHeight);
				if (isOutsideSafeZone(SLoc)) { continue; }

                if(distance < 10 || distance > 800) { continue; }

                ItemData* data = &response.Items[response.ItemsCount];

                strncpy(data->Name, itemName.data(), itemName.size());
                data->isVehicle = true;
                data->isLootBox = false;
                data->isAirDrop = false;
                data->isLootItem = false;
                data->Distance = distance;
                data->Location = SLoc;

                response.ItemsCount++;
            } else if(FindItemName(oname, itemName)){
                if (response.ItemsCount == maxitemsCount) {
                    continue;
                }

				Vector2 SLoc = WorldToScreen(Location, POV, SWidth, SHeight);
				if (isOutsideSafeZone(SLoc)) { continue; }

                if(distance > 800) { continue; }

                ItemData* data = &response.Items[response.ItemsCount];

                strncpy(data->Name, itemName.data(), itemName.size());
                data->isVehicle = false;
                data->isLootBox = false;
                data->isAirDrop = false;
                data->isLootItem = true;
                data->Distance = distance;
                data->Location = SLoc;

                response.ItemsCount++;
            }
        }
    }

    response.Success = true;
    response.NearEnemy = nearEnemy;
    response.MyTeamID = localTeamID;
}

std::string getRealOffsets(Byte byte[], int size){

	char str[size + 1];
	memcpy(str, byte, size);
	str[size] = 0; // Null termination.


	return str;
}
void killProcess(char *pkg){
	char ss[100];
	sprintf(ss,"am force-stop %s",pkg);
	system(ss);
}



int main(int argc, char *argv[]) {
	if (!server.Create()) {
		LOGE("SE:1");
		return -1;
	}

	if (!server.Bind()) {
		LOGE("SE:2");
		return -1;
	}

	if (!server.Listen()) {
		LOGE("SE:3");
		return -1;
	}
	
	if(argc < 2){
		LOGE("SE:4");
		return -1;
	}

	//vkey();

	if (server.Accept()) {
		int mode = atoi(argv[1]);
		if(mode == 1){
			target_pid = find_pid("com.tencent.ig");
			if (target_pid == -1) {
				LOGE("Can't find the process\n");
				return -1;
			}
		} else if(mode == 2){
			target_pid = find_pid("com.pubg.krmobile");
			if (target_pid == -1) {
				LOGE("Can't find the process\n");
				return -1;
			}
		} else if(mode == 3){
			target_pid = find_pid("com.vng.pubgmobile");
			if (target_pid == -1) {
				LOGE("Can't find the process\n");
				return -1;
			}
		} else if(mode == 4){
			target_pid = find_pid("com.rekoo.pubgm");
			if (target_pid == -1) {
				LOGE("Can't find the process\n");
				return -1;
			}
		} else {
			LOGE("Invalid Game Choice\n");
			return -1;
		}

		libbase = get_module_base(lib_name);
		if (libbase == 0) {
			LOGE("Can't find module\n");
			return -1;
		}

		LOGW("Base Address of %s Found At %x\n", lib_name, libbase);

		Request request{};
		while (server.receive((void*)& request) > 0) {
			Response response{};

			if (request.Mode == InitMode) {
				response.Success = (libbase > 0);
			}
			else if (request.Mode == ESPMode) {
				SWidth = request.ScreenWidth;
				SHeight = request.ScreenHeight;
				createDataList(response);
			}
			else if (request.Mode == HackMode) {
				response.Success = true;
			}

			else if (request.Mode == StopMode) {
				response.Success = true;
				server.send((void*)& response, sizeof(response));
				break;
			}

			server.send((void*)&response, sizeof(response));
		}
	}
	
	return 0;
}






