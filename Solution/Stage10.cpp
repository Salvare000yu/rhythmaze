#include "Stage10.h"

#include "SceneManager.h"

#include <sstream>
#include <iomanip>

#include <xaudio2.h>
#include "RandomNum.h"

#include <fstream>
#include <string>
#include <vector>

using namespace DirectX;

void Stage10::pathInit() {
	mapCSVFilePath = "Resources/map/map_stage10.csv";

	//boxModelPath = L"Resources/model/box/box.obj";
	//boxModelTexPath = L"Resources/model/box/box.png";

	//playerModelPath = L"Resources/model/player/model.obj";
	//playerModelTexPath = L"Resources/model/player/tex.png";

	//effectTexPath = L"Resources/effect1.png";

	bgmFilePath = "Resources/Music/rhythmaze_tutorialpoi.wav";

	//particleSeFilePath = "Resources/SE/Sys_Set03-click.wav";
}

// í«â¡Ç≈èâä˙âªÇ™ïKóvÇ»ÇÁÇ±ÇÃä÷êîÇÃíÜÇ…èëÇ≠
void Stage10::additionalInit() {
	stageNum = 10u;

	musicBpm = 100;

	bgmBolume = 0.4f;

	clearCount = 120u;

	spriteInit();
}

void Stage10::spriteInit() {
	for (size_t i = 0u; i < SPRITES_NUM; i++) {
		sprites[i].create(dxCom->getDev(),
						  WinAPI::window_width, WinAPI::window_height,
						  TEX_NUM::TEX1,
						  spriteCommon,
						  XMFLOAT2(0, 0),
						  false, false);
	}
}
