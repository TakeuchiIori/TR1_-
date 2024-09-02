#include "TitleScene.h"
#include "assert.h"

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
	
}

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	model_.reset(Model::CreateFromOBJ("title", true));
	textureHandle_ = TextureManager::Load("./Resources./skydome.png");
	sprite_.reset(Sprite::Create(textureHandle_, {0, 0}));
	sprite_->SetSize(Vector2(1280, 720));
	viewProjection_.Initialize();
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, -2.2f, -40.0f};
	// フェード
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void TitleScene::Update() {
	
		XINPUT_STATE joyState;

	//// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		// フェードインが終わったらメインフェーズに移行
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	 
	case Phase::kMain:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			// フェードアウト開始
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		break;
	}




	worldTransform_.TransferMatrix();
	worldTransform_.Update();
}

void TitleScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	sprite_->Draw();


	//  スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	if (!finished_) {
		model_->Draw(worldTransform_, viewProjection_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	// フェードイン&&フェードアウト中はフェードの描画
	if (phase_ == Phase::kFadeIn || phase_ == Phase::kFadeOut) {
		fade_->Draw();
	}

	// スプライト描画後処理
	Sprite::PostDraw();
	

	
	
}
