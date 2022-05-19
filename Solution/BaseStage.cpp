#include "BaseStage.h"

#include <sstream>
#include <fstream>

#include "SceneManager.h"

#include "RandomNum.h"

using namespace DirectX;

namespace {
	constexpr float wallColFL = 0.3f;
	constexpr auto wallCol = XMFLOAT4(wallColFL, wallColFL, wallColFL, 1);
	constexpr auto noRoadWallCol = wallCol;
	constexpr auto roadWallCol = wallCol;
}

// @return 今の位置
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

void BaseStage::updateMovableRoad() {
	// --------------------
	// 進めない道を壁にする
	// --------------------
	constexpr auto defColor = XMFLOAT4(1, 1, 1, 1);
	constexpr auto frontColor = XMFLOAT4(0, 1, 0, 1);
	constexpr auto backColor = XMFLOAT4(1, 0.5f, 1, 1);
	for (UINT y = 0, yLen = mapData.size(); y < yLen; y++) {
		for (UINT x = 0, xLen = mapData[y].size(); x < xLen; x++) {
			switch (mapData[y][x]) {
			case MAP_NUM::FRONT_ROAD:
				if (frontBeatFlag) {
					mapObj[y][x].position.y = floorPosY;
					mapObj[y][x].texNum = BOX_TEXNUM::FRONT;
					mapObj[y][x].color = defColor;
				} else {
					mapObj[y][x].position.y = floorPosY + obj3dScale;
					mapObj[y][x].texNum = BOX_TEXNUM::WALL;
					mapObj[y][x].color = roadWallCol;
				}
				break;
			case MAP_NUM::BACK_ROAD:
				if (frontBeatFlag) {
					mapObj[y][x].position.y = floorPosY + obj3dScale;
					mapObj[y][x].texNum = BOX_TEXNUM::WALL;
					mapObj[y][x].color = roadWallCol;
				} else {
					mapObj[y][x].position.y = floorPosY;
					mapObj[y][x].texNum = BOX_TEXNUM::BACK;
					mapObj[y][x].color = defColor;
				}
				break;
			}
		}
	}

	// --------------------
	// 今プレイヤーがいるところは壁にしない
	// --------------------
	mapObj[playerMapPos.y][playerMapPos.x].position.y = floorPosY;

	mapObj[playerMapPos.y][playerMapPos.x].color = defColor;

	if (mapData[playerMapPos.y][playerMapPos.x] == MAP_NUM::FRONT_ROAD) mapObj[playerMapPos.y][playerMapPos.x].texNum = BOX_TEXNUM::FRONT;
	else mapObj[playerMapPos.y][playerMapPos.x].texNum = BOX_TEXNUM::BACK;
}

void BaseStage::changeBeatProc(const Time::timeType &nowTime) {
	beatChangeNum++;
	beatChangeTime = nowTime;	// 今の時間を記録
	frontBeatFlag = !frontBeatFlag;	// 表迫と裏拍をchange

	if (playerMoved) {
		playerMoved = false;
	} //else combo = 0U;	// 止まっていたらコンボをリセット

	if (frontBeatFlag) {
		playerObj->scale = XMFLOAT3(playerScale, playerScale, playerScale);
	} else {
		const float backBeatScale = playerScale * 0.8f;
		playerObj->scale = XMFLOAT3(backBeatScale, backBeatScale, backBeatScale);
	}

	updateMovableRoad();
}

void BaseStage::updateLightPosition() {
	// プレイヤーの位置にライトを置く
	light = playerObj->position;
	light.z -= playerObj->scale.y / 2.f;

	playerObj->setLightPos(light);

	for (auto &y : mapObj) {
		for (auto &x : y) {
			x.setLightPos(light);
		}
	}
}

void BaseStage::updateCamera() {
	// 移動量
	const float moveSpeed = 75.f / dxCom->getFPS();
	// 視点移動
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
	// コンボ数がクリア条件に達していたら
	SceneManager::getInstance()->goal(stageNum, beatChangeNum, combo, true);
	return true;
}

void BaseStage::timeOut() {
	SceneManager::getInstance()->goal(stageNum, beatChangeNum, combo, false);
}

