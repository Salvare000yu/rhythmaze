#include "PlayScene.h"

#include "SceneManager.h"

#include <sstream>
#include <iomanip>

#include <xaudio2.h>
#include "RandomNum.h"

#include <fstream>
#include <string>
#include <vector>

using namespace DirectX;

namespace {

	// @return �ǂݍ���csv�̒��g�B���s������f�t�H���g�R���X�g���N�^�ŏ��������ꂽ���vector2�����z�񂪕Ԃ�
	std::vector<std::vector<std::string>> loadCsv(const std::string& csvFilePath) {
		std::vector<std::vector<std::string>> csvData{};	// csv�̒��g���i�[

		std::ifstream ifs(csvFilePath);
		if (!ifs) {
			return csvData;
		}

		std::string line{};
		// �J�����t�@�C������s�ǂݍ���(�J�[�\��������)
		for (unsigned i = 0U; std::getline(ifs, line); i++) {
			csvData.emplace_back();

			std::istringstream stream(line);
			std::string field;
			// �ǂݍ��񂾍s��','��؂�ŕ���
			while (std::getline(stream, field, ',')) {
				csvData[i].emplace_back(field);
			}
		}

		return csvData;
	}

	// @return 0 <= ret[rad] < 2PI
	float angleRoundRad(float rad) {
		float angle = rad;

		if (angle >= 0.f && angle < XM_2PI) return angle;

		while (angle >= XM_2PI) {
			angle -= XM_2PI;
		}
		while (angle < 0) {
			angle += XM_2PI;
		}
		return angle;
	}

	float nearSin(float rad) {
		constexpr auto a = +0.005859483;
		constexpr auto b = +0.005587939;
		constexpr auto c = -0.171570726;
		constexpr auto d = +0.0018185485;
		constexpr auto e = +0.9997773594;

		double x = angleRoundRad(rad);

		// 0 ~ PI/2���킩��΋��߂���
		if (x < XM_PIDIV2) {
			// ���̂܂�
		} else if (x >= XM_PIDIV2 && x < XM_PI) {
			x = XM_PI - x;
		} else if (x < XM_PI * 1.5f) {
			x = -(x - XM_PI);
		} else if (x < XM_2PI) {
			x = -(XM_2PI - x);
		}

		return x * (x * (x * (x * (a * x + b) + c) + d) + e);
	}

	float nearCos(float rad) {
		return nearSin(rad + XM_PIDIV2);
	}

	float nearTan(float rad) {
		return nearSin(rad) / nearCos(rad);
	}

	float mySin(float rad) {
		float ret = angleRoundRad(rad);

		if (rad < XM_PIDIV2) {
			ret = nearSin(rad);
		} else if (rad < XM_PI) {
			ret = nearSin(XM_PI - rad);
		} else if (rad < XM_PI * 1.5f) {
			ret = -nearSin(rad - XM_PI);
		} else if (rad < XM_2PI) {
			ret = -nearSin(XM_2PI - rad);
		} else {
			ret = nearSin(rad);
		}

		return ret;
	}

	float myCos(float rad) {
		return mySin(rad + XM_PIDIV2);
	}
}

void PlayScene::init() {
	WinAPI::getInstance()->setWindowText("���Y���C�Y (SE : OtoLogic)");

	dxCom = DirectXCommon::getInstance();

	input = Input::getInstance();

#pragma region �r���[�ϊ�

	camera.reset(new Camera(WinAPI::window_width, WinAPI::window_height));
	camera->setEye(XMFLOAT3(0, 0, -175));	// ���_���W
	camera->setTarget(XMFLOAT3(0, 0, 0));	// �����_���W
	camera->setUp(XMFLOAT3(0, 1, 0));		// �����

#pragma endregion �r���[�ϊ�

#pragma region ��

	soundCommon.reset(new Sound::SoundCommon());
	bgm.reset(new Sound("Resources/Music/mmc_125_BGM2.wav", soundCommon.get()));

	particleSE.reset(new Sound("Resources/SE/Sys_Set03-click.wav", soundCommon.get()));

#pragma endregion ��

#pragma region �X�v���C�g

	// --------------------
	// �X�v���C�g����
	// --------------------
	spriteCommon = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height);

	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	Sprite::commonLoadTexture(spriteCommon, TEX_NUM::TEX1, L"Resources/texture.png", DirectXCommon::getInstance()->getDev());

	// �X�v���C�g�̐���
	for (int i = 0; i < _countof(sprites); i++) {
		sprites[i].create(
			DirectXCommon::getInstance()->getDev(),
			WinAPI::window_width, WinAPI::window_height,
			TEX_NUM::TEX1, spriteCommon, { 0,0 }, false, false
		);
		// �X�v���C�g�̍��W�ύX
		sprites[i].position.x = 1280 / 10;
		sprites[i].position.y = 720 / 10;
		//sprites[i].isInvisible = true;
		//sprites[i].position.x = (float)(rand() % 1280);
		//sprites[i].position.y = (float)(rand() % 720);
		//sprites[i].rotation = (float)(rand() % 360);
		//sprites[i].rotation = 0;
		//sprites[i].size.x = 400.0f;
		//sprites[i].size.y = 100.0f;
		// ���_�o�b�t�@�ɔ��f
		sprites[i].SpriteTransferVertexBuffer(spriteCommon);
	}

	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	Sprite::commonLoadTexture(spriteCommon, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());
	// �f�o�b�O�e�L�X�g������
	debugText.Initialize(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height, debugTextTexNumber, spriteCommon);

	// 3D�I�u�W�F�N�g�p�p�C�v���C������
	object3dPipelineSet = Object3d::createGraphicsPipeline(DirectXCommon::getInstance()->getDev());


