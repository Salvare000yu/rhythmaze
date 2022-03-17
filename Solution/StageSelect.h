#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

#include "Object3d.h"

#include "DirectXCommon.h"

class StageSelect :
	public GameScene {

	// --------------------
	// デバッグテキスト
	// --------------------
	Sprite::SpriteCommon spCom;
	// スプライト共通テクスチャ読み込み
	enum TEX_NUM { EX, P1, P2, P3, BACK };
	enum class STAGE_SELECT : USHORT { EXPLANATION, STAGE1, STAGE2, STAGE3 };
	STAGE_SELECT SELECT = STAGE_SELECT::EXPLANATION;

	// --------------------
	// スプライト個別
	// --------------------
	static const int SPRITES_NUM = 4;
	Sprite sprites[SPRITES_NUM]{};
	Sprite selectBack[SPRITES_NUM]{};

	DebugText debugText{};
	// デバッグテキスト用のテクスチャ番号を指定
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;

	Input* input = nullptr;

	// 3Dオブジェクト用パイプライン生成
	Object3d::PipelineSet object3dPipelineSet;

public:
	void init() override;
	void update() override;
	void draw() override;
};

