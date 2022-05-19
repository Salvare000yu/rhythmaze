#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

#include "Sound.h"

#include <memory>

#include "Time.h"

class TitleScene :
	public GameScene {

	// --------------------
	// デバッグテキスト
	// --------------------
	Sprite::SpriteCommon spCom;
	DebugText debugText{};
	// デバッグテキスト用のテクスチャ番号を指定
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;

	std::unique_ptr<Sprite> titleSprite;
	std::unique_ptr<Sprite> titleBackSprite;
	std::unique_ptr<Sprite> pressSpace;

	Input* input = nullptr;

	std::unique_ptr<Sound::SoundCommon> soundCom;
	std::unique_ptr<Sound> bgm;

	std::unique_ptr<Sound> sceneChangeSe;

	bool startEaseFlag = true;
	std::unique_ptr<Time> easeTimer;

	bool endEaseFlag = false;

public:
	void init() override;
	void update() override;
	void draw() override;
};