void BaseStage::updatePlayerPos() {

	// 移動入力情報
	const bool inputUp = input->triggerKey(DIK_UP) || input->triggerKey(DIK_W);
	const bool inputDown = input->triggerKey(DIK_DOWN) || input->triggerKey(DIK_S);
	const bool inputRight = input->triggerKey(DIK_RIGHT) || input->triggerKey(DIK_D);
	const bool inputLeft = input->triggerKey(DIK_LEFT) || input->triggerKey(DIK_A);

	constexpr uint8_t up = 0b0001;
	constexpr uint8_t down = 0b0010;
	constexpr uint8_t left = 0b0100;
	constexpr uint8_t right = 0b1000;

	// 移動の入力があったら
	if (inputUp || inputDown || inputRight || inputLeft) {

		// ゴールしたかどうか
		bool goalFlag = false;

		// 移動可能なら(まだ移動しておらず、かつ移動が許されているなら)
		if (!playerMoved) {

			uint8_t moveDir = 0u;

			// 押した方向を記録
			if (inputUp) {
				moveDir = up;
			} else if (inputDown) {
				moveDir = down;
			} else if (inputLeft) {
				moveDir = left;
			} else if (inputRight) {
				moveDir = right;
			}

			// 今移動可能な道の種類
			auto nowMovableRoad = MAP_NUM::BACK_ROAD;
			if (frontBeatFlag) {
				nowMovableRoad = MAP_NUM::FRONT_ROAD;
			}

			// 移動後の種類を記録
			MAP_NUM nextMapNum = MAP_NUM::UNDEF;	// 移動後のマップの種類格納用
			auto nextPlayerMapPos = playerMapPos;	// 移動後のプレイヤーのマップ座標格納用

			// 移動先のマップの種類を記録し、移動後のプレイヤーのマップ座標を記録
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

			// 移動可能なら移動する
			if (nextMapNum != MAP_NUM::UNDEF
				&&
				(nextMapNum == nowMovableRoad || nextMapNum == MAP_NUM::GOAL)) {
				// 移動したことを記録
				playerMoved = true;
				// 移動後のマップ座標を反映
				playerMapPos = nextPlayerMapPos;
				// イージング開始
				playerEasing = true;
				easeStartPos = playerObj->position;
				easeEndPos = XMFLOAT3(playerMapPos.x * mapSide,
									  playerObj->position.y,
									  playerMapPos.y * -mapSide);
				// イージング開始時間をリセット
				easeTime->reset();

				// コンボ数加算
				if (combo < UINT_MAX) ++combo;

				// 移動後の位置がゴールならゴールしている
				if (nextMapNum == MAP_NUM::GOAL) {
					goalFlag = true;
				}
				// パーティクル開始
				createParticleFlag = true;
			}

			// 入力はあったが移動しなかったなら
			if (playerMoved == false) {
				// 押したが移動しなかった = ミス
				missFlag = true;
			}
		} else {
			// 動けない状態で動こうとした = ミス
			missFlag = true;
		}

		// ミスしたら
		if (missFlag) {
			combo = 0U;	// コンボをリセット
			missFlag = false;

			beatChangeNum++;
			missNum++;

			red->isInvisible = false;
			redTimer->reset();
		}

		if (goalFlag) goal();
	}

	if (playerEasing) {
		const auto halfBeatTime = 60 * Time::oneSec / musicBpm / 2.f;
		const auto easeAllTime = halfBeatTime / 2;

		const auto easeNowTime = easeTime->getNowTime();

		const auto easeTimeRaito = (float)easeNowTime / easeAllTime;

		playerObj->position = easePos(easeStartPos, easeEndPos, easeTimeRaito, easeAllTime);

		//指定した時間が過ぎたらイージング終了
		if (easeTimeRaito >= 1.f) playerEasing = false;
	}

	if (!red->isInvisible) {
		auto nowTime = redTimer->getNowTime();

		auto raito = (float)nowTime / (float)redTime;
		if (raito >= 1.f) {
			red->isInvisible = true;	// 定めた時間が経過したら終了
		} else {
			red->color.w = (1.f - raito) * 0.5f;	// 0 ~ 0.5の範囲で変化
			red->SpriteTransferVertexBuffer(spriteCommon);
		}
	}
}

