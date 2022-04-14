#include "StageSelect.h"

#include "SceneManager.h"

#include "WinAPI.h"

using namespace DirectX;

namespace {
	constexpr XMFLOAT4 selectCol = XMFLOAT4(0.75f, 0, 0.75f, 1);
	constexpr XMFLOAT4 unSelectCol = XMFLOAT4(0, 0.5f, 0.5f, 1);
	constexpr float unSelectScale = 0.75f, selectScale = 1.f;

	// todo ステージを追加したらここも増やす
	constexpr UINT stageNum = 7;	// ステージの総数
	constexpr UINT spriteNum = stageNum + 1;	// ステージ数 + 操作説明シーン

	XMFLOAT2 operator*(const XMFLOAT2& left, const float right) {
		return XMFLOAT2(left.x * right, left.y * right);
	}
	XMFLOAT2 operator*(const float left, const XMFLOAT2 right) {
		return XMFLOAT2(left * right.x, left * right.y);
	}
	XMFLOAT2 operator*(const XMFLOAT2 left, const XMFLOAT2 right) {
		return XMFLOAT2(left.x * right.x, left.y * right.y);
	}
}

void StageSelect::init() {
	input = Input::getInstance();

	// --------------------
	// スプライト共通
	// --------------------
	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height);

	const auto backSpriteNum = debugTextTexNumber - 1;
	// スプライト共通テクスチャ読み込み
	Sprite::commonLoadTexture(spCom, 0, L"Resources/StageSelect/select_Explanation.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, 1, L"Resources/StageSelect/Stage1.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, 2, L"Resources/StageSelect/Stage2.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, 3, L"Resources/StageSelect/Stage3.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, 4, L"Resources/StageSelect/Stage4.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, 5, L"Resources/StageSelect/Stage5.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, 6, L"Resources/StageSelect/Stage6.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, 7, L"Resources/StageSelect/Stage7.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, backSpriteNum, L"Resources/StageSelect/select_back.png", DirectXCommon::getInstance()->getDev());

	for (UINT i = 0u; i < spriteNum; ++i) {
		stage.emplace_back();

		stage[i].create(
			DirectXCommon::getInstance()->getDev(),
			WinAPI::window_width, WinAPI::window_height,
			i, spCom
		);
		const auto grWid = stage[0].size.x * 1.5f;
		stage[i].position.x = grWid * i + WinAPI::window_width / 2.f;
		stage[i].position.y = WinAPI::window_height / 2.f;

		stage[i].size = unSelectScale * stage[0].texSize;
		stage[i].color = unSelectCol;

		stage[i].SpriteTransferVertexBuffer(spCom);
	}
	stage[0].color = selectCol;
	stage[0].size = selectScale * stage[0].texSize;
	stage[0].SpriteTransferVertexBuffer(spCom);


	// 各ステージ画像の背景
	for (UINT i = 0u, size = stage.size(); i < size; i++) {
		stageBack.emplace_back();

		stageBack[i].create(
			DirectXCommon::getInstance()->getDev(),
			WinAPI::window_width, WinAPI::window_height,
			backSpriteNum, spCom
		);
		stageBack[i].position = stage[i].position;
		stageBack[i].size = stage[i].size;
		// 頂点バッファに反映
		stageBack[i].SpriteTransferVertexBuffer(spCom);
	}

	// 3Dオブジェクト用パイプライン生成
	object3dPipelineSet = Object3d::createGraphicsPipeline(DirectXCommon::getInstance()->getDev());


	/*spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height);*/

		// デバッグテキスト用のテクスチャ読み込み
	Sprite::commonLoadTexture(spCom, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());

	debugText.Initialize(DirectXCommon::getInstance()->getDev(),
						 WinAPI::window_width, WinAPI::window_height,
						 debugTextTexNumber, spCom);
}

