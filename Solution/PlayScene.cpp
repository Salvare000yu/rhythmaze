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

	// @return 読み込んだcsvの中身。失敗したらデフォルトコンストラクタで初期化された空のvector2次元配列が返る
	std::vector<std::vector<std::string>> loadCsv(const std::string& csvFilePath) {
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

		// 0 ~ PI/2がわかれば求められる
		if (x < XM_PIDIV2) {
			// そのまま
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
	WinAPI::getInstance()->setWindowText("リズメイズ (SE : OtoLogic)");

	dxCom = DirectXCommon::getInstance();

	input = Input::getInstance();

#pragma region ビュー変換

	camera.reset(new Camera(WinAPI::window_width, WinAPI::window_height));
	camera->setEye(XMFLOAT3(0, 0, -175));	// 視点座標
	camera->setTarget(XMFLOAT3(0, 0, 0));	// 注視点座標
	camera->setUp(XMFLOAT3(0, 1, 0));		// 上方向

#pragma endregion ビュー変換

#pragma region 音

	soundCommon.reset(new Sound::SoundCommon());
	bgm.reset(new Sound("Resources/Music/mmc_125_BGM2.wav", soundCommon.get()));

	particleSE.reset(new Sound("Resources/SE/Sys_Set03-click.wav", soundCommon.get()));

#pragma endregion 音

#pragma region スプライト

	// --------------------
	// スプライト共通
	// --------------------
	spriteCommon = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height);

	// スプライト共通テクスチャ読み込み
	Sprite::commonLoadTexture(spriteCommon, TEX_NUM::TEX1, L"Resources/texture.png", DirectXCommon::getInstance()->getDev());

	// スプライトの生成
	for (int i = 0; i < _countof(sprites); i++) {
		sprites[i].create(
			DirectXCommon::getInstance()->getDev(),
			WinAPI::window_width, WinAPI::window_height,
			TEX_NUM::TEX1, spriteCommon, { 0,0 }, false, false
		);
		// スプライトの座標変更
		sprites[i].position.x = 1280 / 10;
		sprites[i].position.y = 720 / 10;
		//sprites[i].isInvisible = true;
		//sprites[i].position.x = (float)(rand() % 1280);
		//sprites[i].position.y = (float)(rand() % 720);
		//sprites[i].rotation = (float)(rand() % 360);
		//sprites[i].rotation = 0;
		//sprites[i].size.x = 400.0f;
		//sprites[i].size.y = 100.0f;
		// 頂点バッファに反映
		sprites[i].SpriteTransferVertexBuffer(spriteCommon);
	}

	// デバッグテキスト用のテクスチャ読み込み
	Sprite::commonLoadTexture(spriteCommon, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());
	// デバッグテキスト初期化
	debugText.Initialize(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height, debugTextTexNumber, spriteCommon);

	// 3Dオブジェクト用パイプライン生成
	object3dPipelineSet = Object3d::createGraphicsPipeline(DirectXCommon::getInstance()->getDev());


#pragma endregion スプライト

#pragma region マップ

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

#pragma endregion マップ

#pragma region 3Dオブジェクト

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

#pragma endregion 3Dオブジェクト

#pragma region ライト

	light = mapObj[0][0].position;
	light.x += mapObj[0][0].scale.y * 2;	// 仮

#pragma endregion ライト


	// パーティクル初期化
	particleMgr.reset(new ParticleManager(dxCom->getDev(), L"Resources/effect1.png", camera.get()));

	Sound::SoundPlayWave(soundCommon.get(), bgm.get(), XAUDIO2_LOOP_INFINITE);

	// 時間初期化
	timer.reset(new Time());
}