void BaseStage::updateTime() {
	const auto nowTime = timer->getNowTime();	// 今の時間
	const float speed = musicBpm * 2/*/2*/;		// beat / min 毎分何拍か(表裏)
	const auto oneBeatTime = timer->getOneBeatTime(speed);	// 一拍の時間を記録

	constexpr float aheadJudgeRange = 0.25f;	// この値分次の拍の始まりが速くなる[0~1]

	// 拍が変わったら
	if (nowTime >= oneBeatTime * (float)(beatChangeNum - missNum + 1u) - oneBeatTime * aheadJudgeRange) {
		changeBeatProc(nowTime);
	}

	const float beatRaito = (nowTime - beatChangeTime) / (float)oneBeatTime;	// 今の拍の進行度[0~1]
	constexpr float movableRaitoMin = 0.625f, movableRaitoMax = 1.f - aheadJudgeRange;	// 移動できない時間の範囲

	// この範囲内なら移動はできない
	//movableFlag = !(movableRaitoMin < beatRaito&& beatRaito < movableRaitoMax);

	// --------------------
	// タイミングを示す円の大きさ更新
	// --------------------
	constexpr float circleScaleMin = 0.3f;
	float circleScale = beatRaito * (1.f - circleScaleMin) + circleScaleMin;
	if (!frontBeatFlag) circleScale = 1.f - circleScale, circleScale = 0;

	circleSprite->size = XMFLOAT2(circleSprite->texSize.x * circleScale,
								  circleSprite->texSize.y * circleScale);

	constexpr float circleColMax = 0.5f;
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

		// 追加
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

	// コンボ数に応じて派手にする[0~20]
	particleNum += (particleNumMax - particleNumMin) * nowComboRaito;
	startScale += (scaleMax - scaleMin) * nowComboRaito;

	createParticle(pos, particleNum, startScale);

	// SE鳴らす
	Sound::SoundPlayWave(soundCommon.get(), particleSE.get());
}



std::vector<std::vector<std::string>> BaseStage::loadCsv(const std::string &csvFilePath) {
	std::vector<std::vector<std::string>> csvData{};	// csvの中身を格納

	std::ifstream ifs(csvFilePath);
	if (!ifs) {
		return csvData;
	}

	std::string line{};
	// 開いたファイルを一行読み込む(カーソルも動く)
	for (unsigned i = 0U; std::getline(ifs, line); i++) {
		csvData.emplace_back();

		std::istringstream stream(line);
		std::string field;
		// 読み込んだ行を','区切りで分割
		while (std::getline(stream, field, ',')) {
			csvData[i].emplace_back(field);
		}
	}

	return csvData;
}

void BaseStage::loadMapFile(const std::string &csvFilePath, DirectX::XMFLOAT2 *startPosition) {
	const auto mapFileData = loadCsv(csvFilePath);

	bool findStartPosFlag = false;
	XMFLOAT2 startPos{};

	for (UINT y = 0, yLen = mapFileData.size(); y < yLen; y++) {
		mapData.emplace_back();

		for (UINT x = 0, xLen = mapFileData[y].size(); x < xLen; x++) {
			mapData[y].emplace_back();

			std::string chip = mapFileData[y][x];
			if (chip == "1") {
				mapData[y][x] = MAP_NUM::WALL;
			} else if (chip == "2") {
				mapData[y][x] = MAP_NUM::FRONT_ROAD;
				if (!findStartPosFlag) {
					startPos = XMFLOAT2(x, y);
					findStartPosFlag = true;
				}
			} else if (chip == "3") {
				mapData[y][x] = MAP_NUM::BACK_ROAD;
				if (!findStartPosFlag) {
					startPos = XMFLOAT2(x, y);
					findStartPosFlag = true;
				}
			} else if (chip == "4") {
				mapData[y][x] = MAP_NUM::GOAL;
			} else {
				mapData[y][x] = MAP_NUM::UNDEF;
			}
		}
	}

	if (startPosition != nullptr) *startPosition = startPos;
}


void BaseStage::init() {
	pathInit();

#pragma region シングルトンインスタンス
	dxCom = DirectXCommon::getInstance();

	input = Input::getInstance();
#pragma endregion シングルトンインスタンス

#pragma region ビュー変換

	camera.reset(new Camera(WinAPI::window_width, WinAPI::window_height));
	camera->setEye(XMFLOAT3(0, 0, -175));	// 視点座標
	camera->setTarget(XMFLOAT3(0, 0, 0));	// 注視点座標
	camera->setUp(XMFLOAT3(0, 1, 0));		// 上方向

#pragma endregion ビュー変換

#pragma region スプライト
	spriteCommon = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
											  WinAPI::window_width, WinAPI::window_height);

#pragma region デバッグテキスト
	// デバッグテキスト用のテクスチャ読み込み
	Sprite::commonLoadTexture(spriteCommon,
							  debugTextTexNumber,
							  L"Resources/debugfont.png",
							  DirectXCommon::getInstance()->getDev());

	debugText.Initialize(dxCom->getDev(),
						 WinAPI::window_width, WinAPI::window_height,
						 debugTextTexNumber,
						 spriteCommon);
