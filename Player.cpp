#include "Player.h"
#include "ImGuiManager.h"
#include <WinApp.h>
#define NOMINMAX
Player::Player() {}

Player::~Player() {
	// bullet_の解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	delete sprite2DReticle_;
}

void Player::Initialize(Model* model, Vector3 pos) {
	// NILLポインタチェック
	assert(model);
	model_ = model;
	modelPlayerBullet.reset(Model::CreateFromOBJ("playerBullet", true));
	//textureHandle_ = textureHandle;
	viewProjection_.Initialize();
	// レティクル用のテクスチャ
	uint32_t textureReticle = TextureManager::Load("./Resources./2DReticle.png");
	worldTransform_.Initialize();
	audio_ = Audio::GetInstance();//SetVolume
	shotHandle_ = audio_->LoadWave("./Resources./Sound./SE./se_alien_shot01.mp3");
	damegeHandle_ = audio_->LoadWave("./Resources./Sound./SE./se_damage10.mp3");

	worldTransform3DReticle_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	//isPad_ = false;
	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {100,50}, {1, 1, 1, 1}, {0.5, 0.5});
	// シングルインスタンスを取得
	input_ = Input::GetInstance();
	isDead_ = false;
}

void Player::Update() {
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	// XInputGetState関数を使用してゲームパッドの状態を取得
	DWORD result = XInputGetState(0, &state);

	if (result == ERROR_SUCCESS) {
		isPad_ = true;
	} else {
		// ゲームパッドが接続されていない場合の処理
		isPad_ = false;
	}	

	Bulletdelete();

	Rotate();

	Move();

	Attack();

	Reticle3DMove();

	TransformReticle();

	GetMousePosition();

	// 弾の更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
	// 行列を定数バッファに転送
	worldTransform_.Update();
	ImGui::Begin("Player");
	ImGui::DragFloat("3DPlayer", &worldTransform_.translation_.z);
	ImGui::DragFloat("3DReticle_", &worldTransform3DReticle_.translation_.z);
	ImGui::End();
}

void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデル
	if (!isDead_) {
		model_->Draw(worldTransform_, viewProjection, textureHandle_);
	}
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
	model_->Draw(worldTransform3DReticle_, viewProjection, textureHandle_);
}

void Player::DrawUI() {
	sprite2DReticle_->Draw();

}

void Player::Rotate() {

}

void Player::OnCollision() {
	isDead_ = true;
	//audio_->SetVolume(damegeHandle_, 0.01f);
	damegeSound_ = audio_->PlayWave(damegeHandle_, false);
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

void Player::Attack() {

	timeSinceLastShot_ += deltaTime; // deltaTimeはフレーム間の時間
	XINPUT_STATE joyState;
	
	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		

		// クールダウン時間が経過しているか確認
		if (timeSinceLastShot_ >= fireCooldown_) {
			shotSound_ = audio_->PlayWave(shotHandle_, false);
			//audio_->SetVolume(shotHandle_, 0.01f);
			
			// 弾の速度
			const float kBulletSpeed = 1.0f;
			Vector3 velocity(0, 0, kBulletSpeed);

			// 速度ベクトルを自機の向きに合わせて回転させる
			velocity = TransformNormal(velocity, worldTransform_.matWorld_);
			// 自機から照準オブジェクトへのベクトル
			velocity = GetReticleWorldPosition() - GetWorldPosition();
			velocity = Normalize(velocity) * kBulletSpeed;

			// 弾を生成し、初期化
			PlayerBullet* newBullet = new PlayerBullet();
			newBullet->Initialize(modelPlayerBullet.get(), CenterPosition(), velocity);

			// 弾を登録する
			bullets_.push_back(newBullet);

			// クールダウンタイマーをリセット
			timeSinceLastShot_ = 0.0f;
		}
	} else {
		audio_->StopWave(shotSound_);
	}

}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
Vector3 Player::GetReticleWorldPosition() { 
	// ワールド座標を入れる変数
	Vector3 worldPos3DReticle;
	worldPos3DReticle.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPos3DReticle.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPos3DReticle.z = worldTransform3DReticle_.matWorld_.m[3][2];

	return worldPos3DReticle;
}
Vector3 Player::GetWorldRotate() { return Vector3(); }
void Player::Move() {
	// キャラクターの移動ベクトル
	Vector3 move = {0.0f, 0.0f, 0.0f};
	// キャラクターの速さ
	const float kCharacterSpeed = 0.5f;

	   

	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}
	worldTransform_.translation_ += move;
	//----- 移動限界座標 -----//
	const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;
	// 範囲を超えない処理
	worldTransform_.translation_.x = clamp(worldTransform_.translation_.x, -kMoveLimitX, kMoveLimitX);
	worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, -kMoveLimitY, kMoveLimitY);
}
void Player::Reticle3DMove() {
	// 自機から3Dレティクルへの距離を設定
	const float kDistancePlayerTo3DReticle = 25.0f;

	// 自機のワールド行列の回転を反映し、指定した距離だけ前方にレティクルを移動
	Vector3 offset = { 0.0f, 0.0f, 1.0f };
	offset = Normalize(TransformNormal(offset, worldTransform_.matWorld_));
	offset *= kDistancePlayerTo3DReticle;

	// 3Dレティクルの座標を設定
	worldTransform3DReticle_.translation_ = worldTransform_.translation_ + offset;
	worldTransform3DReticle_.Update();
}

