#include "StageSelect.h"

#include "SceneManager.h"

#include "WinAPI.h"

using namespace DirectX;

namespace {
	constexpr XMFLOAT4 selectCol = XMFLOAT4(1, 0, 0, 1);
	constexpr XMFLOAT4 unSelectCol = XMFLOAT4(0, 0, 1, 1);
	constexpr float unSelectScale = 0.75f, selectScale = 1.f;

	XMFLOAT2 operator*(const XMFLOAT2& left, const float right) {
		return XMFLOAT2(left.x * right, left.y * right);
	}
	XMFLOAT2 operator*(const float left, const XMFLOAT2 right) {
		return XMFLOAT2(left * right.x, left * right.y);
	}
	XMFLOAT2 operator*(const XMFLOAT2 left, const XMFLOAT2 right) {
		return XMFLOAT2(left.x * right.x, left.y * right.y);
	}
}

void StageSelect::init() {
	input = Input::getInstance();

	// --------------------
	// �X�v���C�g����
	// --------------------
	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height);

	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	Sprite::commonLoadTexture(spCom, TEX_NUM::EX, L"Resources/StageSelect/select_Explanation.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, TEX_NUM::P1, L"Resources/StageSelect/select_stage1.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, TEX_NUM::P2, L"Resources/StageSelect/select_stage2.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, TEX_NUM::P3, L"Resources/StageSelect/select_stage3.png", DirectXCommon::getInstance()->getDev());
	Sprite::commonLoadTexture(spCom, TEX_NUM::BACK, L"Resources/StageSelect/select_back.png", DirectXCommon::getInstance()->getDev());

	// �X�v���C�g�̐���
	sprites[0].create(
		DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		TEX_NUM::EX, spCom
	);
	sprites[1].create(
		DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		TEX_NUM::P1, spCom
	);
	sprites[2].create(
		DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		TEX_NUM::P2, spCom
	);
	sprites[3].create(
		DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		TEX_NUM::P3, spCom
	);

	// �X�v���C�g�̍��W�ύX
	for (UINT i = 1, size = _countof(sprites); i < size; ++i) {
		sprites[i].position.x = WinAPI::window_width / 3.f * (i - 0.5f);
		sprites[i].position.y = WinAPI::window_height * 0.75f;
		sprites[i].color = unSelectCol;
		sprites[i].size = unSelectScale * sprites[i].texSize;
		// ���_�o�b�t�@�ɔ��f
		sprites[i].SpriteTransferVertexBuffer(spCom);
	}
	sprites[0].position.x = WinAPI::window_width / 2;
	sprites[0].position.y = WinAPI::window_height * 0.25f;
	sprites[0].color = selectCol;
	sprites[0].size = selectScale * sprites[0].texSize;
	sprites[0].SpriteTransferVertexBuffer(spCom);


	// �e�X�e�[�W�摜�̔w�i
	for (UINT i = 0u; i < SPRITES_NUM; i++) {
		selectBack[i].create(
		DirectXCommon::getInstance()->getDev(),
		WinAPI::window_width, WinAPI::window_height,
		TEX_NUM::BACK, spCom
		);
		selectBack[i].position = sprites[i].position;
		selectBack[i].size = sprites[i].size;
		// ���_�o�b�t�@�ɔ��f
		selectBack[i].SpriteTransferVertexBuffer(spCom);
	}

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

	constexpr auto up = DIK_UP;
	constexpr auto down = DIK_DOWN;
	constexpr auto right = DIK_RIGHT;
	constexpr auto left = DIK_LEFT;

	if (input->triggerKey(down) || input->triggerKey(up)
	   || input->triggerKey(right) || input->triggerKey(left)) {

		bool changeFlag = false;
		uint8_t preSelectSpriteNum = 0u, selectSpriteNum = 0u;
		switch (SELECT) {
		case SCENE_NUM::EXPLANATION:
			if (input->triggerKey(down)) {
				SELECT = SCENE_NUM::STAGE1;
				preSelectSpriteNum = 0;
				selectSpriteNum = 1;

				changeFlag = true;
			}
			break;

		case SCENE_NUM::STAGE1:
			if (input->triggerKey(right)) {
				SELECT = SCENE_NUM::STAGE2;
				preSelectSpriteNum = 1;
				selectSpriteNum = 2;

				changeFlag = true;
			} else if (input->triggerKey(left)) {
				SELECT = SCENE_NUM::STAGE3;
				preSelectSpriteNum = 1;
				selectSpriteNum = 3;

				changeFlag = true;
			} else if (input->triggerKey(up)) {
				SELECT = SCENE_NUM::EXPLANATION;
				preSelectSpriteNum = 1;
				selectSpriteNum = 0;

				changeFlag = true;
			}
			break;

		case SCENE_NUM::STAGE2:
			if (input->triggerKey(right)) {
				SELECT = SCENE_NUM::STAGE3;
				preSelectSpriteNum = 2;
				selectSpriteNum = 3;

				changeFlag = true;
			} else if (input->triggerKey(left)) {
				SELECT = SCENE_NUM::STAGE1;
				preSelectSpriteNum = 2;
				selectSpriteNum = 1;

				changeFlag = true;
			} else if (input->triggerKey(up)) {
				SELECT = SCENE_NUM::EXPLANATION;
				preSelectSpriteNum = 2;
				selectSpriteNum = 0;

				changeFlag = true;
			}
			break;

		case SCENE_NUM::STAGE3:
			if (input->triggerKey(right)) {
				SELECT = SCENE_NUM::STAGE1;
				preSelectSpriteNum = 3;
				selectSpriteNum = 1;

				changeFlag = true;
			} else if (input->triggerKey(left)) {
				SELECT = SCENE_NUM::STAGE2;
				preSelectSpriteNum = 3;
				selectSpriteNum = 2;

				changeFlag = true;
			} else if (input->triggerKey(up)) {
				SELECT = SCENE_NUM::EXPLANATION;
				preSelectSpriteNum = 3;
				selectSpriteNum = 0;

				changeFlag = true;
			}
			break;

		default:
			break;
		}

		if (changeFlag) {
			auto& preSel = sprites[preSelectSpriteNum];
			auto& nowSel = sprites[selectSpriteNum];

			preSel.color = unSelectCol;
			preSel.size = unSelectScale * preSel.texSize;
			nowSel.color = selectCol;
			nowSel.size = selectScale * nowSel.texSize;

			// ���_�o�b�t�@�ɔ��f
			preSel.SpriteTransferVertexBuffer(spCom);
			nowSel.SpriteTransferVertexBuffer(spCom);

			selectBack[preSelectSpriteNum].size = preSel.size;
			selectBack[selectSpriteNum].size = nowSel.size;

			selectBack[preSelectSpriteNum].SpriteTransferVertexBuffer(spCom);
			selectBack[selectSpriteNum].SpriteTransferVertexBuffer(spCom);
		}
	} else {
	// �X�y�[�X����������I�񂾃V�[���Ɉړ�
		if (input->triggerKey(DIK_SPACE)) {
			SceneManager::getInstange()->changeScene(SELECT);
		}
	}


	// todo STAGE4���ق��̃X�e�[�W���l�ɑI�ׂ�悤�ɂ���
	if (input->triggerKey(DIK_4)) SceneManager::getInstange()->changeScene(SCENE_NUM::STAGE4);

	debugText.Print(spCom, "STAGE SELECT\n\nARROW : MOVE\nENTER : SELECT\n\nR : BACK_TITLE\n\n4 : START STAGE4", 0, 0);
}

void StageSelect::draw() {
	// �X�v���C�g���ʃR�}���h
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	// �X�v���C�g�`��
	for (UINT i = 0, size = _countof(sprites); i < size; ++i) {
		selectBack[i].drawWithUpdate(DirectXCommon::getInstance(), spCom);
		sprites[i].drawWithUpdate(DirectXCommon::getInstance(), spCom);
	}
	// �f�o�b�O�e�L�X�g�`��
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}