#pragma endregion デバッグテキスト

#pragma region タイミング円
	constexpr UINT circleTexNum = 0;
	Sprite::commonLoadTexture(spriteCommon,
							  circleTexNum,
							  L"Resources/circle.png",
							  DirectXCommon::getInstance()->getDev());

	circleSprite.reset(new Sprite());
	circleSprite->create(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height,
						 circleTexNum, spriteCommon);
	circleSprite->position.x = WinAPI::window_width / 2.f;
	circleSprite->position.y = WinAPI::window_height / 2.f;

	circleSprite->color = XMFLOAT4(0.25f, 1.f, 0.25f, 1.f);
#pragma endregion タイミング円

#pragma region 失敗時の赤画面
	constexpr UINT redTexNum = circleTexNum + 1;
	Sprite::commonLoadTexture(spriteCommon,
							  redTexNum,
							  L"Resources/red.png",
							  DirectXCommon::getInstance()->getDev());

	red.reset(new Sprite());
	red->create(DirectXCommon::getInstance()->getDev(),
				WinAPI::window_width, WinAPI::window_height,
				redTexNum,
				spriteCommon,
				{ 0, 0 });
	red->size = XMFLOAT2(WinAPI::window_width, WinAPI::window_height);
	red->color.w = 0.5f;
	red->isInvisible = true;


	// 一分/BPM即ち一拍の時間(表裏拍のセット一つ分)
	redTime = 60 * Time::oneSec / musicBpm;

	redTimer.reset(new Time());
#pragma endregion 失敗時の赤画面

#pragma region 時間表す棒
	constexpr UINT barTexNum = redTexNum + 1;
	Sprite::commonLoadTexture(spriteCommon,
							  barTexNum,
							  L"Resources/timeBar.png",
							  DirectXCommon::getInstance()->getDev());

	timeBarSprite.reset(new Sprite());
	timeBarSprite->create(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height,
						  barTexNum, spriteCommon,
						  { 0.5f, 0.f });

	timeBarSprite->size.y = WinAPI::window_height / 20.f;
	timeBarSprite->size.x = timeBarWid;

	timeBarSprite->position.x = WinAPI::window_width / 2.f;
	timeBarSprite->position.y = timeBarSprite->size.y / 2.f + debugText.fontHeight;


	timeBarSprite->SpriteTransferVertexBuffer(spriteCommon);
#pragma endregion 時間表す棒

#pragma region ビネッタ
	constexpr UINT vignTexNum = barTexNum + 1;
	Sprite::commonLoadTexture(spriteCommon, vignTexNum,
							  L"Resources/vign.png", dxCom->getDev());
	vign.reset(new Sprite());
	vign->create(dxCom->getDev(), WinAPI::window_width, WinAPI::window_height,
				 vignTexNum, spriteCommon, { 0, 0 });
	vign->size.x = WinAPI::window_width;
	vign->size.y = WinAPI::window_height;
	vign->SpriteTransferVertexBuffer(spriteCommon);

#pragma endregion ビネッタ

#pragma endregion スプライト

#pragma region マップファイル読み込み

	loadMapFile(mapCSVFilePath, &playerMapPos);

#pragma endregion マップファイル読み込み

#pragma region オブジェクト3D

#pragma region マップ

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
	backObj->rotation.x = 90.f;

	model.reset(new Model(DirectXCommon::getInstance()->getDev(),
						  boxModelPath.c_str(), boxModelTexPath_wall.c_str(),
						  WinAPI::window_width, WinAPI::window_height,
						  Object3d::constantBufferNum, BOX_TEXNUM::WALL));
	model->loadTexture(dxCom->getDev(), boxModelTexPath_wall.c_str(), BOX_TEXNUM::WALL);
	model->loadTexture(dxCom->getDev(), boxModelTexPath_front.c_str(), BOX_TEXNUM::FRONT);
	model->loadTexture(dxCom->getDev(), boxModelTexPath_back.c_str(), BOX_TEXNUM::BACK);
	model->loadTexture(dxCom->getDev(), boxModelTexPath_goal.c_str(), BOX_TEXNUM::GOAL);

	// ゴールのモデルの宣言
	constexpr UINT goalTexNum = 0;
	goalModel.reset(new Model(dxCom->getDev(),
							  goalModelPath.c_str(), goalModelTexPath.c_str(),
							  WinAPI::window_width, WinAPI::window_height,
							  Object3d::constantBufferNum, goalTexNum));
	constexpr XMFLOAT3 goalObjScale = XMFLOAT3(2.5, 2.5, 2.5);
	constexpr XMFLOAT3 goalObjRotation = XMFLOAT3(0, 90, 90);