void Player::TransformReticle() {
	sprite2DPosition = GetReticleWorldPosition();

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	// ビュー行列をとプロジェクション行列、ビューポート行列を合成
	Matrix4x4 matViewProjectionViewport = Multiply(viewProjection_.matView, Multiply(viewProjection_.matProjection, matViewport));
	// ワールド→スクリーン座標変換
	sprite2DPosition = Transform(sprite2DPosition, matViewProjectionViewport);
	// スプライトのレティクルに座標設定
	sprite2DReticle_->SetPosition(Vector2(sprite2DPosition.x, sprite2DPosition.y));
}

void Player::GetMousePosition() {
	if (isPad_) {
		XINPUT_STATE joyState;
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			Input::GetInstance()->SetJoystickDeadZone(0, 3000, 100);

			// 前回のスティック入力を保持する変数を追加
			static float prevThumbRX = 0.0f;
			static float prevThumbRY = 0.0f;

			// スティックの入力を取得
			float thumbRX = (float)joyState.Gamepad.sThumbRX / SHRT_MAX;
			float thumbRY = (float)joyState.Gamepad.sThumbRY / SHRT_MAX;

			// デッドゾーンの設定
			const float DEADZONE = 0.2f;
			if (fabs(thumbRX) < DEADZONE)
				thumbRX = 0.0f;
			if (fabs(thumbRY) < DEADZONE)
				thumbRY = 0.0f;

			// 線形補間を使用してスムーズにする
			float smoothThumbRX = Lerp(prevThumbRX, thumbRX, 0.06f);
			float smoothThumbRY = Lerp(prevThumbRY, thumbRY, 0.06f);

			// スプライトの位置を更新
			sprite2DPosition.x += smoothThumbRX * 280.0f;
			sprite2DPosition.y -= smoothThumbRY * 280.0f;

			// スプライトの座標変更を反映
			sprite2DReticle_->SetPosition({ sprite2DPosition.x, sprite2DPosition.y });

			// 現在のスティック入力を保存
			prevThumbRX = smoothThumbRX;
			prevThumbRY = smoothThumbRY;
		}

		// レティクルの位置をワールド座標に変換
		Vector3 positionReticle = Vector3(sprite2DPosition.x, sprite2DPosition.y, 0);

		// ビュープロジェクションビューポート合成行列
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
		Matrix4x4 matVPV = Multiply(viewProjection_.matView, Multiply(viewProjection_.matProjection, matViewport));
		Matrix4x4 matInverseVPV = Inverse(matVPV);

		// スクリーン座標
		posNear = Vector3(static_cast<float>(sprite2DPosition.x), static_cast<float>(sprite2DPosition.y), 0);
		posFar = Vector3(static_cast<float>(sprite2DPosition.x), static_cast<float>(sprite2DPosition.y), 1);

		// スクリーン座標系からワールド座標系へ
		posNear = Transform(posNear, matInverseVPV);
		posFar = Transform(posFar, matInverseVPV);

		// マウスレイの方向
		Vector3 PadDirection = posFar - posNear;
		PadDirection = Normalize(PadDirection);

		// カメラから照準オブジェクトの距離
		const float kDistanceTestObject = 50.0f;
		worldTransform3DReticle_.translation_ = posNear + PadDirection * kDistanceTestObject;
		worldTransform3DReticle_.Update();

	}
	else {
		// ここはマウス操作の処理なので、パッドが接続されていない場合の処理になります
		POINT mousePosition;
		GetCursorPos(&mousePosition);
		HWND hwnd = WinApp::GetInstance()->GetHwnd();
		ScreenToClient(hwnd, &mousePosition);

		sprite2DReticle_->SetPosition(Vector2(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)));

		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
		Matrix4x4 matVPV = Multiply(viewProjection_.matView, Multiply(viewProjection_.matProjection, matViewport));
		Matrix4x4 matInverseVPV = Inverse(matVPV);

		posNear = Vector3(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 0);
		posFar = Vector3(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 1);

		posNear = Transform(posNear, matInverseVPV);
		posFar = Transform(posFar, matInverseVPV);

		Vector3 mouseDirection = posFar - posNear;
		mouseDirection = Normalize(mouseDirection);

		const float kDistanceTestObject = 100.0f;
		worldTransform3DReticle_.translation_ = posNear + mouseDirection * kDistanceTestObject;
		worldTransform3DReticle_.Update();
	}
}

void Player::Bulletdelete() {
	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
}

Vector3 Player::CenterPosition() {
	// 見た目上の中心点のオフセット（モデル座標系）
	const Vector3 offset = {0.0f, -0.5f, 0.0f};
	// ワールド座標系に変換
	Vector3 worldPos = Transform(offset, worldTransform_.matWorld_);
	return worldPos;
}



