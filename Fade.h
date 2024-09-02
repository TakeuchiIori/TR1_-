#pragma once
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <memory>
/// <summary>
/// フェード
/// </summary>




class Fade {
public:
	enum class Status {
		None,    // フェードなし
		FadeIn,  // フェードイン中
		FadeOut, // フェードアウト中
	};


public:
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
	/// フェード開始
	/// </summary>
	void Start(Status status, float duration);

	/// < summary>
	/// フェード終了
	/// </summary>
	void Stop();

	/// < summary>
	/// フェード終了判定
	/// </summary>
	bool IsFinished() const;

private:
	
	std::unique_ptr<Sprite> sprite_;

	// テクスチャ
	uint32_t textureHandle_ = 0;
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;

	// 終了フラグ
	bool finished_ = false;
	// 現在のフェードの状態
	Status status_ = Status::None;

	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;

};
