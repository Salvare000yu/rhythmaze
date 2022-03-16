#pragma once
#include "GameScene.h"

#include "BaseStage.h"

class PlayScene :
	public BaseStage {

	// スプライト共通テクスチャ読み込み
	enum TEX_NUM { TEX1 };

	// --------------------
	// スプライト個別
	// --------------------
	static const int SPRITES_NUM = 1;
	Sprite sprites[SPRITES_NUM]{};

private:
	void pathInit() override;

	void additionalInit() override;

	void spriteInit();

	void additionalDrawSprite() override;

};

