#pragma once
#include "EnemyBullet.h"
#include "Input.h"
#include "Audio.h"
#include <list>
using namespace std;


// 行動フェーズ
enum class Phase {
	Approch, // 接近する
	Leave,   // 離脱する
};
class Player;
class GameScene;
class Enemy {
public:
	
	/// <summary>
	/// コンストクラタ
	/// </summary>
	Enemy();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& position,ViewProjection* viewProjection);

	/// <summary>
	/// 接近処理の初期化
	/// </summary>
	void InitApproch();

	/// <summary>
	/// 接近処理
	/// </summary>
	void Approch();

	/// <summary>
	/// 接近処理の初期化
	/// </summary>
	void InitLeave();
	/// <summary>
	/// 離脱処理
	/// </summary>
	void Leave();

	/// <summary>
	/// 弾発射
	/// </summary>
	void Fire();

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// プレイヤー
	/// </summary>
	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// 弾リスト取得
	/// </summary>
//	const list<EnemyBullet*>& GetBullets() const { return bullets_; }

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetWorldPosition();

	/// <summary>
	/// ゲームシーン
	/// </summary>
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

public:
	float Setradius() { return rad; }
	float GetRadius() { return rad; }
	bool IsDead() const { return isDead_; }


private:
	Audio* audio_ = nullptr;
	// ダメージ食らったときに鳴る音
	uint32_t damegeHandle_ = 0;
	uint32_t damegeSound_ = 0;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	// モデル
	Model* model_ = nullptr;
	std::unique_ptr<Model> modelEnemyBullet;
	// テクスチャ
	uint32_t textureHandle_ = 0;
	// フェーズ
	Phase phase_ = Phase::Approch;
	// 弾
	//list<EnemyBullet*> bullets_;
	// 自キャラ
	Player* player_ = nullptr;
	// 接近
	Vector3 kApprochSpeed = {0.0f, 0.0f, -1.05f};
	// 離脱
	Vector3 kLeaveSpeed = {0.0f, 0.0f, -0.01f}; //{-0.1f, 0.1f, 0.0f};
	// ゲームシーン
	GameScene* gameScene_ = nullptr;
	// 半径
	static inline const float rad = 1.0f;

	bool isDead_ = false;

	// クリアフラグ
	bool isClear_ = false;

	

	
private: //メンバ関数ポインタ
	// 発射タイマー
	int32_t shotTImer_ = 0;
	// 発射感覚
	int32_t kFireInterval = 60;

	static void (Enemy::*phaseEnemy[])();


	
};