void PlayScene::update() {

#pragma region カメラ移動回転

	{

		const float rotaVal = XM_PIDIV2 / DirectXCommon::getInstance()->getFPS();	// 毎秒四半周

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

		// angleラジアンだけY軸まわりに回転。半径は-100
		constexpr float camRange = 100.f;	// targetLength
		camera->rotation(camRange, angle.x, angle.y);


		// 移動量
		const float moveSpeed = 75.f / dxCom->getFPS();
		// 視点移動
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

#pragma endregion カメラ移動回転

#pragma region 音タイミング

	{
		const auto nowTime = timer->getNowTime();	// 今の時間
		constexpr float bpm = 125 * 2;	// beat / min 毎分何拍か(表裏)
		const auto oneBeatTime = timer->getOneBeatTime(bpm);	// 一拍の時間を記録

		constexpr float aheadJudgeRange = 0.25f;	// この値分次の拍の始まりが速くなる[0~1]

		// 拍が変わったら
		if (nowTime >= oneBeatTime * (beatChangeNum + 1) - oneBeatTime * aheadJudgeRange) {
			beatChangeNum++;
			beatChangeTime = nowTime;	// 今の時間を記録
			frontBeatFlag = !frontBeatFlag;	// 表迫と裏拍をchange

			if (playerMoved) {
				playerMoved = false;
			} //else combo = 0U;	// 止まっていたらコンボをリセット
		}

		const float beatRaito = (nowTime - beatChangeTime) / (float)oneBeatTime;	// 今の拍の進行度[0~1]
		constexpr float movableRaitoMin = 0.625f, movableRaitoMax = 1.f - aheadJudgeRange;	// 移動できない時間の範囲

		// この範囲内なら移動はできない
		movableFlag = !(movableRaitoMin < beatRaito&& beatRaito < movableRaitoMax);

		debugText.Print(spriteCommon, "O         N\nK         G", 0, debugText.fontHeight,
						1.f, XMFLOAT4(1, 1, 1, 0.5f));

		debugText.Print(spriteCommon,
						movableFlag == true ? "O\nK" : "N\nG",
						beatRaito * 10.f * debugText.fontWidth, debugText.fontHeight,
						1.f,
						XMFLOAT4(1, (float)movableFlag, 1, 1));

	}

#pragma endregion 音タイミング


#pragma region プレイヤー移動
	preMissFlag = missFlag;

	// 移動の入力があったら
	if (input->triggerKey(DIK_UP) || input->triggerKey(DIK_DOWN) ||
		   input->triggerKey(DIK_LEFT) || input->triggerKey(DIK_RIGHT)) {

		// 移動可能なら
		if (playerMoved == false && movableFlag) {

			uint8_t moveDir = 0u;

			// 押した方向を記録
			if (input->triggerKey(DIK_UP)) {
				moveDir = DIK_UP;
			} else if (input->triggerKey(DIK_DOWN)) {
				moveDir = DIK_DOWN;
			} else if (input->triggerKey(DIK_LEFT)) {
				moveDir = DIK_LEFT;
			} else if (input->triggerKey(DIK_RIGHT)) {
				moveDir = DIK_RIGHT;
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

			// 移動可能なら移動する
			if (nextMapNum != MAP_NUM::UNDEF
			   &&
			  (nextMapNum == nowMovableRoad || nextMapNum == MAP_NUM::GOAL)) {
				// 移動したことを記録
				playerMoved = true;
				// 移動後のマップ座標を反映
				playerMapPos = nextPlayerMapPos;
				// 移動後の座標を反映
				playerObj->position = XMFLOAT3(playerMapPos.x * mapSide,
											   playerObj->position.y,
											   playerMapPos.y * -mapSide);

				if (nextMapNum == MAP_NUM::GOAL) {
					SceneManager::getInstange()->changeScene(SCENE_NUM::END);
				}
				// パーティクル開始
				createParticleFlag = true;
				// コンボ数加算
				if (combo < UINT_MAX) combo++;

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
		if (missFlag && preMissFlag) {
			combo = 0U;	// コンボをリセット
			missFlag = false;
			preMissFlag = false;
		}
	}
#pragma endregion プレイヤー移動

#pragma region パーティクル

	// Pを押すたびパーティクル50粒追加
	if (createParticleFlag) {
		constexpr UINT particleNumMax = 50U, particleNumMin = 20U;
		UINT particleNum = particleNumMin;

		constexpr float scaleMin = 1.f, scaleMax = 10.f;
		constexpr UINT maxCombo = 20U;
		float startScale = scaleMin;

		auto nowComboRaito = (float)combo / maxCombo;
		if (nowComboRaito >= 1.f) nowComboRaito = 1.f;
		else if (nowComboRaito <= 0.f) nowComboRaito = 0.f;

		// コンボ数に応じて派手にする[0~20]
		particleNum += (particleNumMax - particleNumMin) * nowComboRaito;
		startScale += (scaleMax - scaleMin) * nowComboRaito;

		createParticle(playerObj->position, particleNum, startScale);

		// SE鳴らす
		Sound::SoundPlayWave(soundCommon.get(), particleSE.get());

		createParticleFlag = false;
	}

#pragma endregion パーティクル

#pragma region ライト
	{

		// プレイヤーの位置にライトを置く
		light = playerObj->position;

		for (UINT y = 0; y < mapData.size(); y++) {
			for (UINT x = 0; x < mapData[y].size(); x++) {
				mapObj[y][x].setLightPos(light);
			}
		}
	}
#pragma endregion ライト



#pragma region 情報表示
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

#pragma endregion 情報表示

	camera->update();
}

void PlayScene::draw() {
	// ４．描画コマンドここから
	// 3Dオブジェクトコマンド
	Object3d::startDraw(dxCom->getCmdList(), object3dPipelineSet);
	for (UINT y = 0; y < mapData.size(); y++) {
		for (UINT x = 0; x < mapData[y].size(); x++) {
			mapObj[y][x].drawWithUpdate(camera->getViewMatrix(), dxCom);
		}
	}
	playerObj->drawWithUpdate(camera->getViewMatrix(), dxCom);

	ParticleManager::startDraw(dxCom->getCmdList(), object3dPipelineSet);
	particleMgr->drawWithUpdate(dxCom->getCmdList());


	// スプライト共通コマンド
	Sprite::drawStart(spriteCommon, dxCom->getCmdList());
	// スプライト描画
	/*for (UINT i = 0; i < _countof(sprites); i++) {
		sprites[i].drawWithUpdate(dxCom, spriteCommon);
	}*/
	// デバッグテキスト描画
	debugText.DrawAll(dxCom, spriteCommon);
	// ４．描画コマンドここまで
}

void PlayScene::fin() {
	//Sound::SoundStopWave(soundData1.get());
}

void PlayScene::createParticle(const DirectX::XMFLOAT3 pos, const UINT particleNum, const float startScale) {
	for (UINT i = 0U; i < particleNum; i++) {

		const float theata = RandomNum::getRandf(0, XM_PIDIV2);
		const float phi = RandomNum::getRandf(0, XM_PI * 2.f);
		const float r = RandomNum::getRandf(0, 5.f);

		// X,Y,Z全て[-2.5f,+2.5f]でランダムに分布
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

		// 追加
		particleMgr->add(timer.get(),
						 life, generatePos, vel, acc,
						 startScale, endScale,
						 startRota, endRota,
						 startCol, endCol);
	}
}