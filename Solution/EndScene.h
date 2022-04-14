#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

#include <memory>

#include "Sound.h"

class EndScene :
    public GameScene {

	// --------------------
	// �f�o�b�O�e�L�X�g
	// --------------------
	Sprite::SpriteCommon spCom;
	DebugText debugText{};
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;

	std::unique_ptr<Sprite> backSprite;

	Input* input = nullptr;

	char clearinfiStr[64]{};

	std::unique_ptr<Sound::SoundCommon> soundCom;
	std::unique_ptr<Sound> bgm;

public:
    void init() override;
    void update() override;
    void draw() override;
    void fin() override;
};

