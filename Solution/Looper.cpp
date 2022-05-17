#include "Looper.h"

#include "Input.h"

#include "DirectXCommon.h"
#include "SceneManager.h"

#include <DirectXMath.h>






Looper::Looper() {
	SceneManager::getInstance()->init();
}

Looper::~Looper() {
	SceneManager::getInstance()->fin();
}

Looper* Looper::getInstance() {
	static Looper lpr;
	return &lpr;
}

bool Looper::loop() {

	// 入力情報の更新
	Input::getInstance()->update();

	// ESCで終了
	if (Input::getInstance()->hitKey(DIK_ESCAPE)) return true;

	// --------------------
	// シーンマネージャーの更新
	// --------------------
	SceneManager::getInstance()->update();

	// --------------------
	// シーンマネージャーの描画
	// --------------------
	constexpr DirectX::XMFLOAT3 clearColor = { 34.f / 255, 32.f / 255, 52.f / 255 };	//青っぽい色
	DirectXCommon::getInstance()->startDraw(clearColor);

	SceneManager::getInstance()->draw();

	DirectXCommon::getInstance()->endDraw();


	return false;
}
