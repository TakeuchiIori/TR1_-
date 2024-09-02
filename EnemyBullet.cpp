#include "EnemyBullet.h"

EnemyBullet::EnemyBullet() {}

EnemyBullet::~EnemyBullet() {}

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NILLポインタチェック
	assert(model);
	model_ = model;
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取ったワールドトランスフォームをメンバ変数に代入
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = 45;
	// 引数で受け取っ速度をメンバ変数に代入
	velocity_ = velocity;
	// Y軸周り角度（θy）
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);

	// 自機の方向を基準にして、velocity_を変換するための回転行列を計算
	Matrix4x4 rotateToFaceDirection = MakeRotateMatrixY(-worldTransform_.rotation_.y);

	// velocity_に回転行列を掛け算して自機の向きに合わせた新しいvelocity_を求める
	Vector3 rotateVeclocity = TransformNormal(velocity_, rotateToFaceDirection);

	// X軸回りの回転角度（θx）を計算
	worldTransform_.rotation_.x = std::atan2(-rotateVeclocity.y, rotateVeclocity.z);
}

void EnemyBullet::Update() {
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
	// 座標を移動させる
	worldTransform_.translation_ += velocity_;
	// ワールドトランスフォームの更新
	worldTransform_.Update();
}

void EnemyBullet::OnCollision() {
	isDead_ = true;
}

Vector3 EnemyBullet::GetWorldPosition() { 
	// ワールド座標を入れる変数
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;

}

void EnemyBullet::Draw(const ViewProjection& viewProjection) {
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection);
}
