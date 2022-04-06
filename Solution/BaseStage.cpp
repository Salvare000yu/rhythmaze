#include "BaseStage.h"

#include <sstream>
#include <fstream>

#include "SceneManager.h"

#include "RandomNum.h"

using namespace DirectX;

DirectX::XMFLOAT3 BaseStage::easePos(const DirectX::XMFLOAT3 startPos,
									 const DirectX::XMFLOAT3 endPos,
									 const float timeRaito,
									 const int easeTime) {
	const auto raitoCoe = 1.f - timeRaito;
	const auto easeVal = raitoCoe * raitoCoe * raitoCoe * raitoCoe * raitoCoe;
	const auto nowTime = easeVal * easeTime;

	const auto moveVal = XMFLOAT3(
		endPos.x - startPos.x,
		endPos.y - startPos.y,
		endPos.z - startPos.z
	);

	return XMFLOAT3(
		startPos.x + (1.f - easeVal) * moveVal.x,
		startPos.y + (1.f - easeVal) * moveVal.y,
		startPos.z + (1.f - easeVal) * moveVal.z
	);
}

void BaseStage::updateLightPosition() {
	// �v���C���[�̈ʒu�Ƀ��C�g��u��
	light = playerObj->position;
	light.z -= playerObj->scale.y / 2.f;

	playerObj->setLightPos(light);

	for (UINT y = 0; y < mapData.size(); y++) {
		for (UINT x = 0; x < mapData[y].size(); x++) {
			mapObj[y][x].setLightPos(light);
		}
	}
}

void BaseStage::updateCamera() {
	// �ړ���
	const float moveSpeed = 75.f / dxCom->getFPS();
	// ���_�ړ�
	if (input->hitKey(DIK_E)) {
		camera->moveForward(moveSpeed);
	} else if (input->hitKey(DIK_Z)) {
		camera->moveForward(-moveSpeed);
	}

	XMFLOAT3 camPos = camera->getEye();
	camPos.x = playerObj->position.x;
	camPos.z = playerObj->position.z;
	camera->setEye(camPos);
	camera->setTarget(playerObj->position);
	camera->setUp(XMFLOAT3(0, 0, 1));

	camera->update();
}

bool BaseStage::goal() {
	// �R���{�����N���A�����ɒB���Ă�����
	if (combo >= clearCombo) {
		SceneManager::getInstange()->goal(beatChangeNum, combo, true);
		return true;
	}
	return false;
}

void BaseStage::timeOut() {
	SceneManager::getInstange()->goal(beatChangeNum, combo, false);
}

