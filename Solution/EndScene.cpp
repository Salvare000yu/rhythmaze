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

	if (SceneManager::getInstange()->getAchieving()) {
		snprintf(clearinfiStr, _countof(clearinfiStr),
				 "clear count : %u\nclear combo : %u",
				 SceneManager::getInstange()->getClearHalfBeat(),
				 SceneManager::getInstange()->getClearCombo());
	} else {
		snprintf(clearinfiStr, _countof(clearinfiStr),
				 "failed!\ncount : %u\ncombo : %u",
				 SceneManager::getInstange()->getClearHalfBeat(),
				 SceneManager::getInstange()->getClearCombo());
	}
}

void EndScene::update() {
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::SELECT);
	}

	debugText.Print(spCom, "END", 0, 0, 10.f);
	debugText.Print(spCom, "SPACE : back Stage select", 0, WinAPI::window_height / 2);

	debugText.formatPrint(spCom, 0, WinAPI::window_height / 2 + debugText.fontHeight * 2, 1.f,
						  DirectX::XMFLOAT4(1, 1, 1, 1), clearinfiStr);
}

void EndScene::draw() {
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}

void EndScene::fin() {
}
