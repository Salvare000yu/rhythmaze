#include "TitleScene.h"

#include "SceneManager.h"

#include "WinAPI.h"

void TitleScene::init() {
	input = Input::getInstance();

	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
									   WinAPI::window_width, WinAPI::window_height);

	constexpr UINT titleSpriteNum = 0;
	Sprite::commonLoadTexture(spCom, 0, L"Resources/backSprite/title.png", DirectXCommon::getInstance()->getDev());

	titleSprite.reset(new Sprite());
	titleSprite->create(DirectXCommon::getInstance()->getDev(),
						WinAPI::window_width, WinAPI::window_height,
						titleSpriteNum, spCom, DirectX::XMFLOAT2(0, 0), false, false);

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
	debugText.Print(spCom, "Press SPACE...",
					WinAPI::window_width * 0.5f - debugText.fontWidth * 7,
					WinAPI::window_height * 0.75);
}

void TitleScene::draw() {
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	titleSprite->drawWithUpdate(DirectXCommon::getInstance(), spCom);
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}
