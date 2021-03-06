#pragma once
#include "GameScene.h"

// undone シーンを追加する際は此処にシーン管理番号を追加する
enum class SCENE_NUM : unsigned short {
	NONE,
	TITLE, SELECT, EXPLANATION, END,
	STAGE1,
	STAGE2,
	STAGE3,
	STAGE4,
	STAGE5,
	STAGE6,
	STAGE7,
	STAGE8,
	STAGE9,
	STAGE10,
	STAGE11,
	STAGE12,
	STAGE13,
	STAGE14,
	STAGE15
};

class SceneManager
	: public GameScene {
private:
	SceneManager(const SceneManager& sm) = delete;
	SceneManager& operator=(const SceneManager& sm) = delete;
	SceneManager();

	GameScene* nowScene = nullptr;
	SCENE_NUM nextScene;

	unsigned stageNum = 0u;
	unsigned clearHalfBeat = 0u;	// クリア時間
	unsigned clearCombo = 0u;	// クリア時のコンボ
	bool achieving = true;

public:
	// https://dixq.net/g/sp_06.html
	// ↓SceneMgr.h↓　シーン管理部

	static SceneManager* getInstance();

	void setStageNum(unsigned stageNum) { this->stageNum = stageNum; }

	void init() override;
	void update() override;
	void draw() override;
	void fin() override;

	void changeScene(const SCENE_NUM nextScene);
	void goal(unsigned stageNum, const unsigned clearHalfBeat, const unsigned clearCombo, const bool achieving = true);

	inline unsigned getStageNum() const { return stageNum; }
	inline unsigned getClearHalfBeat() const { return clearHalfBeat; }
	inline unsigned getClearCombo() const { return clearCombo; }
	inline bool getAchieving() const { return achieving; }
};

