#ifndef OFFSETS_H
#define OFFSETS_H

namespace Offsets {
    enum Offsets {
		//Global
        GWorld = 0x70AAE1C,//GWorld = 0x6333120,
        GNames = 0x70A1550, //GNames = 0x65C1D64,
		PointerSize = 0x4,

		//Struct Size
		FTransformSizeInGame = 0x30,

		//---------SDK-----------
		//Class: FNameEntry
		FNameEntryToNameString = 0x8,
		//Class: UObject
		UObjectToInternalIndex = 0x8,
		UObjectToFNameIndex = 0x10,

		//---------PUBG UEClasses-----------
		//Class: World
		WorldToPersistentLevel = 0x20,
		//Class: Level
		LevelToAActors = 0x70, //not updated //0x70
		//Class: PlayerController
		UAEPlayerControllerToPlayerKey = 0x62C,
		UAEPlayerControllerToTeamID = 0x644,
		//Class: PlayerCameraManager
		PlayerCameraManagerToCameraCacheEntry = 0x350,
		//Class: CameraCacheEntry
		CameraCacheEntryToMinimalViewInfo = 0x10,
		//Class: SceneComponent
		SceneComponentToComponentToWorld = 0x150, //not updated  //0x1AC //0x140
		//Class: SkeletalMeshComponent
		SkeletalMeshComponentToCachedComponentSpaceTransforms = 0x710,
		//Class: Actor
		ActorToRootComponent = 0x14c,
		//Class: Character
		CharacterToMesh = 0x320,
		//Class: UAECharacter
		UAECharacterToPlayerName = 0x648,
		UAECharacterToPlayerKey = 0x650,
		UAECharacterToTeamID = 0x670,
		UAECharacterTobIsAI = 0x6e8,
		//Class: STExtraCharacter
        STExtraCharacterToHealth = 0x928,//STExtraCharacterToHealth = 0x788,
    };
}

#endif
