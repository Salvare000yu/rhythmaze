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

	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	Sprite::commonLoadTexture(spCom, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());

	debugText.Initialize(DirectXCommon::getInstance()->getDev(),
						 WinAPI::window_width, WinAPI::window_height,
						 debugTextTexNumber, spCom);

	// �C�[�W���O
	startEaseFlag = true;
	endEaseFlag = false;
	easeTimer.reset(new Time());


	// ��
	soundCom.reset(new Sound::SoundCommon());
	constexpr char titleBgmPath[] = "Resources/Music/BGM.wav";
	bgm.reset(new Sound(titleBgmPath, soundCom.get()));

	sceneChangeSe.reset(new Sound("Resources/SE/Shortbridge29-1.wav", soundCom.get()));

	// --- BGM�Đ�
	Sound::SoundPlayWave(soundCom.get(), bgm.get(), XAUDIO2_LOOP_INFINITE);
	// --- �C�[�W���O�p�^�C�}�[�J�n(����0�Ƃ���)
	easeTimer->reset();
}

void TitleScene::update() {

	// �J�n���C�[�W���O���Ȃ�
	if (startEaseFlag) {
		// �C�[�W���O�̎���
		constexpr auto startEaseTime = Time::oneSec * 1.5;
		// �C�[�W���O�i�s�x(0~1)
		const auto raito = (float)easeTimer->getNowTime() / startEaseTime;

		if (raito >= 1.f) {
			// �w�莞�Ԃ𒴂��Ă�����C�[�W���O�I��
			startEaseFlag = false;
			titleSprite->position.y = 0;
		} else {
			// �w�莞�Ԃ��o�߂��Ă��Ȃ���΃C�[�W���O����

			const auto easeRaito = easeOutBounce(raito);
			const auto easePos = easeRaito * titleSprite->size.y - titleSprite->size.y;

			titleSprite->position.y = easePos;

		}
		titleSprite->SpriteTransferVertexBuffer(spCom);
	} else {
		if (endEaseFlag) {
			// �I�����C�[�W���O���I��������

			// �C�[�W���O���鎞��
			constexpr auto endEaseTime = Time::oneSec * 0.75;
			// �C�[�W���O�i�s�x(0~1)
			const auto raito = (float)easeTimer->getNowTime() / endEaseTime;

			if (raito >= 1.f && !Sound::checkPlaySound(sceneChangeSe.get())) {
				// �w�莞�Ԉȏォ��SE���Đ��I�����Ă����Ȃ�V�[���J��
				endEaseFlag = false;
				SceneManager::getInstange()->changeScene(SCENE_NUM::SELECT);
			} else {
				// �w�莞�Ԗ����Ȃ�C�[�W���O����
				const auto easeRaito = pow(raito, 3);
				const auto easePos = easeRaito * titleSprite->size.y;

				titleSprite->position.y = easePos;
				titleSprite->SpriteTransferVertexBuffer(spCom);
			}
		} else {
			// �I�����C�[�W���O���I�����Ă��Ȃ����

			// �X�y�[�X����������I�����C�[�W���O���J�n����
			if (input->triggerKey(DIK_SPACE)) {
				endEaseFlag = true;
				// BGM���~�߂�
				Sound::SoundStopWave(bgm.get());
				// �����Ō��ʉ��𗬂�
				constexpr float sceneChangeSeVolume = 0.3f;
				Sound::SoundPlayWave(soundCom.get(), sceneChangeSe.get(), sceneChangeSeVolume);
				// �C�[�W���O�p�^�C�}�[������
				easeTimer->reset();
			}
			// �܂������Ă��Ȃ���Α���𑣂����b�Z�[�W��\��
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
