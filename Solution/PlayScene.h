#pragma once
#include "GameScene.h"

#include "BaseStage.h"

class PlayScene :
	public BaseStage {

	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	enum TEX_NUM { TEX1 };

	// --------------------
	// �X�v���C�g��
	// --------------------
	static const int SPRITES_NUM = 1;
	Sprite sprites[SPRITES_NUM]{};

private:
	void pathInit() override;

	void additionalInit() override;

	void spriteInit();

	void additionalDrawSprite() override;

};