void BaseStage::updatePlayerPos() {

	// �ړ��Ɏg���L�[
	constexpr auto up = DIK_UP;
	constexpr auto down = DIK_DOWN;
	constexpr auto right = DIK_RIGHT;
	constexpr auto left = DIK_LEFT;

	// �ړ��̓��͂���������
	if (input->triggerKey(up) || input->triggerKey(down)
		|| input->triggerKey(left) || input->triggerKey(right)) {

		// �S�[���������ǂ���
		bool goalFlag = false;

		// �ړ��\�Ȃ�(�܂��ړ����Ă��炸�A���ړ���������Ă���Ȃ�)
		if (!playerMoved) {

			uint8_t moveDir = 0u;

			// �������������L�^
			if (input->triggerKey(up)) {
				moveDir = up;
			} else if (input->triggerKey(down)) {
				moveDir = down;
			} else if (input->triggerKey(left)) {
				moveDir = left;
			} else if (input->triggerKey(right)) {
				moveDir = right;
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
			case up:
				if (playerMapPos.y - 1 >= 0) {
					nextMapNum = mapData[playerMapPos.y - 1][playerMapPos.x];
					nextPlayerMapPos.y--;
				}
				break;
			case down:
				if (playerMapPos.y + 1 <= mapData.size() - 1) {
					nextMapNum = mapData[playerMapPos.y + 1][playerMapPos.x];
					nextPlayerMapPos.y++;
				}
				break;
			case left:
				if (playerMapPos.x - 1 >= 0) {
					nextMapNum = mapData[playerMapPos.y][playerMapPos.x - 1];
					nextPlayerMapPos.x--;
				}
				break;
			case right:
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
				// �C�[�W���O�J�n
				playerEasing = true;
				easeStartPos = playerObj->position;
				easeEndPos = XMFLOAT3(playerMapPos.x * mapSide,
											   playerObj->position.y,
											   playerMapPos.y * -mapSide);
				// �C�[�W���O�J�n���Ԃ����Z�b�g
				easeTime->reset();

				// �R���{�����Z
				if (combo < UINT_MAX) ++combo;

				// �ړ���̈ʒu���S�[���Ȃ�S�[�����Ă���
				if (nextMapNum == MAP_NUM::GOAL) {
					goalFlag = true;
				}
				// �p�[�e�B�N���J�n
				createParticleFlag = true;
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
		if (missFlag) {
			combo = 0U;	// �R���{�����Z�b�g
			missFlag = false;
		}

		if (goalFlag) goal();
	}

	if (playerEasing) {
		const auto halfBeatTime = 60 * Time::oneSec / musicBpm / 2.f;
		const auto easeAllTime = halfBeatTime / 2;

		const auto easeNowTime = easeTime->getNowTime();

		const auto easeTimeRaito = (float)easeNowTime / easeAllTime;

		playerObj->position = easePos(easeStartPos, easeEndPos, easeTimeRaito, easeAllTime);

		//�w�肵�����Ԃ��߂�����C�[�W���O�I��
		if (easeTimeRaito >= 1.f) playerEasing = false;
	}
}

void BaseStage::updateTime() {
	const auto nowTime = timer->getNowTime();	// ���̎���
	float speed = musicBpm * 2/*/2*/;	// beat / min ����������(�\��)
	const auto oneBeatTime = timer->getOneBeatTime(speed);	// �ꔏ�̎��Ԃ��L�^

	constexpr float aheadJudgeRange = 0.25f;	// ���̒l�����̔��̎n�܂肪�����Ȃ�[0~1]

	// �����ς������
	if (nowTime >= oneBeatTime * (beatChangeNum + 1) - oneBeatTime * aheadJudgeRange) {
		beatChangeNum++;
		beatChangeTime = nowTime;	// ���̎��Ԃ��L�^
		frontBeatFlag = !frontBeatFlag;	// �\���Ɨ�����change

		if (playerMoved) {
			playerMoved = false;
		} //else combo = 0U;	// �~�܂��Ă�����R���{�����Z�b�g

		if (frontBeatFlag) {
			playerObj->scale = XMFLOAT3(playerScale, playerScale, playerScale);
		} else {
			const float backScale = playerScale * 0.8f;
			playerObj->scale = XMFLOAT3(backScale, backScale, backScale);
		}

		// --------------------
		// �i�߂Ȃ�����ǂɂ���
		// --------------------
		constexpr auto wallCol = XMFLOAT4(0.25f, 0.25f, 0.25f, 1);
		constexpr auto defColor = XMFLOAT4(1, 1, 1, 1);
		constexpr auto frontColor = XMFLOAT4(0, 1, 0, 1);
		constexpr auto backColor = XMFLOAT4(1, 0.5f, 1, 1);
		/*for (UINT y = 0; y < mapData.size(); y++) {
			for (UINT x = 0; x < mapData[y].size(); x++) {
				switch (mapData[y][x]) {
				case MAP_NUM::FRONT_ROAD:
					if (frontBeatFlag) {
						mapObj[y][x].position.y = floorPosY;
						mapObj[y][x].color = defColor;
					} else {
						mapObj[y][x].position.y = floorPosY + obj3dScale;
						mapObj[y][x].color = wallCol;
					}
					break;
				case MAP_NUM::BACK_ROAD:
					if (frontBeatFlag) {
						mapObj[y][x].position.y = floorPosY + obj3dScale;
						mapObj[y][x].color = wallCol;
					} else {
						mapObj[y][x].position.y = floorPosY;
						mapObj[y][x].color = defColor;
					}
					break;
				}
			}
		}*/
		if (frontBeatFlag) {
			playerObj->color = defColor;
			circleSprite->color = frontColor;
		} else {
			playerObj->color = XMFLOAT4(0.5, 0.5, 0.5, 1);
			circleSprite->color = backColor;
		}


		// --------------------
		// ���v���C���[������Ƃ���͕ǂɂ��Ȃ�
		// --------------------
		mapObj[playerMapPos.y][playerMapPos.x].position.y = floorPosY;

		mapObj[playerMapPos.y][playerMapPos.x].color = defColor;

		if (mapData[playerMapPos.y][playerMapPos.x] == MAP_NUM::FRONT_ROAD) mapObj[playerMapPos.y][playerMapPos.x].texNum = BOX_TEXNUM::FRONT;
		else mapObj[playerMapPos.y][playerMapPos.x].texNum = BOX_TEXNUM::BACK;
	}

	const float beatRaito = (nowTime - beatChangeTime) / (float)oneBeatTime;	// ���̔��̐i�s�x[0~1]
	constexpr float movableRaitoMin = 0.625f, movableRaitoMax = 1.f - aheadJudgeRange;	// �ړ��ł��Ȃ����Ԃ͈̔�

	// ���͈͓̔��Ȃ�ړ��͂ł��Ȃ�
	//movableFlag = !(movableRaitoMin < beatRaito&& beatRaito < movableRaitoMax);

	debugText.Print(spriteCommon, "X         X", 0, debugText.fontHeight,
					1.f, XMFLOAT4(1, 1, 1, 0.5f));

	debugText.Print(spriteCommon,
					"X",
					beatRaito * 10.f * debugText.fontWidth, debugText.fontHeight,
					1.f,
					XMFLOAT4(1, 1, 1, 1));


	constexpr float circleScaleMin = 0.3f;
	float circleScale = beatRaito * (1.f - circleScaleMin) + circleScaleMin;
	if (!frontBeatFlag) circleScale = 1.f - circleScale, circleScale = 0;

	circleSprite->size = XMFLOAT2(circleSprite->texSize.x * circleScale,
								  circleSprite->texSize.y * circleScale);

	constexpr float circleColMax = 1.f - circleScaleMin;
	circleSprite->color.w = circleColMax - beatRaito * circleColMax;
	if (!frontBeatFlag) circleSprite->color.w = 1.f - circleSprite->color.w;

	circleSprite->SpriteTransferVertexBuffer(spriteCommon);
}

void BaseStage::createParticle(const DirectX::XMFLOAT3 pos,
							   const UINT particleNum, const float startScale) {
	for (UINT i = 0U; i < particleNum; ++i) {

		constexpr auto startCol = XMFLOAT3(1, 1, 0.75f), endCol = XMFLOAT3(1, 0, 1);
		constexpr int life = Time::oneSec / 4;
		constexpr float endScale = 0.f;
		constexpr float startRota = 0.f, endRota = 0.f;

		// �ǉ�
		particleMgr->add(timer.get(),
						 life, pos,
						 startScale, endScale,
						 startRota, endRota,
						 startCol, endCol);
	}
}

void BaseStage::startParticle(const DirectX::XMFLOAT3 pos) {
	constexpr UINT particleNumMax = 50U, particleNumMin = 20U;
	UINT particleNum = particleNumMin;

	constexpr float scaleMin = 1.5f, scaleMax = 10.f;
	constexpr UINT maxCombo = 20U;
	float startScale = scaleMin;

	auto nowComboRaito = (float)combo / maxCombo;
	if (nowComboRaito >= 1.f) nowComboRaito = 1.f;
	else if (nowComboRaito <= 0.f) nowComboRaito = 0.f;

	// �R���{���ɉ����Ĕh��ɂ���[0~20]
	particleNum += (particleNumMax - particleNumMin) * nowComboRaito;
	startScale += (scaleMax - scaleMin) * nowComboRaito;

	createParticle(pos, particleNum, startScale);

	// SE�炷
	Sound::SoundPlayWave(soundCommon.get(), particleSE.get());
}



std::vector<std::vector<std::string>> BaseStage::loadCsv(const std::string& csvFilePath) {
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

void BaseStage::loadMapFile(const std::string& csvFilePath) {
	const auto mapFileData = loadCsv(csvFilePath);

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
}


void BaseStage::init() {
	pathInit();

#pragma region �V���O���g���C���X�^���X
	dxCom = DirectXCommon::getInstance();

	input = Input::getInstance();
#pragma endregion �V���O���g���C���X�^���X

#pragma region �r���[�ϊ�

	camera.reset(new Camera(WinAPI::window_width, WinAPI::window_height));
	camera->setEye(XMFLOAT3(0, 0, -175));	// ���_���W
	camera->setTarget(XMFLOAT3(0, 0, 0));	// �����_���W
	camera->setUp(XMFLOAT3(0, 1, 0));		// �����

#pragma endregion �r���[�ϊ�

#pragma region �X�v���C�g
	spriteCommon = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
											  WinAPI::window_width, WinAPI::window_height);

	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	Sprite::commonLoadTexture(spriteCommon,
							  debugTextTexNumber,
							  L"Resources/debugfont.png",
							  DirectXCommon::getInstance()->getDev());

	Sprite::commonLoadTexture(spriteCommon,
							  0,
							  L"Resources/circle.png",
							   DirectXCommon::getInstance()->getDev());

	circleSprite.reset(new Sprite());
	circleSprite->create(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height,
						 0, spriteCommon);
	circleSprite->position.x = WinAPI::window_width / 2;
	circleSprite->position.y = WinAPI::window_height / 2;


	debugText.Initialize(dxCom->getDev(),
						 WinAPI::window_width, WinAPI::window_height,
						 debugTextTexNumber,
						 spriteCommon);
#pragma endregion �X�v���C�g

#pragma region �}�b�v�t�@�C��

	loadMapFile(mapCSVFilePath);

#pragma endregion �}�b�v�t�@�C��

#pragma region 3D�I�u�W�F�N�g

#pragma region �}�b�v

	object3dPipelineSet = Object3d::createGraphicsPipeline(dxCom->getDev());
	backPipelineSet = Object3d::createGraphicsPipeline(dxCom->getDev(), Object3d::BLEND_MODE::ALPHA,
													   L"Resources/Shaders/backVS.hlsl",
													   L"Resources/Shaders/backPS.hlsl");

	backModel.reset(new Model(dxCom->getDev(),
							  backModelPath.c_str(), backModelTexPath.c_str(),
							  WinAPI::window_width, WinAPI::window_height,
							  Object3d::constantBufferNum,
							  0));
	backObj.reset(new Object3d(dxCom->getDev(), backModel.get(), 0));
	constexpr float backScale = 10.f;
	backObj->scale = XMFLOAT3(backScale, backScale, backScale);

	model.reset(new Model(DirectXCommon::getInstance()->getDev(),
						  boxModelPath.c_str(), boxModelTexPath_wall.c_str(),
						  WinAPI::window_width, WinAPI::window_height,
						  Object3d::constantBufferNum, BOX_TEXNUM::WALL));
	model->loadTexture(dxCom->getDev(), boxModelTexPath_wall.c_str(), BOX_TEXNUM::WALL);
	model->loadTexture(dxCom->getDev(), boxModelTexPath_front.c_str(), BOX_TEXNUM::FRONT);
	model->loadTexture(dxCom->getDev(), boxModelTexPath_back.c_str(), BOX_TEXNUM::BACK);
	model->loadTexture(dxCom->getDev(), boxModelTexPath_goal.c_str(), BOX_TEXNUM::GOAL);

#pragma endregion �}�b�v

#pragma region ���H

	constexpr XMFLOAT4 wallCol = XMFLOAT4(0.5f, 0.3f, 0, 1);
	constexpr XMFLOAT4 backRoadCol = XMFLOAT4(1, 0, 1, 1);
	constexpr XMFLOAT4 frontRoadCol = XMFLOAT4(0, 1, 1, 1);
	constexpr XMFLOAT4 goalCol = XMFLOAT4(1, 0, 0, 1);

	constexpr float mapPosY = -150.f;
	floorPosY = mapPosY;

	for (UINT y = 0; y < mapData.size(); ++y) {
		mapObj.emplace_back();
		for (UINT x = 0; x < mapData[y].size(); ++x) {
			mapObj[y].emplace_back(Object3d(DirectXCommon::getInstance()->getDev(), model.get(), BOX_TEXNUM::WALL));
			mapObj[y][x].scale = XMFLOAT3(obj3dScale, obj3dScale, obj3dScale);
			mapObj[y][x].position = XMFLOAT3(x * mapSide,
											 mapPosY,
											 y * -mapSide);

			switch (mapData[y][x]) {
			case MAP_NUM::WALL:
				mapObj[y][x].position.y += obj3dScale;
				mapObj[y][x].texNum = BOX_TEXNUM::WALL;
				mapObj[y][x].color = XMFLOAT4(0.25f, 0.25f, 0.25f, 1);
				break;
			case MAP_NUM::FRONT_ROAD:
				mapObj[y][x].texNum = BOX_TEXNUM::FRONT;
				break;
			case MAP_NUM::BACK_ROAD:
				mapObj[y][x].texNum = BOX_TEXNUM::BACK;
				break;
			case MAP_NUM::GOAL:
				mapObj[y][x].texNum = BOX_TEXNUM::GOAL;
				mapObj[y][x].color = XMFLOAT4(1, 1, 1, 1);
				break;
			default:
				mapObj[y][x].color = XMFLOAT4(0, 0, 1, 1);
			}
		}
	}

#pragma endregion ���H

#pragma region �v���C���[

	constexpr XMFLOAT2 startMapPos = XMFLOAT2(1, 1);
	playerMapPos = startMapPos;

	playerModel.reset(new Model(DirectXCommon::getInstance()->getDev(),
								playerModelPath.c_str(), playerModelTexPath.c_str(),
								WinAPI::window_width, WinAPI::window_height,
								Object3d::constantBufferNum, 0));

	playerObj.reset(new Object3d(dxCom->getDev(), playerModel.get(), 0));

	playerScale = obj3dScale * 0.4f;

	playerObj->scale = { playerScale, playerScale, playerScale };

	playerObj->position = mapObj[startMapPos.y][startMapPos.x].position;
	playerObj->position.y += mapSide;

	playerObj->rotation.y += 90.f;
	playerObj->rotation.z += 90.f;
	//playerObj->scale = XMFLOAT3(0.5f, 0.5f, 0.5f);

	easeStartPos = playerObj->position;
	easeEndPos = playerObj->position;

#pragma endregion �v���C���[

#pragma endregion 3D�I�u�W�F�N�g

#pragma region ���C�g

	updateLightPosition();

#pragma endregion ���C�g

#pragma region �p�[�e�B�N��

	// �p�[�e�B�N��������
	particleMgr.reset(new ParticleManager(dxCom->getDev(), effectTexPath.c_str(), camera.get()));
#pragma endregion �p�[�e�B�N��

#pragma region ��

	soundCommon.reset(new Sound::SoundCommon());
	bgm.reset(new Sound(bgmFilePath.c_str(), soundCommon.get()));

	particleSE.reset(new Sound(particleSeFilePath.c_str(), soundCommon.get()));

#pragma endregion ��

	additionalInit();

	// BGM�Đ�
	Sound::SoundPlayWave(soundCommon.get(), bgm.get(), XAUDIO2_LOOP_INFINITE, bgmBolume);

	// ���ԏ�����
	easeTime.reset(new Time());
	timer.reset(new Time());
}

void BaseStage::update() {
	// �V����]
	backObj->rotation.y += 0.1f;

	// �J�����̍��W���X�V
	updateCamera();

	// ���Ԋ֌W�̍X�V
	updateTime();

	// �v���C���[�̍��W���X�V
	updatePlayerPos();

	// �p�[�e�B�N���J�n
	if (createParticleFlag) {
		startParticle(playerObj->position);

		createParticleFlag = false;
	}

	// ���C�g�̈ʒu�X�V
	updateLightPosition();

	additionalUpdate();

#pragma region ���\��

	constexpr XMFLOAT4 dbFontCol = XMFLOAT4(1, 1, 1, 1);

	debugText.formatPrint(spriteCommon, 0, 0, 1.f,
						  dbFontCol, "FPS : %f", dxCom->getFPS());

	debugText.Print(spriteCommon, "EZ : move camera", 0, debugText.fontHeight * 3);

	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 4, 1.f,
						  dbFontCol,
						  "combo %u / %u", combo, clearCombo);

	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 5, 1.f,
						  dbFontCol,
						  "count %u / %u", beatChangeNum, clearCount);



	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 7, 1.f,
						  dbFontCol,
						  "Time : %.6f[s]", (long double)timer->getNowTime() / Time::oneSec);

	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 8, 1.f,
						  dbFontCol,
						  "count Remaining : %u / %u", clearCount - beatChangeNum, clearCount);

