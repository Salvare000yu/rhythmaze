#include "SceneManager.h"

#include "TitleScene.h"
#include "PlayScene.h"
#include "EndScene.h"
#include "StageSelect.h"
#include "Explanation.h"
#include "Stage2.h"
#include "Stage3.h"
#include "Stage4.h"
#include "Stage5.h"
#include "Stage6.h"
#include "Stage7.h"
#include "Stage8.h"
#include "Stage9.h"
#include "Stage10.h"
#include "Stage11.h"
#include "Stage12.h"
#include "Stage13.h"
#include "Stage14.h"
#include "Stage15.h"

#include "Input.h"

SceneManager::SceneManager()
	: nextScene(SCENE_NUM::NONE) {

	nowScene = (GameScene*)new TitleScene();
}

SceneManager* SceneManager::getInstange() {
	static SceneManager sm;
	return &sm;
}


void SceneManager::init() {
	nowScene->init();
}

void SceneManager::update() {

	// 次のシーンがあったら
	if (nextScene != SCENE_NUM::NONE) {

		// 今のシーンを削除
		nowScene->fin();
		delete nowScene;

		// undone シーンを追加する際はここのcaseも追加
		switch (nextScene) {
		case SCENE_NUM::TITLE:
			nowScene = new TitleScene();
			break;
		case SCENE_NUM::STAGE1:
			nowScene = new PlayScene();
			break;
		case SCENE_NUM::END:
			nowScene = new EndScene();
			break;
		case SCENE_NUM::SELECT:
			nowScene = new StageSelect();
			break;
		case SCENE_NUM::EXPLANATION:
			nowScene = new Explanation();
			break;
		case SCENE_NUM::STAGE2:
			nowScene = new Stage2();
			break;
		case SCENE_NUM::STAGE3:
			nowScene = new Stage3();
			break;
		case SCENE_NUM::STAGE4:
			nowScene = new Stage4();
			break;
		case SCENE_NUM::STAGE5:
			nowScene = new Stage5();
			break;
		case SCENE_NUM::STAGE6:
			nowScene = new Stage6();
			break;
		case SCENE_NUM::STAGE7:
			nowScene = new Stage7();
			break;
		case SCENE_NUM::STAGE8:
			nowScene = new Stage8();
			break;
		case SCENE_NUM::STAGE9:
			nowScene = new Stage9();
			break;
		case SCENE_NUM::STAGE10:
			nowScene = new Stage10();
			break;
		case SCENE_NUM::STAGE11:
			nowScene = new Stage11();
			break;
		case SCENE_NUM::STAGE12:
			nowScene = new Stage12();
			break;
		case SCENE_NUM::STAGE13:
			nowScene = new Stage13();
			break;
		case SCENE_NUM::STAGE14:
			nowScene = new Stage14();
			break;
		case SCENE_NUM::STAGE15:
			nowScene = new Stage15();
			break;
		}

		nextScene = SCENE_NUM::NONE;	//次シーンの情報をクリア
		nowScene->init();	//シーンを初期化
	}

	nowScene->update();
}

void SceneManager::draw() {
	nowScene->draw();
}

void SceneManager::fin() {
	nowScene->fin();

	if (nowScene != nullptr) {
		delete nowScene;
		nowScene = nullptr;
	}
}

void SceneManager::changeScene(const SCENE_NUM nextScene) {
	this->nextScene = nextScene;
}

void SceneManager::goal(const unsigned clearHalfBeat, const unsigned clearCombo, const bool achieving) {
	this->clearHalfBeat = clearHalfBeat;
	this->clearCombo = clearCombo;
	this->achieving = achieving;
	changeScene(SCENE_NUM::END);
}
