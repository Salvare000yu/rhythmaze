#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

#include "Sound.h"

#include <memory>

class TitleScene :
	public GameScene {

	// --------------------
	// デバッグテキスト
	// --------------------
	Sprite::SpriteCommon spCom;
	DebugText debugText{};
	// デバッグテキスト用のテクスチャ番号を指定
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;

	Input* input = nullptr;

	std::unique_ptr<Sound::SoundCommon> soundCom;
	std::unique_ptr<Sound> bgm;

public:
	void init() override;
	void update() override;
	void draw() override;
};