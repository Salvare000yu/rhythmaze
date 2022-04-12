#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

#include "Object3d.h"

#include "DirectXCommon.h"

#include "SceneManager.h"

#include <vector>

class StageSelect :
	public GameScene {

	// --------------------
	// デバッグテキスト
	// --------------------
	Sprite::SpriteCommon spCom;
	SCENE_NUM SELECT = SCENE_NUM::EXPLANATION;

	// --------------------
	// スプライト個別
	// --------------------
	static const int SPRITES_NUM = 4;
	std::vector<Sprite> stage;
	std::vector<Sprite> stageBack;
	UINT nowSelect = 0;

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

