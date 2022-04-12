#include "Stage7.h"

#include "SceneManager.h"

#include <sstream>
#include <iomanip>

#include <xaudio2.h>
#include "RandomNum.h"

#include <fstream>
#include <string>
#include <vector>

using namespace DirectX;

void Stage7::pathInit() {
	mapCSVFilePath = "Resources/map/map_Stage7.csv";

	//boxModelPath = L"Resources/model/box/box.obj";
	//boxModelTexPath = L"Resources/model/box/box.png";

	//playerModelPath = L"Resources/model/player/model.obj";
	//playerModelTexPath = L"Resources/model/player/tex.png";

	//effectTexPath = L"Resources/effect1.png";

	bgmFilePath = "Resources/Music/mmc_125_BGM2.wav";

	//particleSeFilePath = "Resources/SE/Sys_Set03-click.wav";
}

// �ǉ��ŏ��������K�v�Ȃ炱�̊֐��̒��ɏ���
void Stage7::additionalInit() {
	musicBpm = 125.f;

	bgmBolume = 0.4f;

	clearCount = 250u;

	spriteInit();
}

void Stage7::spriteInit() {
	for (size_t i = 0u; i < SPRITES_NUM; i++) {
		sprites[i].create(dxCom->getDev(),
						  WinAPI::window_width, WinAPI::window_height,
						  TEX_NUM::TEX1,
						  spriteCommon,
						  XMFLOAT2(0, 0),
						  false, false);
	}
}

void Stage7::additionalDrawSprite() {
	/*for (auto& i: sprites) {
		i.drawWithUpdate(dxCom, spriteCommon);
	}*/
}
