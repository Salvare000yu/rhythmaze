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

	const bool cleared = SceneManager::getInstange()->getAchieving();

	std::wstring backSpritePath;

	if (cleared) {
		snprintf(clearinfiStr, _countof(clearinfiStr),
				 "clear combo : %u\nclear count : %u",
				 SceneManager::getInstange()->getClearCombo(),
				 SceneManager::getInstange()->getClearHalfBeat());
		backSpritePath = L"Resources/backSprite/clear.png";
	} else {
		snprintf(clearinfiStr, _countof(clearinfiStr),
				 "failed!\ncount : %u\ncombo : %u",
				 SceneManager::getInstange()->getClearHalfBeat(),
				 SceneManager::getInstange()->getClearCombo());
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
		SceneManager::getInstange()->changeScene(SCENE_NUM::SELECT);
	}

	debugText.Print(spCom, "SPACE : back Stage select", 0, WinAPI::window_height / 2.f);

	debugText.Print(spCom, clearinfiStr, 0, WinAPI::window_height / 2.f + debugText.fontHeight * 2);
}

void EndScene::draw() {
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	backSprite->drawWithUpdate(DirectXCommon::getInstance(), spCom);
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}

void EndScene::fin() {
}
