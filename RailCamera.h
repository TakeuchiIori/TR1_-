#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"
class RailCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WorldTransform& worldTransform);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetWroldPosition();

	/// <summary>
	/// ワールドトランスフォームを取得
	/// </summary>
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const ViewProjection& GetViewProjection() const { return viewProjection_; }


private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// キーボード入力
	Input* input_ = nullptr;
};