#pragma endregion �X�v���C�g

#pragma region �}�b�v

	constexpr char mapCSVFilePath[] = "Resources/map/map.csv";
	static auto mapFileData = loadCsv(mapCSVFilePath);

	constexpr XMFLOAT4 wallCol = XMFLOAT4(0.5f, 0.3f, 0, 1);
	constexpr XMFLOAT4 backRoadCol = XMFLOAT4(1, 0, 1, 1);
	constexpr XMFLOAT4 frontRoadCol = XMFLOAT4(0, 1, 1, 1);
	constexpr XMFLOAT4 goalCol = XMFLOAT4(1, 0, 0, 1);

	for (UINT y = 0; y < mapFileData.size(); y++) {
		mapData.emplace_back();

		for (UINT x = 0; x < mapFileData[y].size(); x++) {
			mapData[y].emplace_back();

			std::string chip = mapFileData[y][x];
			if (chip == "1") {
				mapData[y][x] = MAP_NUM::WALL;
			} else if (chip == "2") {
				mapData[y][x] = MAP_NUM::FRONT_ROAD;
			} else if (chip == "3") {
				mapData[y][x] = MAP_NUM::BACK_ROAD;
			} else if (chip == "4") {
				mapData[y][x] = MAP_NUM::GOAL;
			} else {
				mapData[y][x] = MAP_NUM::UNDEF;
			}
		}
	}

#pragma endregion �}�b�v

#pragma region 3D�I�u�W�F�N�g

	model.reset(new Model(DirectXCommon::getInstance()->getDev(),
						  L"Resources/model/box/box.obj", L"Resources/model/box/box.png",
						  WinAPI::window_width, WinAPI::window_height,
						  Object3d::constantBufferNum, obj3dTexNum));

	constexpr float mapPosY = -150.f;

	for (UINT y = 0; y < mapData.size(); y++) {
		mapObj.emplace_back();
		for (UINT x = 0; x < mapData[y].size(); x++) {
			mapObj[y].emplace_back(Object3d(DirectXCommon::getInstance()->getDev(), model.get(), obj3dTexNum));
			mapObj[y][x].scale = XMFLOAT3(obj3dScale, obj3dScale, obj3dScale);
			mapObj[y][x].position = XMFLOAT3(x * mapSide,
											 mapPosY,
											 y * -mapSide);

			switch (mapData[y][x]) {
			case MAP_NUM::WALL:
				mapObj[y][x].color = wallCol;
				mapObj[y][x].scale.y *= 2.f;
				mapObj[y][x].position.y += obj3dScale;
				break;
			case MAP_NUM::FRONT_ROAD:
				mapObj[y][x].color = frontRoadCol;
				break;
			case MAP_NUM::BACK_ROAD:
				mapObj[y][x].color = backRoadCol;
				break;
			case MAP_NUM::GOAL:
				mapObj[y][x].color = goalCol;
				break;
			default:
				mapObj[y][x].color = XMFLOAT4(1, 1, 1, 1);
			}
		}
	}

	constexpr XMFLOAT2 startMapPos = XMFLOAT2(1, 1);
	playerMapPos = startMapPos;

	playerModel.reset(new Model(DirectXCommon::getInstance()->getDev(),
								L"Resources/model/player/model.obj", L"Resources/model/player/tex.png",
								WinAPI::window_width, WinAPI::window_height,
								Object3d::constantBufferNum, 0));
	playerObj.reset(new Object3d(dxCom->getDev(), playerModel.get(), 0));
	playerObj->scale = { obj3dScale, obj3dScale, obj3dScale };
	playerObj->position = mapObj[startMapPos.x][startMapPos.y].position;
	playerObj->position.y += mapSide;