#pragma endregion マップ

#pragma region 迷路

	constexpr XMFLOAT4 backRoadCol = XMFLOAT4(1, 0, 1, 1);
	constexpr XMFLOAT4 frontRoadCol = XMFLOAT4(0, 1, 1, 1);
	constexpr XMFLOAT4 goalCol = XMFLOAT4(1, 0, 0, 1);

	constexpr float mapPosY = -150.f;
	floorPosY = mapPosY;

	mapObj.reserve(mapData.size());
	for (UINT y = 0, loopLenY = mapData.size(); y < loopLenY; ++y) {
		mapObj.emplace_back();
		mapObj[y].reserve(mapData[y].size());
		for (UINT x = 0, loopLenX = mapData[y].size(); x < loopLenX; ++x) {
			mapObj[y].emplace_back(Object3d(DirectXCommon::getInstance()->getDev(), model.get(), BOX_TEXNUM::WALL));
			mapObj[y][x].scale = XMFLOAT3(obj3dScale, obj3dScale, obj3dScale);
			mapObj[y][x].position = XMFLOAT3(x * mapSide,
											 mapPosY,
											 y * -mapSide);

			switch (mapData[y][x]) {
			case MAP_NUM::WALL:
				mapObj[y][x].position.y += obj3dScale;
				mapObj[y][x].texNum = BOX_TEXNUM::WALL;

				if (y == 0 || x == 0) {
					/*再外周の左と上*/
				} else if (x == loopLenX - 1 || y == loopLenY - 1) {
					/*再外周の右と下*/
				} else {
					/*その他の壁*/
					mapObj[y][x].color = noRoadWallCol;
				}
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
				// todo 此処でゴールのオブジェクトの座標を設定する
				goalObj.emplace_back(Object3d(dxCom->getDev(), goalModel.get(), goalTexNum));
				{
					auto &lastGoal = goalObj.back();
					lastGoal.position = mapObj[y][x].position;
					lastGoal.position.y += obj3dScale;
					lastGoal.scale = goalObjScale;

					lastGoal.rotation = goalObjRotation;
				}
				break;
			default:
				mapObj[y][x].color = XMFLOAT4(0, 0, 1, 1);
				break;
			}
		}
	}

#pragma endregion 迷路

#pragma region プレイヤー

	playerModel.reset(new Model(DirectXCommon::getInstance()->getDev(),
								playerModelPath.c_str(), playerModelTexPath.c_str(),
								WinAPI::window_width, WinAPI::window_height,
								Object3d::constantBufferNum, 0));

	playerObj.reset(new Object3d(dxCom->getDev(), playerModel.get(), 0));

	playerScale = obj3dScale * 0.4f;

	playerObj->scale = { playerScale, playerScale, playerScale };

	playerObj->position = mapObj[playerMapPos.y][playerMapPos.x].position;
	playerObj->position.y += mapSide;

	playerObj->rotation.y += 90.f;
	playerObj->rotation.z += 90.f;
	//playerObj->scale = XMFLOAT3(0.5f, 0.5f, 0.5f);

	easeStartPos = playerObj->position;
	easeEndPos = playerObj->position;

#pragma endregion プレイヤー

	// 進めない道を壁にする
	updateMovableRoad();

#pragma endregion オブジェクト3D

#pragma region ライト

	updateLightPosition();

#pragma endregion ライト

#pragma region パーティクル

	// パーティクル初期化
	particleMgr.reset(new ParticleManager(dxCom->getDev(), effectTexPath.c_str(), camera.get()));
#pragma endregion パーティクル

#pragma region 音

	soundCommon.reset(new Sound::SoundCommon());
	bgm.reset(new Sound(bgmFilePath.c_str(), soundCommon.get()));

	particleSE.reset(new Sound(particleSeFilePath.c_str(), soundCommon.get()));

#pragma endregion 音

	additionalInit();

	// BGM再生
	Sound::SoundPlayWave(soundCommon.get(), bgm.get(), XAUDIO2_LOOP_INFINITE, bgmBolume);

	// 時間初期化
	easeTime.reset(new Time());
	timer.reset(new Time());
}

