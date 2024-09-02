#include "RailCamera.h"
#include "MathFunc.h"
void RailCamera::Initialize(WorldTransform& worldTransform) {
	worldTransform_.translation_.x = worldTransform.matWorld_.m[3][0];
	worldTransform_.translation_.y = worldTransform.matWorld_.m[3][1];
	worldTransform_.translation_.z = worldTransform.matWorld_.m[3][2];
	worldTransform_.rotation_ = worldTransform.rotation_;
	worldTransform_.translation_.z = -50;
	viewProjection_.farZ = 1000.0f;
	viewProjection_.Initialize();
	// シングルインスタンスを取得
	input_ = Input::GetInstance();
}

void RailCamera::Update() {
	//worldTransform_.translation_.z -= 0.01f;

	

	
	worldTransform_.World();
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);

}

Vector3 RailCamera::GetWroldPosition() { 
	// ワールド座標を入れる変数
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

