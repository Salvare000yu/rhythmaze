#include "TitleScene.h"

#include "SceneManager.h"

#include "WinAPI.h"

void TitleScene::init() {
	input = Input::getInstance();

	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
									   WinAPI::window_width, WinAPI::window_height);

	// デバッグテキスト用のテクスチャ読み込み
	Sprite::commonLoadTexture(spCom, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());

	debugText.Initialize(DirectXCommon::getInstance()->getDev(),
						 WinAPI::window_width, WinAPI::window_height,
						debugTextTexNumber, spCom);


	// 音
	soundCom.reset(new Sound::SoundCommon());
	constexpr char titleBgmPath[] = "Resources/Music/BGM.wav";
	bgm.reset(new Sound(titleBgmPath, soundCom.get()));

	Sound::SoundPlayWave(soundCom.get(), bgm.get(), XAUDIO2_LOOP_INFINITE);
}

void TitleScene::update() {
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::SELECT);
	}
	debugText.Print(spCom, "RHYTHMAZE", 0, 0, 7.5f);
	debugText.Print(spCom, "SPACE : stage select", 0, WinAPI::window_height / 2);
}

void TitleScene::draw() {
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}