void BaseStage::update() {
	// ステージ選択画面に戻る操作
	if (input->hitKey(DIK_LSHIFT) && input->hitKey(DIK_R)) SceneManager::getInstance()->changeScene(SCENE_NUM::SELECT);

	// 天球回転
	backObj->rotation.y += 0.1f;

	// カメラの座標を更新
	updateCamera();

	// 時間関係の更新
	updateTime();

	// プレイヤーの座標を更新
	updatePlayerPos();

	// パーティクル開始
	if (createParticleFlag) {
		startParticle(playerObj->position);

		createParticleFlag = false;
	}

	// ライトの位置更新
	updateLightPosition();

	additionalUpdate();

#pragma region 情報表示

	constexpr XMFLOAT4 dbFontCol = XMFLOAT4(1, 1, 1, 1);
	constexpr XMFLOAT4 shadowCol = XMFLOAT4(0, 0, 0, 1);

	debugText.formatPrint(spriteCommon, 1, 1, 1, shadowCol, "Stage %u", stageNum);
	debugText.formatPrint(spriteCommon, 0, 0, 1, dbFontCol, "Stage %u", stageNum);

	debugText.Print(spriteCommon, "LSHIFT + R : Return SELECT",
					1, debugText.fontHeight * 3 + 1,
					1.f, shadowCol);
	debugText.Print(spriteCommon, "LSHIFT + R : Return SELECT",
					0, debugText.fontHeight * 3);

	debugText.formatPrint(spriteCommon, 1, debugText.fontHeight * 5 + 1, 1,
						  shadowCol,
						  "%u combo", combo);
	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 5, 1,
						  dbFontCol,
						  "%u combo", combo);

	const auto timeLimit = clearCount - beatChangeNum;
	const float raito = (float)timeLimit / clearCount;

	debugText.formatPrint(spriteCommon, 1, debugText.fontHeight * 6 + 1, 1,
						  shadowCol,
						  "%u / %u",
						  clearCount - beatChangeNum, clearCount);
	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 6, 1,
						  XMFLOAT4(1, raito, raito, 1),
						  "%u / %u",
						  clearCount - beatChangeNum, clearCount);

	timeBarSprite->size.x = timeBarWid * raito;

	timeBarSprite->SpriteTransferVertexBuffer(spriteCommon);

	debugText.Print(spriteCommon, "TIME",
					timeBarSprite->position.x - debugText.fontWidth * 2.f + 1,
					timeBarSprite->position.y - debugText.fontHeight + 1,
					1.f,
					shadowCol);
	debugText.Print(spriteCommon, "TIME",
					timeBarSprite->position.x - debugText.fontWidth * 2.f,
					timeBarSprite->position.y - debugText.fontHeight,
					1.f,
					dbFontCol);


#pragma endregion 情報表示

	// 制限時間
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

	// 壁描画
	for (auto &y : mapObj) {
		for (auto &x : y) {
			x.drawWithUpdate(camera->getViewMatrix(), dxCom);
		}
	}

	// 自機描画
	playerObj->drawWithUpdate(camera->getViewMatrix(), dxCom);

	// ゴールモデル描画
	for (auto &i : goalObj) {
		i.drawWithUpdate(camera->getViewMatrix(), dxCom);
	}

	// その他の描画指定があれば描画
	additionalDrawObj3d();

	// 背景描画
	Object3d::startDraw(DirectXCommon::getInstance()->getCmdList(), backPipelineSet);
	backObj->drawWithUpdate(camera->getViewMatrix(), dxCom);
}

void BaseStage::drawParticle() {
	ParticleManager::startDraw(dxCom->getCmdList(), object3dPipelineSet);

	particleMgr->drawWithUpdate(dxCom->getCmdList(), playerObj->position);

	additionalDrawParticle();
}

void BaseStage::drawSprite() {
	Sprite::drawStart(spriteCommon, dxCom->getCmdList());
	circleSprite->drawWithUpdate(dxCom, spriteCommon);
	red->drawWithUpdate(dxCom, spriteCommon);
	timeBarSprite->drawWithUpdate(dxCom, spriteCommon);
	// スプライト描画
	additionalDrawBackSprite();

	additionalDrawFrontprite();
	// デバッグテキスト描画
	debugText.DrawAll(dxCom, spriteCommon);
	vign->drawWithUpdate(dxCom, spriteCommon);
}