#pragma endregion 3D�I�u�W�F�N�g

#pragma region ���C�g

	light = mapObj[0][0].position;
	light.x += mapObj[0][0].scale.y * 2;	// ��

#pragma endregion ���C�g


	// �p�[�e�B�N��������
	particleMgr.reset(new ParticleManager(dxCom->getDev(), L"Resources/effect1.png", camera.get()));

	Sound::SoundPlayWave(soundCommon.get(), bgm.get(), XAUDIO2_LOOP_INFINITE);

	// ���ԏ�����
	timer.reset(new Time());
}

void PlayScene::update() {

#pragma region �J�����ړ���]

	{

		const float rotaVal = XM_PIDIV2 / DirectXCommon::getInstance()->getFPS();	// ���b�l����

		if (input->hitKey(DIK_L)) {
			angle.y += rotaVal;
			if (angle.y > XM_PI * 2) { angle.y = 0; }
		} else if (input->hitKey(DIK_J)) {
			angle.y -= rotaVal;
			if (angle.y < 0) { angle.y = XM_PI * 2; }
		}

		if (input->hitKey(DIK_I)) {
			if (angle.x + rotaVal < XM_PIDIV2) angle.x += rotaVal;
		} else if (input->hitKey(DIK_K)) {
			if (angle.x - rotaVal > -XM_PIDIV2) angle.x -= rotaVal;
		}

		// angle���W�A������Y���܂��ɉ�]�B���a��-100
		constexpr float camRange = 100.f;	// targetLength
		camera->rotation(camRange, angle.x, angle.y);


		// �ړ���
		const float moveSpeed = 75.f / dxCom->getFPS();
		// ���_�ړ�
		if (input->hitKey(DIK_W)) {
			camera->moveForward(moveSpeed);
		} else if (input->hitKey(DIK_S)) {
			camera->moveForward(-moveSpeed);
		}
		if (input->hitKey(DIK_A)) {
			camera->moveRight(-moveSpeed);
		} else if (input->hitKey(DIK_D)) {
			camera->moveRight(moveSpeed);
		}

		XMFLOAT3 camPos = camera->getEye();
		camPos.x = playerObj->position.x;
		camPos.z = playerObj->position.z;
		camera->setEye(camPos);
		camera->setTarget(playerObj->position);
		camera->setUp(XMFLOAT3(0, 0, 1));
	}

#pragma endregion �J�����ړ���]

#pragma region ���^�C�~���O

	{
		const auto nowTime = timer->getNowTime();	// ���̎���
		constexpr float bpm = 125 * 2;	// beat / min ����������(�\��)
		const auto oneBeatTime = timer->getOneBeatTime(bpm);	// �ꔏ�̎��Ԃ��L�^

		constexpr float aheadJudgeRange = 0.25f;	// ���̒l�����̔��̎n�܂肪�����Ȃ�[0~1]

		// �����ς������
		if (nowTime >= oneBeatTime * (beatChangeNum + 1) - oneBeatTime * aheadJudgeRange) {
			beatChangeNum++;
			beatChangeTime = nowTime;	// ���̎��Ԃ��L�^
			frontBeatFlag = !frontBeatFlag;	// �\���Ɨ�����change

			if (playerMoved) {
				playerMoved = false;
			} //else combo = 0U;	// �~�܂��Ă�����R���{�����Z�b�g
		}

		const float beatRaito = (nowTime - beatChangeTime) / (float)oneBeatTime;	// ���̔��̐i�s�x[0~1]
		constexpr float movableRaitoMin = 0.625f, movableRaitoMax = 1.f - aheadJudgeRange;	// �ړ��ł��Ȃ����Ԃ͈̔�

		// ���͈͓̔��Ȃ�ړ��͂ł��Ȃ�
		movableFlag = !(movableRaitoMin < beatRaito&& beatRaito < movableRaitoMax);

		debugText.Print(spriteCommon, "O         N\nK         G", 0, debugText.fontHeight,
						1.f, XMFLOAT4(1, 1, 1, 0.5f));

		debugText.Print(spriteCommon,
						movableFlag == true ? "O\nK" : "N\nG",
						beatRaito * 10.f * debugText.fontWidth, debugText.fontHeight,
						1.f,
						XMFLOAT4(1, (float)movableFlag, 1, 1));

	}

#pragma endregion ���^�C�~���O


#pragma region �v���C���[�ړ�
	preMissFlag = missFlag;

	// �ړ��̓��͂���������
	if (input->triggerKey(DIK_UP) || input->triggerKey(DIK_DOWN) ||
		   input->triggerKey(DIK_LEFT) || input->triggerKey(DIK_RIGHT)) {

		// �ړ��\�Ȃ�
		if (playerMoved == false && movableFlag) {

			uint8_t moveDir = 0u;

			// �������������L�^
			if (input->triggerKey(DIK_UP)) {
				moveDir = DIK_UP;
			} else if (input->triggerKey(DIK_DOWN)) {
				moveDir = DIK_DOWN;
			} else if (input->triggerKey(DIK_LEFT)) {
				moveDir = DIK_LEFT;
			} else if (input->triggerKey(DIK_RIGHT)) {
				moveDir = DIK_RIGHT;
			}

			// ���ړ��\�ȓ��̎��
			auto nowMovableRoad = MAP_NUM::BACK_ROAD;
			if (frontBeatFlag) {
				nowMovableRoad = MAP_NUM::FRONT_ROAD;
			}

			// �ړ���̎�ނ��L�^
			MAP_NUM nextMapNum = MAP_NUM::UNDEF;	// �ړ���̃}�b�v�̎�ފi�[�p
			auto nextPlayerMapPos = playerMapPos;	// �ړ���̃v���C���[�̃}�b�v���W�i�[�p

			// �ړ���̃}�b�v�̎�ނ��L�^���A�ړ���̃v���C���[�̃}�b�v���W���L�^
			switch (moveDir) {
			case DIK_UP:
				if (playerMapPos.y - 1 >= 0) {
					nextMapNum = mapData[playerMapPos.y - 1][playerMapPos.x];
					nextPlayerMapPos.y--;
				}
				break;
			case DIK_DOWN:
				if (playerMapPos.y + 1 <= mapData.size() - 1) {
					nextMapNum = mapData[playerMapPos.y + 1][playerMapPos.x];
					nextPlayerMapPos.y++;
				}
				break;
			case DIK_LEFT:
				if (playerMapPos.x - 1 >= 0) {
					nextMapNum = mapData[playerMapPos.y][playerMapPos.x - 1];
					nextPlayerMapPos.x--;
				}
				break;
			case DIK_RIGHT:
				if (playerMapPos.x + 1 <= mapData[0].size() - 1) {
					nextMapNum = mapData[playerMapPos.y][playerMapPos.x + 1];
					nextPlayerMapPos.x++;
				}
				break;
			}

			// �ړ��\�Ȃ�ړ�����
			if (nextMapNum != MAP_NUM::UNDEF
			   &&
			  (nextMapNum == nowMovableRoad || nextMapNum == MAP_NUM::GOAL)) {
				// �ړ��������Ƃ��L�^
				playerMoved = true;
				// �ړ���̃}�b�v���W�𔽉f
				playerMapPos = nextPlayerMapPos;
				// �ړ���̍��W�𔽉f
				playerObj->position = XMFLOAT3(playerMapPos.x * mapSide,
											   playerObj->position.y,
											   playerMapPos.y * -mapSide);

				if (nextMapNum == MAP_NUM::GOAL) {
					SceneManager::getInstange()->changeScene(SCENE_NUM::END);
				}
				// �p�[�e�B�N���J�n
				createParticleFlag = true;
				// �R���{�����Z
				if (combo < UINT_MAX) combo++;

			}

			// ���͂͂��������ړ����Ȃ������Ȃ�
			if (playerMoved == false) {
				// ���������ړ����Ȃ����� = �~�X
				missFlag = true;
			}
		} else {
			// �����Ȃ���Ԃœ������Ƃ��� = �~�X
			missFlag = true;
		}

		// �~�X������
		if (missFlag && preMissFlag) {
			combo = 0U;	// �R���{�����Z�b�g
			missFlag = false;
			preMissFlag = false;
		}
	}
#pragma endregion �v���C���[�ړ�

#pragma region �p�[�e�B�N��

	// P���������уp�[�e�B�N��50���ǉ�
	if (createParticleFlag) {
		constexpr UINT particleNumMax = 50U, particleNumMin = 20U;
		UINT particleNum = particleNumMin;

		constexpr float scaleMin = 1.f, scaleMax = 10.f;
		constexpr UINT maxCombo = 20U;
		float startScale = scaleMin;

		auto nowComboRaito = (float)combo / maxCombo;
		if (nowComboRaito >= 1.f) nowComboRaito = 1.f;
		else if (nowComboRaito <= 0.f) nowComboRaito = 0.f;

		// �R���{���ɉ����Ĕh��ɂ���[0~20]
		particleNum += (particleNumMax - particleNumMin) * nowComboRaito;
		startScale += (scaleMax - scaleMin) * nowComboRaito;

		createParticle(playerObj->position, particleNum, startScale);

		// SE�炷
		Sound::SoundPlayWave(soundCommon.get(), particleSE.get());

		createParticleFlag = false;
	}

#pragma endregion �p�[�e�B�N��

#pragma region ���C�g
	{

		// �v���C���[�̈ʒu�Ƀ��C�g��u��
		light = playerObj->position;

		for (UINT y = 0; y < mapData.size(); y++) {
			for (UINT x = 0; x < mapData[y].size(); x++) {
				mapObj[y][x].setLightPos(light);
			}
		}
	}
#pragma endregion ���C�g



#pragma region ���\��
	constexpr auto dbFontCol = XMFLOAT4(1, 1, 1, 1);


	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 3, 1.f,
						  dbFontCol,
						  "%u combo", combo);

	debugText.formatPrint(spriteCommon, 0, 0, 1.f,
						  dbFontCol, "FPS : %f", dxCom->getFPS());

	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 15, 1.f,
						  dbFontCol,
						  "Time : %.6f[s]", (long double)timer->getNowTime() / Time::oneSec);

	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 16, 1.f,
						  dbFontCol,
						  "half beat : %u", beatChangeNum);

	debugText.Print(spriteCommon, "WS : move camera", 0, debugText.fontHeight * 8);

