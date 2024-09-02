#pragma once
#include <Input.h>
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Fade.h"
#include "Input.h"
#include <memory>
#include <DirectXCommon.h>
#include "TextureManager.h"
#include "Sprite.h"
/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {

public:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,	  // フェードイン
		kMain,		  // メイン部
		kFadeOut,	  // フェードアウト
	};


public:
	// コンストラクタ
	TitleScene();
	// デストラクタ
	~TitleScene();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// < summary>
	/// 更新
	/// </summary>
	void Update();

	/// < summary>
	/// 描画
	/// </summary>
	void Draw();

	/// < summary>
	/// デスフラグのgetter
	/// </summary>
	bool IsFinished() const { return finished_; }

private: // メンバ変数
	Input* input_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<Fade> fade_;
	std::unique_ptr<Model> model_;
	uint32_t textureHandle_ = 0;
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;
	// 終了フラグ
	bool finished_ = false;
	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

};
