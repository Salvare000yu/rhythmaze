#pragma once
#include "GameScene.h"

// undone �V�[����ǉ�����ۂ͍����ɃV�[���Ǘ��ԍ���ǉ�����
enum class SCENE_NUM : unsigned short {
	NONE,
	TITLE, SELECT, EXPLANATION, END,
	STAGE1,
	STAGE2,
	STAGE3,
	STAGE4,
	STAGE5,
	STAGE6,
	STAGE7
};

class SceneManager
	: public GameScene {
private:
	SceneManager(const SceneManager& sm) = delete;
	SceneManager& operator=(const SceneManager& sm) = delete;
	SceneManager();

	GameScene* nowScene = nullptr;
	SCENE_NUM nextScene;

	unsigned clearHalfBeat = 0u;	// �N���A����
	unsigned clearCombo = 0u;	// �N���A���̃R���{
	bool achieving = true;

public:
	// https://dixq.net/g/sp_06.html
	// ��SceneMgr.h���@�V�[���Ǘ���

	static SceneManager* getInstange();

	void init() override;
	void update() override;
	void draw() override;
	void fin() override;

	void changeScene(const SCENE_NUM nextScene);
	void goal(const unsigned clearHalfBeat, const unsigned clearCombo, const bool achieving = true);

	inline unsigned getClearHalfBeat() const { return clearHalfBeat; }
	inline unsigned getClearCombo() const { return clearCombo; }
	inline bool getAchieving() const { return achieving; }
};

