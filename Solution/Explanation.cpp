#include "Explanation.h"

#include "SceneManager.h"

#include "WinAPI.h"

namespace {
	constexpr UINT spriteNum = 4;
}

void Explanation::init() {
	input = Input::getInstance();

	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height);

	// デバッグテキスト用のテクスチャ読み込み
	Sprite::commonLoadTexture(spCom, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, 1, L"Resources/explation2/1_Explanation_arrange.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, 2, L"Resources/explation2/2_Explanation_arrange.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, 3, L"Resources/explation2/3_Explanation_arrange.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, 4, L"Resources/explation2/4_Explanation_arrange.png", DirectXCommon::getInstance()->getDev());

	debugText.Initialize(DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		debugTextTexNumber, spCom);

	sprite = new Sprite();
	sprite->create(DirectXCommon::getInstance()->getDev(), WinAPI::window_height, WinAPI::window_height,
				   1, spCom, DirectX::XMFLOAT2(0, 0));
}

void Explanation::update() {
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstance()->changeScene(SCENE_NUM::SELECT);
	}
	/*debugText.Print(spCom, "EXPLANATION", 0, 0, 5.f);
	debugText.Print(spCom,
					"[SPACE : back stage select]\n\n[operation]\narrow(rhythmically) : move",
					0, WinAPI::window_height / 2);*/

	if (input->triggerKey(DIK_RIGHT)) {
		if (sprite->texNumber < spriteNum) {
			sprite->texNumber++;
		}
	} else if (input->triggerKey(DIK_LEFT)) {
		if (sprite->texNumber > 1) {
			sprite->texNumber--;
		}
	}
}

void Explanation::draw() {
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	sprite->drawWithUpdate(DirectXCommon::getInstance(), spCom);
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}
