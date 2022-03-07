#include "StageSelect.h"

#include "SceneManager.h"

#include "WinAPI.h"

void StageSelect::init(){
	input = Input::getInstance();

	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height);

	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	Sprite::commonLoadTexture(spCom, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());

	debugText.Initialize(DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		debugTextTexNumber, spCom);
}

void StageSelect::update(){
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::EXPLANATION);
	}

	if (input->triggerKey(DIK_RETURN)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::PLAY);
	}
	debugText.Print(spCom, "SELECT", 0, 0, 10.f);
	debugText.Print(spCom, "ENTER : PLAY\nSPACE : EXPLANATION", 0, WinAPI::window_height / 2);
}

void StageSelect::draw(){
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}
