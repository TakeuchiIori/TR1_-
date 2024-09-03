#include "Enemy.h"
#include "Player.h"
#include "ImGuiManager.h"
#include "GameScene.h"
#include <algorithm>
#define NOMINMAX
Enemy::Enemy() {
	
 }

Enemy::~Enemy() { 
}

 void(Enemy::*Enemy::phaseEnemy[])() = {
	 &Enemy::Approch,
     &Enemy::Leave
};

void Enemy::Initialize(Model* model, const Vector3& position, ViewProjection* viewProjection) {
	// NILLポインタチェック
	assert(model);
	model_ = model;
	modelEnemyBullet.reset(Model::CreateFromOBJ("enemyBullet", true));
	viewProjection_ = viewProjection;
	worldTransform_.translation_ = position;
	textureHandle_ = TextureManager::Load("./Resources./Enemy./enemy.png");
	worldTransform_.Initialize();
	audio_ = Audio::GetInstance();

	damegeHandle_ = audio_->LoadWave("./Resources./Sound./SE./se_damage12.mp3");
	isDead_ = false;
	//worldTransform_.translation_ = {2.0f, 2.0f, 5.0f};
	// 接近フェーズ
	InitApproch();
}
void Enemy::Update() {
	audio_->SetVolume(damegeHandle_, 0.1f);
	// メンバ関数ポインタの呼び出し
	(this->*phaseEnemy[static_cast<size_t>(phase_)])();
	//worldTransform_.translation_.z += 0.01f;
	// アフィン変換行列の計算
	Matrix4x4 moveMatrix = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// ワールド行列に代入
	worldTransform_.matWorld_ = moveMatrix;
	// 行列を定数バッファに転送
	worldTransform_.Update();
	ImGui::Begin("Enemy");
	ImGui::DragFloat("3DEnemy", &worldTransform_.translation_.z);
	ImGui::DragInt("kFireInterval", &kFireInterval);
	ImGui::End();
}

void Enemy::Draw() {
	// 3Dモデル
	if (!isDead_) {
		model_->Draw(worldTransform_, *viewProjection_, textureHandle_);
	}
}
void Enemy::InitApproch() {
	// 発射タイマーを初期化
	shotTImer_ = 50;
}
void Enemy::Approch() {
	shotTImer_--;
	if (shotTImer_ == 0) {
		Fire();
		shotTImer_ = kFireInterval;
	}
	// 移動（ベクトルを加算）
	worldTransform_.translation_ += kApprochSpeed;
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::InitLeave() {
	// 発射タイマーを初期化
	shotTImer_ = 20;
}

void Enemy::Leave() {
	shotTImer_--;
	if (shotTImer_ == 0) {
		Fire();
		shotTImer_ = kFireInterval;
	}
	 //移動（ベクトルを加算）
	//worldTransform_.translation_ += kLeaveSpeed;
	 if (worldTransform_.translation_.y > 20.0f) {
		worldTransform_.translation_ = {2.0f, 2.0f, 5.0f};
		phase_ = Phase::Approch;
	 }
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得　（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
void Enemy::Fire() {
	if (!isDead_) {
		assert(player_);
		assert(gameScene_);
		// 弾の速度
		const float kBulletSpeed = 0.5f;
		// 自キャラのワールド座標を取得
		Vector3 playerPos = player_->GetWorldPosition();
		Vector3 enemyPos = GetWorldPosition();
		Vector3 difference = playerPos - enemyPos;
		difference = Normalize(difference);
		difference *= kBulletSpeed;
		Vector3 velocity = difference;

		// 弾を生成し、初期化
		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(modelEnemyBullet.get(), worldTransform_.translation_, velocity);
		

		// GameSceneの敵弾リストに弾を追加
		gameScene_->AddEnemyBullet(newBullet);
	}
}
void Enemy::OnCollision() {
	isDead_ = true;
	damegeSound_ = audio_->PlayWave(damegeHandle_, false);

	// ゲームシーンのカウンターをインクリメント
	if (gameScene_) {
		gameScene_->IncrementEnemyDefeatedCount();
	}
}

void Enemy::AdjustParameters(float difficultyAdjustment) {
	// 速度の調整
	kApprochSpeed.z *= (1.0f + difficultyAdjustment);
	kLeaveSpeed.z *= (1.0f + difficultyAdjustment);

	// 弾の発射間隔を調整
	kFireInterval = static_cast<int32_t>(kFireInterval / (1.0f + difficultyAdjustment));
	kFireInterval = (std::max)(kFireInterval, 20); // 最低でも20フレーム間隔に制限
}
