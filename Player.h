#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include "Input.h"
#include "MathFunc.h"
//#include "imgui.h"
#include "PlayerBullet.h"
#include "Sprite.h"
#include <numbers>
#include <cassert>
#include <list>
#include "Audio.h"
using namespace std;
using namespace std::numbers;
/// <summary>
///
/// </summary>
class Player {
public:
	/// <summary>
	/// コンストクラタ
	/// </summary>
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model,Vector3 pos);

	/// <summary>
	/// 旋回
	/// </summary>
	void Rotate();

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
	void Draw(ViewProjection& viewProjection);

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetWorldPosition();

	/// <summary>
	/// 3Dレティクルのワールド座標を取得
	/// </summary>
	Vector3 GetReticleWorldPosition();
	/// <summary>
	/// ワールド座標の回転行列を取得
	/// </summary>
	Vector3 GetWorldRotate();

	/// <summary>
	/// 弾リスト取得
	/// </summary>
	const list<PlayerBullet*>& GetBullets() const { return bullets_; }

	/// <summary>
	/// 親となるワールドトランスフォームをセット
	/// </summary>
	void SetParent(const WorldTransform* parent);

	/// <summary>
	/// 移動関数
	/// </summary>
	void Move();

	/// <summary>
	/// 3Dレティクルの移動関数
	/// </summary>
	void Reticle3DMove();

	/// <summary>
	/// レティクルの座標変換　ワールド座標　＜　スクリーン座標
	/// </summary>
	void TransformReticle();

	/// <summary>
	/// マウス座標を取得
	/// </summary>
	void GetMousePosition();

	/// <summary>
	/// 弾削除
	/// </summary>
	void Bulletdelete();

	 Vector3 CenterPosition();


	//====== アクセッサ ======//
	float Setradius() { return rad; }
	float GetRadius() { return rad; }
	// デスフラグのgetter
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

private: //====== メンバ変数 ・メンバ関数 =====//


	Audio* audio_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_ ;
	// 3Dレティクル用
	WorldTransform worldTransform3DReticle_;
	// モデル
	Model* model_ = nullptr;
	std::unique_ptr<Model> modelPlayerBullet;
	// 2Dレティクル用スプライト
	Sprite* sprite2DReticle_ = nullptr;
	// テクスチャ
	uint32_t textureHandle_ = 0;
	// キーボード入力
	Input* input_ = nullptr;
	// 弾
	list<PlayerBullet*> bullets_;
	// 半径
	static inline const float rad = 1.0f;
	// 音声ハンドル
	uint32_t shotHandle_ = 0;
	// 音声再生ハンドル
	uint32_t shotSound_ = 0;
	//　ダメージ
	uint32_t damegeHandle_ = 0;
	uint32_t damegeSound_ = 0;

	Vector3 sprite2DPosition;
	Vector3 posNear;
	Vector3 posFar;
	bool isPad_;
	// デスフラグ
	bool isDead_ = false;
	// クールダウンタイマー
	float fireCooldown_ = 0.2f; // 例: 0.5秒のクールダウン
	float timeSinceLastShot_ = 0.0f;
	float deltaTime = 1.0f / 60.0f;
};
