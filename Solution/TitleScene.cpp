#include "TitleScene.h"

#include "SceneManager.h"

#include "WinAPI.h"

namespace {

	inline float easeOutBounce(float t) {
		constexpr float n1 = 7.5625f;
		constexpr float d1 = 2.75f;

		if (t < 1.f / d1) {
			return n1 * t * t;
		} else if (t < 2.f / d1) {
			return n1 * (t -= 1.5 / d1) * t + 0.75f;
		} else if (t < 2.5f / d1) {
			return n1 * (t -= 2.25f / d1) * t + 0.9375f;
		} else {
			return n1 * (t -= 2.625f / d1) * t + 0.984375f;
		}
	}

}

void TitleScene::init() {
	input = Input::getInstance();

	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
									   WinAPI::window_width, WinAPI::window_height);

	constexpr UINT titleSpriteNum = 0;
	Sprite::commonLoadTexture(spCom, 0, L"Resources/backSprite/title.png", DirectXCommon::getInstance()->getDev());

	titleSprite.reset(new Sprite());
	titleSprite->create(DirectXCommon::getInstance()->getDev(),
						WinAPI::window_width, WinAPI::window_height,
						titleSpriteNum, spCom, DirectX::XMFLOAT2(0, 0), false, false);

	// デバッグテキスト用のテクスチャ読み込み
	Sprite::commonLoadTexture(spCom, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());

	debugText.Initialize(DirectXCommon::getInstance()->getDev(),
						 WinAPI::window_width, WinAPI::window_height,
						 debugTextTexNumber, spCom);

	// イージング
	startEaseFlag = true;
	endEaseFlag = false;
	easeTimer.reset(new Time());


	// 音
	soundCom.reset(new Sound::SoundCommon());
	constexpr char titleBgmPath[] = "Resources/Music/BGM.wav";
	bgm.reset(new Sound(titleBgmPath, soundCom.get()));

	sceneChangeSe.reset(new Sound("Resources/SE/Shortbridge29-1.wav", soundCom.get()));

	// --- BGM再生
	Sound::SoundPlayWave(soundCom.get(), bgm.get(), XAUDIO2_LOOP_INFINITE);
	// --- イージング用タイマー開始(今を0とする)
	easeTimer->reset();
}

void TitleScene::update() {

	// 開始時イージング中なら
	if (startEaseFlag) {
		// イージングの時間
		constexpr auto startEaseTime = Time::oneSec * 1.5;
		// イージング進行度(0~1)
		const auto raito = (float)easeTimer->getNowTime() / startEaseTime;

		if (raito >= 1.f) {
			// 指定時間を超えていたらイージング終了
			startEaseFlag = false;
			titleSprite->position.y = 0;
		} else {
			// 指定時間が経過していなければイージング処理

			const auto easeRaito = easeOutBounce(raito);
			const auto easePos = easeRaito * titleSprite->size.y - titleSprite->size.y;

			titleSprite->position.y = easePos;

		}
		titleSprite->SpriteTransferVertexBuffer(spCom);
	} else {
		if (endEaseFlag) {
			// 終了時イージングが終了したら

			// イージングする時間
			constexpr auto endEaseTime = Time::oneSec * 0.75;
			// イージング進行度(0~1)
			const auto raito = (float)easeTimer->getNowTime() / endEaseTime;

			if (raito >= 1.f && !Sound::checkPlaySound(sceneChangeSe.get())) {
				// 指定時間以上かつSEが再生終了していたならシーン遷移
				endEaseFlag = false;
				SceneManager::getInstange()->changeScene(SCENE_NUM::SELECT);
			} else {
				// 指定時間未満ならイージング処理
				const auto easeRaito = pow(raito, 3);
				const auto easePos = easeRaito * titleSprite->size.y;

				titleSprite->position.y = easePos;
				titleSprite->SpriteTransferVertexBuffer(spCom);
			}
		} else {
			// 終了時イージングが終了していなければ

			// スペースを押したら終了時イージングを開始する
			if (input->triggerKey(DIK_SPACE)) {
				endEaseFlag = true;
				// BGMを止める
				Sound::SoundStopWave(bgm.get());
				// ここで効果音を流す
				constexpr float sceneChangeSeVolume = 0.3f;
				Sound::SoundPlayWave(soundCom.get(), sceneChangeSe.get(), sceneChangeSeVolume);
				// イージング用タイマー初期化
				easeTimer->reset();
			}
			// まだ押していなければ操作を促すメッセージを表示
			debugText.Print(spCom, "Press SPACE...",
							WinAPI::window_width * 0.5f - debugText.fontWidth * 7,
							WinAPI::window_height * 0.75);
		}
	}
}

void TitleScene::draw() {
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	titleSprite->drawWithUpdate(DirectXCommon::getInstance(), spCom);
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}