#pragma endregion ���\��

	// ��������
	if (beatChangeNum >= clearCount) {
		timeOut();
	}
}

void BaseStage::draw() {
	drawObj3d();

	drawParticle();

	drawSprite();
}



void BaseStage::drawObj3d() {
	Object3d::startDraw(DirectXCommon::getInstance()->getCmdList(), object3dPipelineSet);

	backObj->drawWithUpdate(camera->getViewMatrix(), dxCom);

	for (UINT y = 0; y < mapData.size(); ++y) {
		for (UINT x = 0; x < mapData[y].size(); ++x) {
			mapObj[y][x].drawWithUpdate(camera->getViewMatrix(), dxCom);
		}
	}
	playerObj->drawWithUpdate(camera->getViewMatrix(), dxCom);

	additionalDrawObj3d();
}

void BaseStage::drawParticle() {
	ParticleManager::startDraw(dxCom->getCmdList(), object3dPipelineSet);

	particleMgr->drawWithUpdate(dxCom->getCmdList(), playerObj->position);

	additionalDrawParticle();
}

void BaseStage::drawSprite() {
	Sprite::drawStart(spriteCommon, dxCom->getCmdList());
	circleSprite->drawWithUpdate(dxCom, spriteCommon);
	// �X�v���C�g�`��
	additionalDrawSprite();

	// �f�o�b�O�e�L�X�g�`��
	debugText.DrawAll(dxCom, spriteCommon);
}