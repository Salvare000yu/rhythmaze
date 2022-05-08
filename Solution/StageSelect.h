#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

#include "Object3d.h"

#include "DirectXCommon.h"

#include "SceneManager.h"

#include <vector>

#include "Time.h"

#include "Sound.h"

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

	enum ARROW_TEX { LEFT_ARROW, RIGHT_ARROW };
	std::vector<Sprite> arrow;
	std::unique_ptr<Sprite> arrowStr;
	std::unique_ptr<Sprite> returnTitle;
	std::unique_ptr<Sprite> space2Select;
	std::vector<Sprite> shiftAndArrow;
	std::unique_ptr<Sprite> back;
	std::unique_ptr<Sprite> cursor;

	DebugText debugText{};
	// デバッグテキスト用のテクスチャ番号を指定
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;

	Input* input = nullptr;

	// 3Dオブジェクト用パイプライン生成
	Object3d::PipelineSet object3dPipelineSet;

	bool sceneChangeFlag = false;
	float sceneChangeScale = 1.f;

	std::unique_ptr<Time> sceneChangeTimer;


	std::unique_ptr<Sound::SoundCommon> soundCom;
	std::unique_ptr<Sound> sceneChangeSe;

public:
	void init() override;
	void update() override;
	void draw() override;
};

