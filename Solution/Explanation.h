#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

class Explanation:
	public GameScene {

	// --------------------
	// デバッグテキスト
	// --------------------
	Sprite::SpriteCommon spCom;
	Sprite* sprite;
	DebugText debugText{};
	// デバッグテキスト用のテクスチャ番号を指定
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;

	Input* input = nullptr;

public:
	void init() override;
	void update() override;
	void draw() override;
};

