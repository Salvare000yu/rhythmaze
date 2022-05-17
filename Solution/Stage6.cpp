#include "Stage6.h"

#include "SceneManager.h"

#include <sstream>
#include <iomanip>

#include <xaudio2.h>
#include "RandomNum.h"

#include <fstream>
#include <string>
#include <vector>

using namespace DirectX;

void Stage6::pathInit() {
	mapCSVFilePath = "Resources/map/map_Stage6.csv";

	//boxModelPath = L"Resources/model/box/box.obj";
	//boxModelTexPath = L"Resources/model/box/box.png";

	//playerModelPath = L"Resources/model/player/model.obj";
	//playerModelTexPath = L"Resources/model/player/tex.png";

	//effectTexPath = L"Resources/effect1.png";

	bgmFilePath = "Resources/Music/A_rhythmaze_125.wav";

	//particleSeFilePath = "Resources/SE/Sys_Set03-click.wav";
}

// í«â¡Ç≈èâä˙âªÇ™ïKóvÇ»ÇÁÇ±ÇÃä÷êîÇÃíÜÇ…èëÇ≠
void Stage6::additionalInit() {
	stageNum = 6u;

	musicBpm = 125.f;

	bgmBolume = 0.4f;

	clearCount = 120u;

	spriteInit();
}

void Stage6::spriteInit() {
	for (size_t i = 0u; i < SPRITES_NUM; i++) {
		sprites[i].create(dxCom->getDev(),
						  WinAPI::window_width, WinAPI::window_height,
						  TEX_NUM::TEX1,
						  spriteCommon,
						  XMFLOAT2(0, 0),
						  false, false);
	}
}