void StageSelect::update() {
	if (input->triggerKey(DIK_R)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::TITLE);
	}

	constexpr auto up = DIK_UP;
	constexpr auto down = DIK_DOWN;
	constexpr auto right = DIK_RIGHT;
	constexpr auto left = DIK_LEFT;

	if (input->triggerKey(down) || input->triggerKey(up)
		|| input->triggerKey(right) || input->triggerKey(left)
		|| input->triggerKey(DIK_HOME) || input->triggerKey(DIK_END)) {

		bool changeFlag = false;

		if (nowSelect > 0 && input->triggerKey(left)) {
			nowSelect--;
			changeFlag = true;
		} else if (nowSelect < stage.size() - 1 && input->triggerKey(right)) {
			nowSelect++;
			changeFlag = true;
		} else if (input->triggerKey(DIK_HOME)) {
			nowSelect = 0;
			changeFlag = true;
		} else if (input->triggerKey(DIK_END)) {
			nowSelect = stageNum;
			changeFlag = true;
		}

		if (changeFlag) {
			for (UINT i = 0, size = stage.size(); i < size; ++i) {
				const auto grWid = stage[0].size.x * 1.5f;
				stage[i].position.x = grWid * i - (nowSelect * grWid) + WinAPI::window_width / 2.f;
				stageBack[i].position.x = stage[i].position.x;

				stageBack[i].size = unSelectScale * stage[0].texSize;

				stage[i].size = unSelectScale * stage[0].texSize;
				stage[i].color = unSelectCol;

				stage[i].SpriteTransferVertexBuffer(spCom);
				stageBack[i].SpriteTransferVertexBuffer(spCom);
			}

			stage[nowSelect].size = selectScale * stage[nowSelect].texSize;
			stage[nowSelect].color = selectCol;
			stage[nowSelect].SpriteTransferVertexBuffer(spCom);

			stageBack[nowSelect].size = selectScale * stageBack[nowSelect].texSize;
			stageBack[nowSelect].SpriteTransferVertexBuffer(spCom);

			switch (nowSelect) {
			case 0:
				SELECT = SCENE_NUM::EXPLANATION;
				break;
			case 1:
				SELECT = SCENE_NUM::STAGE1;
				break;
			case 2:
				SELECT = SCENE_NUM::STAGE2;
				break;
			case 3:
				SELECT = SCENE_NUM::STAGE3;
				break;
			case 4:
				SELECT = SCENE_NUM::STAGE4;
				break;
			case 5:
				SELECT = SCENE_NUM::STAGE5;
				break;
			case 6:
				SELECT = SCENE_NUM::STAGE6;
				break;
			case 7:
				SELECT = SCENE_NUM::STAGE7;
				break;
			default:
				SELECT = SCENE_NUM::EXPLANATION;
				break;
			}
		}
	} else {
		// スペースを押したら選んだシーンに移動
		if (input->triggerKey(DIK_SPACE)) {
			SceneManager::getInstange()->changeScene(SELECT);
		}
	}

	debugText.Print(spCom, "R : BACK_TITLE", debugText.fontWidth * 1.5f, WinAPI::window_height - debugText.fontHeight * 1.5f);


	debugText.Print(spCom, "arrow key", WinAPI::window_width / 2 - debugText.fontWidth * 4.5f, WinAPI::window_height / 3 - debugText.fontHeight);
	if (nowSelect > 0) debugText.Print(spCom, "<-", WinAPI::window_width / 2 - debugText.fontWidth * 2.5f, WinAPI::window_height / 3);
	if (nowSelect < stageNum) debugText.Print(spCom, "->", WinAPI::window_width / 2 + debugText.fontWidth * 0.5, WinAPI::window_height / 3);

	debugText.Print(spCom,
					"SPACE : SELECT\n\n HOME : MOVE_Setsumei\n  END : MOVE_LAST_STAGE",
					WinAPI::window_width / 2 - debugText.fontWidth * 6,
					WinAPI::window_height / 3.f * 2 - debugText.fontHeight);
}

void StageSelect::draw() {
	// スプライト共通コマンド
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	// スプライト描画
	for (UINT i = 0, size = stage.size(); i < size; ++i) {
		stageBack[i].drawWithUpdate(DirectXCommon::getInstance(), spCom);
		stage[i].drawWithUpdate(DirectXCommon::getInstance(), spCom);
	}
	// デバッグテキスト描画
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}
