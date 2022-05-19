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

	float bgmBolume = 0.2f;

#pragma endregion ��

#pragma region �X�v���C�g
	SpriteCommon spriteCommon;

	std::unique_ptr <Sprite> circleSprite;
	std::unique_ptr <Sprite> timeBarSprite;
	static const UINT timeBarWid = WinAPI::window_width * 0.75f;

	std::unique_ptr<Sprite> red;

	std::unique_ptr<Sprite> vign;

#pragma endregion �X�v���C�g

#pragma region �f�o�b�O�e�L�X�g

	DebugText debugText{};
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;
#pragma endregion �f�o�b�O�e�L�X�g

#pragma region 3D�I�u�W�F�N�g

	// 3D�I�u�W�F�N�g�p�p�C�v���C������
	Object3d::PipelineSet object3dPipelineSet;
	Object3d::PipelineSet backPipelineSet;

	const UINT obj3dTexNum = 0U;
	std::unique_ptr<Model> model;
	const float obj3dScale = 10.f;

	const float mapSide = obj3dScale * 2;

	DirectX::XMFLOAT2 angle{};	// �e������̉�]�p

	std::vector<std::vector<Object3d>> mapObj;

	std::unique_ptr<Model> playerModel;
	std::unique_ptr<Object3d> playerObj;
	float playerScale;

	DirectX::XMFLOAT2 playerMapPos;

	std::unique_ptr<Model> goalModel;
	std::vector<Object3d> goalObj;


	std::unique_ptr<Model> backModel;
	std::unique_ptr<Object3d> backObj;

#pragma endregion 3D�I�u�W�F�N�g

#pragma region �ړ��֘A���

	unsigned beatChangeNum = 0u;
	unsigned missNum = 0u;

	Time::timeType beatChangeTime = 0;

	UINT combo = 0U;

	bool playerMoved = false;
	bool missFlag = false;

	bool frontBeatFlag = true;

#pragma endregion �ړ��֘A���

#pragma region ���C�g
	DirectX::XMFLOAT3 light{};
#pragma endregion ���C�g

#pragma region ����

	std::unique_ptr<Time> timer;

	std::unique_ptr<Time> redTimer;

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
	float floorPosY;

#pragma endregion �}�b�v

#pragma region �N���A����
	unsigned clearCount = 250u;
#pragma endregion �N���A����

#pragma region �C�[�W���O

	DirectX::XMFLOAT3 easeStartPos{};
	DirectX::XMFLOAT3 easeEndPos{};

	std::unique_ptr<Time> easeTime{};

	bool playerEasing = false;

#pragma endregion �C�[�W���O

#pragma region �t�@�C���p�X

	std::string mapCSVFilePath = "Resources/map/map_stage1.csv";

	enum BOX_TEXNUM
		: unsigned short {
		WALL,
		FRONT,
		BACK,
		GOAL
	};

	std::wstring boxModelPath = L"Resources/model/box/box.obj";
	std::wstring boxModelTexPath_wall = L"Resources/model/box/box.png";
	std::wstring boxModelTexPath_front = L"Resources/model/box/haimidoriBlock.png";
	std::wstring boxModelTexPath_back = L"Resources/model/box/tyrianPurpleBlock.png";
	std::wstring boxModelTexPath_goal = L"Resources/model/box/box.png";

	std::wstring playerModelPath = L"Resources/model/player/note.obj";
	std::wstring playerModelTexPath = L"Resources/model/player/nanohanairo.png";

	std::wstring effectTexPath = L"Resources/effect1.png";

	std::string bgmFilePath = "Resources/Music/mmc_125_BGM2.wav";

	std::string particleSeFilePath = "Resources/SE/Sys_Set03-click.wav";

	std::wstring backModelPath = L"Resources/model/back/back.obj";
	std::wstring backModelTexPath = L"Resources/model/back/back_tex.png";

	std::wstring goalModelPath = L"Resources/model/goal/goal.obj";
	std::wstring goalModelTexPath = L"Resources/model/goal/lightBlue.png";

#pragma endregion �t�@�C���p�X

	UINT stageNum = 0;
	float musicBpm = 125.f;

	UINT redTime = 0;

protected:
	void updateLightPosition();

	void updateCamera();

	bool goal();
	void timeOut();

	void updatePlayerPos();

	void updateTime();

	void createParticle(const DirectX::XMFLOAT3 pos,
						const UINT particleNum = 10U, const float startScale = 1.f);
	void startParticle(const DirectX::XMFLOAT3 pos);

	DirectX::XMFLOAT3 easePos(const DirectX::XMFLOAT3 startPos,
							  const DirectX::XMFLOAT3 endPos,
							  const float timeRaito,
							  const int easeTime);

	void updateMovableRoad();

	void changeBeatProc(const Time::timeType& nowTime);

	void drawObj3d();
	void drawParticle();
	void drawSprite();

	virtual void additionalDrawBackSprite() {};
	virtual void additionalDrawFrontprite() {};
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

	// @param startPosition : �ŏ��Ɍ����������̃}�b�v���W���i�[����
	void loadMapFile(const std::string& csvFilePath, DirectX::XMFLOAT2* startPosition = nullptr);

};

