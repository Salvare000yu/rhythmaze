#include "Stage2.h"

#include "SceneManager.h"

#include <sstream>
#include <iomanip>

#include <xaudio2.h>
#include "RandomNum.h"

#include <fstream>
#include <string>
#include <vector>

using namespace DirectX;

void Stage2::pathInit() {
	mapCSVFilePath = "Resources/map/map_stage2.csv";

	//boxModelPath = L"Resources/model/box/box.obj";
	//boxModelTexPath = L"Resources/model/box/box.png";

	//playerModelPath = L"Resources/model/player/model.obj";
	//playerModelTexPath = L"Resources/model/player/tex.png";

	//effectTexPath = L"Resources/effect1.png";

	bgmFilePath = "Resources/Music/mmc_138_BGM3.wav";

	//particleSeFilePath = "Resources/SE/Sys_Set03-click.wav";
}

// í«â¡Ç≈èâä˙âªÇ™ïKóvÇ»ÇÁÇ±ÇÃä÷êîÇÃíÜÇ…èëÇ≠
void Stage2::additionalInit() {
	musicBpm = 138.f;
	bgmBolume = 0.4f;

	clearCombo = 1u;
	clearCount = 250u;

	spriteInit();
}

void Stage2::spriteInit() {
	for (size_t i = 0u; i < SPRITES_NUM; i++) {
		sprites[i].create(dxCom->getDev(),
						  WinAPI::window_width, WinAPI::window_height,
						  TEX_NUM::TEX1,
						  spriteCommon,
						  XMFLOAT2(0, 0),
						  false, false);
	}
}

void Stage2::additionalDrawSprite() {
	/*for (auto& i: sprites) {
		i.drawWithUpdate(dxCom, spriteCommon);
	}*/
}
