#pragma once
#include "GameScene.h"

#include <DirectXMath.h>

#include <memory>

#include "Sound.h"

#include "DebugText.h"

#include "Model.h"
#include "Object3d.h"

#include "Time.h"

#include "Input.h"

#include "Camera.h"

#include "ParticleManager.h"

enum class MAP_NUM : unsigned short {
	UNDEF,		// ����`
	WALL,		// ��
	FRONT_ROAD,	// �\���̓�
	BACK_ROAD,	// �����̓�
	GOAL
};

class BaseStage
	: public GameScene {

protected:

#pragma region �r���[�ϊ��s��

	//DirectX::XMMATRIX matView;
	DirectX::XMFLOAT3 eye_local;   // ���_���W
	DirectX::XMFLOAT3 target_local;   // �����_���W
	DirectX::XMFLOAT3 up_local;       // ������x�N�g��

#pragma endregion �r���[�ϊ��s��

#pragma region ��

	std::unique_ptr<Sound::SoundCommon> soundCommon;

	std::unique_ptr<Sound> bgm;

	std::unique_ptr<Sound> particleSE;

#pragma endregion ��

#pragma region �X�v���C�g
	SpriteCommon spriteCommon;
#pragma endregion �X�v���C�g

#pragma region �f�o�b�O�e�L�X�g

	DebugText debugText{};
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;
#pragma endregion �f�o�b�O�e�L�X�g

#pragma region 3D�I�u�W�F�N�g

	// 3D�I�u�W�F�N�g�p�p�C�v���C������
	Object3d::PipelineSet object3dPipelineSet;

	const UINT obj3dTexNum = 0U;
	std::unique_ptr<Model> model;
	const float obj3dScale = 10.f;

	const float mapSide = obj3dScale * 2;

	DirectX::XMFLOAT2 angle{};	// �e������̉�]�p

	std::vector<std::vector<Object3d>> mapObj;

	std::unique_ptr<Model> playerModel;
	std::unique_ptr<Object3d> playerObj;

	DirectX::XMFLOAT2 playerMapPos;

#pragma endregion 3D�I�u�W�F�N�g

#pragma region �ړ��֘A���

	uint16_t beatChangeNum = 0;

	Time::timeType beatChangeTime = 0;

	UINT combo = 0U;
	bool movableFlag = true;

	bool playerMoved = false;
	bool missFlag = false;

	bool frontBeatFlag = true;

#pragma endregion �ړ��֘A���

#pragma region ���C�g
	DirectX::XMFLOAT3 light{};
#pragma endregion ���C�g

#pragma region ����

	std::unique_ptr<Time> timer;

#pragma endregion ����

#pragma region �J����

	std::unique_ptr<Camera> camera;

#pragma endregion �J����

#pragma region �p�[�e�B�N��

	std::unique_ptr<ParticleManager> particleMgr;

	bool createParticleFlag = false;

#pragma endregion �p�[�e�B�N��

#pragma region �V���O���g���|�C���^�i�[
	Input* input = nullptr;

	DirectXCommon* dxCom = nullptr;
#pragma endregion �V���O���g���|�C���^�i�[

#pragma region �}�b�v

	// �t�@�C������ǂݍ��񂾏����i�[����
	std::vector<std::vector<MAP_NUM>> mapData;

#pragma endregion �}�b�v

#pragma region �N���A����
	unsigned clearCount = 250u;
	unsigned clearCombo = 1u;
#pragma endregion �N���A����

#pragma region �t�@�C���p�X

	std::string mapCSVFilePath = "Resources/map/map_stage1.csv";

	std::wstring boxModelPath = L"Resources/model/box/box.obj";
	std::wstring boxModelTexPath = L"Resources/model/box/box.png";

	std::wstring playerModelPath = L"Resources/model/player/model.obj";
	std::wstring playerModelTexPath = L"Resources/model/player/tex.png";

	std::wstring effectTexPath = L"Resources/effect1.png";

	std::string bgmFilePath = "Resources/Music/mmc_125_BGM2.wav";

	std::string particleSeFilePath = "Resources/SE/Sys_Set03-click.wav";

#pragma endregion �t�@�C���p�X

	float musicBpm = 125.f;

protected:
	void updateLightPosition();

	void updateCamera();

	bool goal();
	void timeOut();

	void updatePlayerPos();

	void updateTime();

	void createParticle(const DirectX::XMFLOAT3 pos,
						const UINT particleNum = 10U, const float startScale = 1.f);
	void startParticle();

	void drawObj3d();
	void drawParticle();
	void drawSprite();

	virtual void additionalDrawSprite() {};
	virtual void additionalDrawObj3d() {};
	virtual void additionalDrawParticle() {};

	virtual void additionalUpdate() {};

public:
	virtual void pathInit() {};
	virtual void additionalInit() {};

	void init() override;
	void update() override;
	void draw() override;

	// @return �ǂݍ���csv�̒��g�B���s������f�t�H���g�R���X�g���N�^�ŏ��������ꂽ���vector2�����z�񂪕Ԃ�
	static std::vector<std::vector<std::string>> loadCsv(const std::string& csvFilePath);

	void loadMapFile(const std::string& csvFilePath);

};

