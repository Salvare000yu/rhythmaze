#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

#include "Object3d.h"

#include "DirectXCommon.h"

class StageSelect :
	public GameScene {

	// --------------------
	// �f�o�b�O�e�L�X�g
	// --------------------
	Sprite::SpriteCommon spCom;
	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	enum TEX_NUM { EX, P1, P2, P3, BACK };
	enum class STAGE_SELECT : USHORT { EXPLANATION, STAGE1, STAGE2, STAGE3 };
	STAGE_SELECT SELECT = STAGE_SELECT::EXPLANATION;

	// --------------------
	// �X�v���C�g��
	// --------------------
	static const int SPRITES_NUM = 4;
	Sprite sprites[SPRITES_NUM]{};
	Sprite selectBack[SPRITES_NUM]{};

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

