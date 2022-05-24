#include "EndScene.h"

#include "SceneManager.h"

#include "WinAPI.h"

void EndScene::init() {
	input = Input::getInstance();

	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
									   WinAPI::window_width, WinAPI::window_height);



	// デバッグテキスト用のテクスチャ読み込み
	Sprite::commonLoadTexture(spCom, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());

	debugText.Initialize(DirectXCommon::getInstance()->getDev(),
						 WinAPI::window_width, WinAPI::window_height,
						 debugTextTexNumber, spCom);

	const bool cleared = SceneManager::getInstance()->getAchieving();

	std::wstring backSpritePath;

	snprintf(clearinfiStr, _countof(clearinfiStr),
			 "stage %u\ncombo:%u\ncount:%u",
			 SceneManager::getInstance()->getStageNum(),
			 SceneManager::getInstance()->getClearCombo(),
			 SceneManager::getInstance()->getClearHalfBeat());
	if (cleared) {
		backSpritePath = L"Resources/backSprite/clear.png";
	} else {
		backSpritePath = L"Resources/backSprite/failed.png";
	}


	// 背景スプライト
	Sprite::commonLoadTexture(spCom, 0, backSpritePath.c_str(), DirectXCommon::getInstance()->getDev());
	backSprite.reset(new Sprite());
	backSprite->create(DirectXCommon::getInstance()->getDev(),
					   WinAPI::window_width, WinAPI::window_height,
					   0,
					   spCom,
					   { 0, 0 });

	// 音
	soundCom.reset(new Sound::SoundCommon());
	constexpr char titleBgmPath[] = "Resources/Music/BGM.wav";
	bgm.reset(new Sound(titleBgmPath, soundCom.get()));

	if (!cleared) Sound::SoundPlayWave(soundCom.get(), bgm.get(), XAUDIO2_LOOP_INFINITE);
}

void EndScene::update() {
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstance()->changeScene(SCENE_NUM::SELECT);
	}

	debugText.Print(spCom, clearinfiStr,
					WinAPI::window_width * 0.375f + 1, WinAPI::window_height * 0.41f + 1, 1.f,
					{ 0, 0, 0, 1 });
	debugText.Print(spCom, clearinfiStr,
					WinAPI::window_width * 0.375f, WinAPI::window_height * 0.41f, 1.f,
					{ 1, 1, 1, 1 });
}

void EndScene::draw() {
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	backSprite->drawWithUpdate(DirectXCommon::getInstance(), spCom);
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}

void EndScene::fin() {
}
