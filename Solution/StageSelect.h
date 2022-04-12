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
	// �f�o�b�O�e�L�X�g
	// --------------------
	Sprite::SpriteCommon spCom;
	SCENE_NUM SELECT = SCENE_NUM::EXPLANATION;

	// --------------------
	// �X�v���C�g��
	// --------------------
	static const int SPRITES_NUM = 4;
	std::vector<Sprite> stage;
	std::vector<Sprite> stageBack;
	UINT nowSelect = 0;

	DebugText debugText{};
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;

	Input* input = nullptr;

	// 3D�I�u�W�F�N�g�p�p�C�v���C������
	Object3d::PipelineSet object3dPipelineSet;

public:
	void init() override;
	void update() override;
	void draw() override;
};

