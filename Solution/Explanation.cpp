#include "Explanation.h"

#include "SceneManager.h"

#include "WinAPI.h"

void Explanation::init(){
	input = Input::getInstance();

	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height);

	// デバッグテキスト用のテクスチャ読み込み
	Sprite::commonLoadTexture(spCom, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());

	debugText.Initialize(DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		debugTextTexNumber, spCom);
}

void Explanation::update()
{
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::SELECT);
	}
	debugText.Print(spCom, "EXPLANATION", 0, 0, 5.f);
	debugText.Print(spCom, "SPACE : stage select", 0, WinAPI::window_height / 2);
}

void Explanation::draw()
{
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}
