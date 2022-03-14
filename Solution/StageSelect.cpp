#include "StageSelect.h"

#include "SceneManager.h"

#include "WinAPI.h"

void StageSelect::init() {
	input = Input::getInstance();

	// --------------------
	// �X�v���C�g����
	// --------------------
	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height);

	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	Sprite::commonLoadTexture(spCom, TEX_NUM::EX, L"Resources/StageSelect/Explanation1.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, TEX_NUM::P1, L"Resources/StageSelect/Play1.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, TEX_NUM::P2, L"Resources/StageSelect/Play2.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, TEX_NUM::P3, L"Resources/StageSelect/Play3.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, TEX_NUM::EXn, L"Resources/StageSelect/Explanation2.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, TEX_NUM::P1n, L"Resources/StageSelect/Play1n.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, TEX_NUM::P2n, L"Resources/StageSelect/Play2n.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, TEX_NUM::P3n, L"Resources/StageSelect/Play3n.png", DirectXCommon::getInstance()->getDev());

	// �X�v���C�g�̐���
	sprites[0].create(
		DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		TEX_NUM::EX, spCom, { 0,0 }, false, false
	);
	sprites[1].create(
		DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		TEX_NUM::P1n, spCom, { 0,0 }, false, false
	);
	sprites[2].create(
		DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		TEX_NUM::P2n, spCom, { 0,0 }, false, false
	);
	sprites[3].create(
		DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		TEX_NUM::P3n, spCom, { 0,0 }, false, false
	);
	// �X�v���C�g�̍��W�ύX
	for (UINT i = 0, size = _countof(sprites); i < size; ++i) {
		sprites[i].position.x = -190 + (350 * i);
		sprites[i].position.y = 426;
		// ���_�o�b�t�@�ɔ��f
		sprites[i].SpriteTransferVertexBuffer(spCom);
	}
	sprites[0].position.x = 510;
	sprites[0].position.y = 110;

	// 3D�I�u�W�F�N�g�p�p�C�v���C������
	object3dPipelineSet = Object3d::createGraphicsPipeline(DirectXCommon::getInstance()->getDev());


	/*spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height);*/

		// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	Sprite::commonLoadTexture(spCom, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());

	debugText.Initialize(DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		debugTextTexNumber, spCom);
}

void StageSelect::update() {
	if (input->triggerKey(DIK_R)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::TITLE);
	}

	constexpr auto up = DIK_W;
	constexpr auto down = DIK_S;
	constexpr auto right = DIK_D;
	constexpr auto left = DIK_A;

	switch (SELECT) {
	case STAGE_SELECT::EXPLANATION:
		if (input->triggerKey(down)) {
			SELECT = STAGE_SELECT::STAGE1;
			sprites[0].texNumber = TEX_NUM::EXn;
			sprites[1].texNumber = TEX_NUM::P1;
		} else if (input->triggerKey(DIK_SPACE)) {
			SceneManager::getInstange()->changeScene(SCENE_NUM::EXPLANATION);
		}
		break;

	case STAGE_SELECT::STAGE1:
		if (input->triggerKey(right)) {
			SELECT = STAGE_SELECT::STAGE2;
			sprites[1].texNumber = TEX_NUM::P1n;
			sprites[2].texNumber = TEX_NUM::P2;
		} else if (input->triggerKey(left)) {
			SELECT = STAGE_SELECT::STAGE3;
			sprites[1].texNumber = TEX_NUM::P1n;
			sprites[3].texNumber = TEX_NUM::P3;
		} else if (input->triggerKey(up)) {
			SELECT = STAGE_SELECT::EXPLANATION;
			sprites[1].texNumber = TEX_NUM::P1n;
			sprites[0].texNumber = TEX_NUM::EX;
		} else if (input->triggerKey(DIK_SPACE)) {
			SceneManager::getInstange()->changeScene(SCENE_NUM::PLAY);
		}
		break;

	case STAGE_SELECT::STAGE2:
		if (input->triggerKey(right)) {
			SELECT = STAGE_SELECT::STAGE3;
			sprites[2].texNumber = TEX_NUM::P2n;
			sprites[3].texNumber = TEX_NUM::P3;
		} else if (input->triggerKey(left)) {
			SELECT = STAGE_SELECT::STAGE1;
			sprites[2].texNumber = TEX_NUM::P2n;
			sprites[1].texNumber = TEX_NUM::P1;
		} else if (input->triggerKey(up)) {
			SELECT = STAGE_SELECT::EXPLANATION;
			sprites[2].texNumber = TEX_NUM::P2n;
			sprites[0].texNumber = TEX_NUM::EX;
		}
		break;

	case STAGE_SELECT::STAGE3:
		if (input->triggerKey(right)) {
			SELECT = STAGE_SELECT::STAGE1;
			sprites[3].texNumber = TEX_NUM::P3n;
			sprites[1].texNumber = TEX_NUM::P1;
		} else if (input->triggerKey(left)) {
			SELECT = STAGE_SELECT::STAGE2;
			sprites[3].texNumber = TEX_NUM::P3n;
			sprites[2].texNumber = TEX_NUM::P2;
		} else if (input->triggerKey(up)) {
			SELECT = STAGE_SELECT::EXPLANATION;
			sprites[3].texNumber = TEX_NUM::P3n;
			sprites[0].texNumber = TEX_NUM::EX;
		}
		break;

	default:
		break;
	}

	// todo �I�񂾃V�[�����ƂɑJ�ڃV�[���𕪂���(changeScene�̈�����SELECT�ɂ��� <- �e�X�e�[�̃N���X���������Ă���)
	if (input->triggerKey(DIK_RETURN)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::PLAY);
	}
	debugText.Print(spCom, "STAGE SELECT\n\nWASD : SELECT\nENTER : PLAY\nSPACE : EXPLANATION", 0, 0);
}

void StageSelect::draw() {
	// �X�v���C�g���ʃR�}���h
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	// �X�v���C�g�`��
	for (UINT i = 0, size = _countof(sprites); i < size; ++i) {
		sprites[i].drawWithUpdate(DirectXCommon::getInstance(), spCom);
	}
	// �f�o�b�O�e�L�X�g�`��
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}