#pragma endregion ���\��

	camera->update();
}

void PlayScene::draw() {
	// �S�D�`��R�}���h��������
	// 3D�I�u�W�F�N�g�R�}���h
	Object3d::startDraw(dxCom->getCmdList(), object3dPipelineSet);
	for (UINT y = 0; y < mapData.size(); y++) {
		for (UINT x = 0; x < mapData[y].size(); x++) {
			mapObj[y][x].drawWithUpdate(camera->getViewMatrix(), dxCom);
		}
	}
	playerObj->drawWithUpdate(camera->getViewMatrix(), dxCom);

	ParticleManager::startDraw(dxCom->getCmdList(), object3dPipelineSet);
	particleMgr->drawWithUpdate(dxCom->getCmdList());


	// �X�v���C�g���ʃR�}���h
	Sprite::drawStart(spriteCommon, dxCom->getCmdList());
	// �X�v���C�g�`��
	/*for (UINT i = 0; i < _countof(sprites); i++) {
		sprites[i].drawWithUpdate(dxCom, spriteCommon);
	}*/
	// �f�o�b�O�e�L�X�g�`��
	debugText.DrawAll(dxCom, spriteCommon);
	// �S�D�`��R�}���h�����܂�
}

void PlayScene::fin() {
	//Sound::SoundStopWave(soundData1.get());
}

