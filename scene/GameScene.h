#pragma once

#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "Player.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "Skydome.h"
#include "RailCamera.h"
#include <list>
#include"sstream"
#include "Fade.h"
#include "DeathParticles.h"
// ゲームのフェーズ（型）
enum class PhaseScene {
	kFadeIn,  // フェードイン
	kPlay,    // ゲームプレイ
	kDeath,   // デス演出
	kFadeOut, // フェードアウト
	kClear,
};

class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>

	void Draw();

	/// <summary>
	///	フェーズの切り替え
	/// </summary>
	void ChangePhase();

	/// < summary>
	/// デスフラグのgetter
	/// </summary>
	bool IsFinished() const { return finished_; }

	/// <summary>
	/// 衝突判定を応答
	/// </summary>
	void CheackAllCollisions();

	/// <summary>
	/// 敵弾を追加する
	/// </summary>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	/// <summary>
	/// 敵弾リストの更新
	/// </summary>
	void UpdateEnemyBullets();

	/// <summary>
	///	敵弾リストの描画
	/// </summary>
	void DrawEnemyBullets(ViewProjection& viewProjection);

	void Bulletdelete();

	/// <summary>
	/// 敵出現
	/// </summary>
	void SpawnEnemy(const Vector3& position);

	/// <summary>
	/// 敵の追加
	/// </summary>
	void AddEnemy(Enemy* enemy);

	/// <summary>
	/// 敵の削除
	/// </summary>
	void EnemyDelete();

	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData(int stageNumber);

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands(int currentStage);

	/// <summary>
	/// ステージを更新する関数
	/// </summary>
	void NextStage();

	bool CheckAllEnemiesDead() const;

	void IncrementEnemyDefeatedCount(); // 敵を倒した数をインクリメントする
	
private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// テクスチャ
	uint32_t textureHandle_ = 0;
	uint32_t clearHandle_ = 0;

	// 3Dモデル
	Model* model_ = nullptr;
	// 天球モデル
	Model* modelSkydome_ = nullptr;
	// 自機モデル
	Model* modelPlayer_ = nullptr;
	std::unique_ptr<Model> playerModel_;
	
	// パーティクル
	std::unique_ptr<Model> modelParticle_;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	WorldTransform worldTransformHP_;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	bool isDebugCameraActive_ = false;
	// 自キャラ
	Player* player_ = nullptr;
	// 敵キャラ
	//Enemy* enemy_ = nullptr;
	// 敵リスト
	std::list<Enemy*> enemies_;

	list<EnemyBullet*> enemyBullets_;
	// 天球
	Skydome* skydome_ = nullptr;
	// レールカメラ
	RailCamera* railCamera_ = nullptr;
	// プレイヤーの座標を前にずらす量
	float frontPos = 25.0f;

	// 敵発生コマンド
	std::stringstream enemyPopCommands;
	bool isWait_ = false;
	int waitTimer_ = 0;

	std::unique_ptr<Sprite> sprite_;
	// パーティクル
	std::unique_ptr<DeathParticles> deathParticles_;
	// フェード
	std::unique_ptr<Fade> fade_;

	// ゲームの現在フェーズ（変数）
	PhaseScene phaseScene_;

	// 終了フラグ
	bool finished_ = false;
	bool isDead = false;

	int currentStage_ = 1;
	int stageNumber_ = 1;
	int enemyCountInCurrentStage_ = 0; // 現在のステージに存在する敵の数
	bool isDeadCount = false;
	int deadCount_ = 0;
	// 全体の敵を倒した数
	int Count_ = 0;
	bool isClear = false;

	int enemyDefeatedCount_ = 0; // 敵を倒した数をカウントする変数

};
