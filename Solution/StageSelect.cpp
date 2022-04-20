#include "StageSelect.h"

#include "SceneManager.h"

#include "WinAPI.h"

using namespace DirectX;

namespace {
	constexpr XMFLOAT4 selectCol = XMFLOAT4(0.827f, 0.345f, 0.443f, 1);
	constexpr XMFLOAT4 unSelectCol = XMFLOAT4(0.367f, 0.685f, 0.427f, 1);
	constexpr float unSelectScale = 0.75f, selectScale = 1.f;

	// todo ステージを追加したらここも増やす
	constexpr UINT stageNum = 7;	// ステージの総数
	constexpr UINT spriteNum = stageNum + 1;	// ステージ数 + 操作説明シーン

	inline XMFLOAT2 operator*(const XMFLOAT2& left, const float right) {
		return XMFLOAT2(left.x * right, left.y * right);
	}
	inline XMFLOAT2 operator*(const float left, const XMFLOAT2 right) {
		return XMFLOAT2(left * right.x, left * right.y);
	}
	inline XMFLOAT2 operator*(const XMFLOAT2 left, const XMFLOAT2 right) {
		return XMFLOAT2(left.x * right.x, left.y * right.y);
	}
}

void StageSelect::init() {
	input = Input::getInstance();

	// --------------------
	// スプライト共通
	// --------------------
	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height);

	constexpr auto stageNumTexNum = 0;
	constexpr auto backSpriteNum = stageNumTexNum + 1;
	constexpr auto leftArrowTexNum = backSpriteNum + 1;
	constexpr auto arrowStrTexNum = leftArrowTexNum + 1;
	constexpr auto returnTitleTexNum = arrowStrTexNum + 1;
	constexpr auto space2SelectTexNum = returnTitleTexNum + 1;
	constexpr auto moveFirstTexNum = space2SelectTexNum + 1;
	constexpr auto moveLastTexNum = moveFirstTexNum + 1;
	constexpr auto backTexNum = moveLastTexNum + 1;
	constexpr auto cursorTexNum = backTexNum + 1;
	// --------------------
	// スプライト共通テクスチャ読み込み
	// --------------------
	// 画像サイズ
	const auto stageGraphSize = Sprite::commonLoadTexture(spCom, stageNumTexNum,
														  L"Resources/StageSelect/stageNum.png",
														  DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, backSpriteNum,
							  L"Resources/StageSelect/select_back.png",
							  DirectXCommon::getInstance()->getDev());

	Sprite::commonLoadTexture(spCom, leftArrowTexNum, L"Resources/arrow/LeftArrow.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, arrowStrTexNum, L"Resources/arrow/Arrow_Str.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, returnTitleTexNum, L"Resources/returnTitle/returnTitle.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, space2SelectTexNum, L"Resources/selectSceneStr/Select.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, moveFirstTexNum, L"Resources/selectSceneStr/moveExplanation.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, moveLastTexNum, L"Resources/selectSceneStr/moveLastStage.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, backTexNum, L"Resources/selectSceneStr/Background.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, cursorTexNum, L"Resources/StageSelect/cursor.png", DirectXCommon::getInstance()->getDev());

	back.reset(new Sprite());
	back->create(DirectXCommon::getInstance()->getDev(),
				 WinAPI::window_width, WinAPI::window_height,
				 backTexNum,
				 spCom, XMFLOAT2(0, 0));

	cursor.reset(new Sprite());
	cursor->create(DirectXCommon::getInstance()->getDev(),
				   WinAPI::window_width, WinAPI::window_height,
				   cursorTexNum, spCom);
	cursor->position = XMFLOAT3(WinAPI::window_width / 2.f,
								WinAPI::window_height / 2.f,
								1.f);
	cursor->SpriteTransferVertexBuffer(spCom);

	const auto numGraphNum = stageGraphSize.x / stageGraphSize.y;	// 正方形を想定
	const auto oneGraphWid = stageGraphSize.x / numGraphNum;		// 数字一つ分の画像横幅

	stage.reserve(spriteNum);
	for (UINT i = 0u; i < spriteNum; ++i) {
		stage.emplace_back();

		stage[i].create(
			DirectXCommon::getInstance()->getDev(),
			WinAPI::window_width, WinAPI::window_height,
			stageNumTexNum, spCom
		);
		stage[i].texLeftTop.x = i * oneGraphWid;
		stage[i].texSize.x = oneGraphWid;

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
	stageBack.reserve(stage.size());
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


	// 矢印
	constexpr UINT arrowNum = 2u;
	arrow.resize(arrowNum);

	for (auto& i : arrow) {
		i.create(DirectXCommon::getInstance()->getDev(),
				 WinAPI::window_width, WinAPI::window_height,
				 leftArrowTexNum, spCom, XMFLOAT2(1.f, 0.5f));
		i.position.x = WinAPI::window_width / 2.f;
		i.position.y = WinAPI::window_height / 3.f;
	}
	arrow[ARROW_TEX::RIGHT_ARROW].isFlipX = true;
	//arrow[ARROW_TEX::RIGHT_ARROW].anchorpoint.x = 0.f;

	arrow[ARROW_TEX::LEFT_ARROW].isInvisible = !(bool)nowSelect;
	arrow[ARROW_TEX::RIGHT_ARROW].isInvisible = nowSelect >= stageNum;

	for (auto& i : arrow) {
		i.SpriteTransferVertexBuffer(spCom);
	}

	arrowStr.reset(new Sprite());
	arrowStr->create(DirectXCommon::getInstance()->getDev(),
					 WinAPI::window_width, WinAPI::window_height,
					 arrowStrTexNum, spCom, XMFLOAT2(0.5f, 0));
	arrowStr->position.x = WinAPI::window_width / 2.f;
	arrowStr->position.y = WinAPI::window_height / 3.f - arrowStr->size.y;

	// 操作の説明文字
	returnTitle.reset(new Sprite());
	returnTitle->create(DirectXCommon::getInstance()->getDev(),
						WinAPI::window_width, WinAPI::window_height,
						returnTitleTexNum, spCom, XMFLOAT2(0.125f, 0));
	returnTitle->position.y = WinAPI::window_height - returnTitle->size.y;

	space2Select.reset(new Sprite());
	space2Select->create(DirectXCommon::getInstance()->getDev(),
						 WinAPI::window_width, WinAPI::window_height,
						 space2SelectTexNum, spCom, XMFLOAT2(0.5f, 0.625));
	space2Select->position.x = WinAPI::window_width / 2.f;
	space2Select->position.y = WinAPI::window_height * 0.66666f;
	space2Select->SpriteTransferVertexBuffer(spCom);


	for (UINT i = 0; i < 2; i++) {
		shiftAndArrow.emplace_back();
		shiftAndArrow[i].create(DirectXCommon::getInstance()->getDev(),
								WinAPI::window_width, WinAPI::window_height,
								moveFirstTexNum + i, spCom, XMFLOAT2(0.5f, 0.625));
		shiftAndArrow[i].position = space2Select->position;
		shiftAndArrow[i].position.y += space2Select->size.y * 0.5f * (i + 1);

		shiftAndArrow[i].isInvisible = arrow[i].isInvisible;

		shiftAndArrow[i].SpriteTransferVertexBuffer(spCom);
	}


	// デバッグテキスト用のテクスチャ読み込み
	Sprite::commonLoadTexture(spCom, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());

	debugText.Initialize(DirectXCommon::getInstance()->getDev(),
						 WinAPI::window_width, WinAPI::window_height,
						 debugTextTexNumber, spCom);



	// 3Dオブジェクト用パイプライン生成
	object3dPipelineSet = Object3d::createGraphicsPipeline(DirectXCommon::getInstance()->getDev());
}

void StageSelect::update() {
	if (input->triggerKey(DIK_R)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::TITLE);
	}

	constexpr auto right = DIK_RIGHT;
	constexpr auto left = DIK_LEFT;

	const auto inputR = input->triggerKey(right);
	const auto inputL = input->triggerKey(left);
	const auto inputLShift = input->hitKey(DIK_LSHIFT);

	if (inputR || inputL || inputLShift) {

		bool changeFlag = false;

		if (inputLShift) {
			if (inputL) {
				nowSelect = 0;
				changeFlag = true;
			} else if (inputR) {
				nowSelect = stageNum;
				changeFlag = true;
			}
		} else if (nowSelect > 0 && inputL) {
			nowSelect--;
			changeFlag = true;
		} else if (nowSelect < stage.size() - 1 && inputR) {
			nowSelect++;
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

				arrow[ARROW_TEX::LEFT_ARROW].isInvisible = !(bool)nowSelect;
				arrow[ARROW_TEX::RIGHT_ARROW].isInvisible = nowSelect >= stageNum;

				shiftAndArrow[0].isInvisible = arrow[ARROW_TEX::LEFT_ARROW].isInvisible;
				shiftAndArrow[1].isInvisible = arrow[ARROW_TEX::RIGHT_ARROW].isInvisible;
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
}

void StageSelect::draw() {
	// スプライト共通コマンド
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	// 背景
	back->drawWithUpdate(DirectXCommon::getInstance(), spCom);
	// スプライト描画
	cursor->drawWithUpdate(DirectXCommon::getInstance(), spCom);
	for (UINT i = 0, size = stage.size(); i < size; ++i) {
		stageBack[i].drawWithUpdate(DirectXCommon::getInstance(), spCom);
		stage[i].drawWithUpdate(DirectXCommon::getInstance(), spCom);
	}
	arrowStr->drawWithUpdate(DirectXCommon::getInstance(), spCom);
	for (auto& i : arrow) {
		i.drawWithUpdate(DirectXCommon::getInstance(), spCom);
	}
	returnTitle->drawWithUpdate(DirectXCommon::getInstance(), spCom);
	space2Select->drawWithUpdate(DirectXCommon::getInstance(), spCom);
	for (auto& i : shiftAndArrow) {
		i.drawWithUpdate(DirectXCommon::getInstance(), spCom);
	}
	// デバッグテキスト描画
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}