void PlayScene::createParticle(const DirectX::XMFLOAT3 pos, const UINT particleNum, const float startScale) {
	for (UINT i = 0U; i < particleNum; i++) {

		const float theata = RandomNum::getRandf(0, XM_PIDIV2);
		const float phi = RandomNum::getRandf(0, XM_PI * 2.f);
		const float r = RandomNum::getRandf(0, 5.f);

		// X,Y,Z�S��[-2.5f,+2.5f]�Ń����_���ɕ��z
		constexpr float rnd_pos = 2.5f;
		XMFLOAT3 generatePos = pos;
		/*generatePos.x += Random::getRandNormallyf(0.f, rnd_pos);
		generatePos.y += Random::getRandNormallyf(0.f, rnd_pos);
		generatePos.z += Random::getRandNormallyf(0.f, rnd_pos);*/

		//constexpr float rnd_vel = 0.0625f;
		const XMFLOAT3 vel{
			r * sin(theata) * cos(phi),
			r * cos(theata),
			r * sin(theata) * sin(phi)
		};

		//constexpr float rnd_acc = 0.05f;
		XMFLOAT3 acc{};

		/*acc.x = 0.f;
		acc.y = -Random::getRandf(rnd_acc, rnd_acc * 2.f);
		acc.z = 0.f;*/


		constexpr auto startCol = XMFLOAT3(1, 1, 0.75f), endCol = XMFLOAT3(1, 0, 1);
		constexpr int life = Time::oneSec / 4;
		constexpr float endScale = 0.f;
		constexpr float startRota = 0.f, endRota = 0.f;

		// �ǉ�
		particleMgr->add(timer.get(),
						 life, generatePos, vel, acc,
						 startScale, endScale,
						 startRota, endRota,
						 startCol, endCol);
	}
}