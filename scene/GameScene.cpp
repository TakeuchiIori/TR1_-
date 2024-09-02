#include "GameScene.h"
#include "AxisIndicator.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "Vector2.h"
#include <cassert>
#include "fstream"
#include <sstream>
#include "algorithm"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete modelSkydome_;
	delete player_;
	delete skydome_;
	delete debugCamera_;
	delete railCamera_;
	// bullet_の解放
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureHandle_ = TextureManager::Load("./Resources./uvChecker.png");
	
	

	
	//=============== 3Dモデル ===============//

	model_ = Model::Create();
	// 天球の3Dモデル
	modelSkydome_ = Model::CreateFromOBJ("skydome_sphere", true);
	// 自機
	playerModel_.reset(Model::CreateFromOBJ("player", true));
	

	//=============== 2Dスプライト ===============//
	
	TextureManager::Load("./Resources/Reticle.png");

	//=============== 天球の初期化 ===============//

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_,&viewProjection_);

	// =============== パーティクル ===============//

	modelParticle_.reset(Model::CreateFromOBJ("depthParticles", true));

	//  ワールドトランスフォームの初期化
	worldTransform_.Initialize();


	worldTransformHP_.rotation_.x = 90;
	// ビュープロジェクションの初期化
	viewProjection_.farZ = 150.0f;
	viewProjection_.Initialize();
	// デバッグカメラ
	debugCamera_ = new DebugCamera(1280,720);
	isDebugCameraActive_ = false;
	// 軸方向表示の表示を有効
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
	//=============== 自キャラの生成 ===============//

	player_ = new Player();
	Vector3 playerPosition(0, -5, frontPos);
	player_->Initialize(playerModel_.get(), playerPosition);

	//=============== 敵キャラの生成 ===============//
	Vector3 enemyPosition_ = {2.0f, 5.0f, 150.0f};
	// ステージの初期化処理
	enemyCountInCurrentStage_ = 0; // ステージ開始時にカウントをリセット
	

	//=============== レールカメラの初期化 ===============//
	railCamera_ = new RailCamera();
	railCamera_->Initialize(worldTransform_);
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());
	LoadEnemyPopData(stageNumber_);

	// フェーズ開始
	phaseScene_ = PhaseScene::kFadeIn;
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void GameScene::Update() { 
	ChangePhase();
	
	
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	
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
	player_->Draw(viewProjection_);
	
	
	for (Enemy* enemy : enemies_) {
		if (!enemy->IsDead()) { // 敵が生きている場合のみ描画
			enemy->Draw();
		}
	}
	DrawEnemyBullets(viewProjection_);

	if (deathParticles_) {
		deathParticles_->Draw();
	}
	
	//=============== 天球の描画 ===============//
	skydome_->Draw();
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
	player_->DrawUI();

	if (phaseScene_ == PhaseScene::kFadeIn || phaseScene_ == PhaseScene::kFadeOut) {
		fade_->Draw();
	}
	
	

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phaseScene_) {
	case PhaseScene::kFadeIn:
		isClear = false;

		player_->Update();
		skydome_->Update();
		for (Enemy* enemy : enemies_) {
			// エネミーの更新
			enemy->Update();
		}	

		#ifdef _DEBUG
		// デバッグカメラの有効/無効を切り替える
		if (input_->TriggerKey(DIK_RETURN)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif

		// カメラの処理
		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		} else {
			// レールカメラの更新
			railCamera_->Update();
			viewProjection_.matView = railCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		}

		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();

		fade_->Update();
		// フェードインが終わったらメインフェーズに移行
		if (fade_->IsFinished()) {
			phaseScene_ = PhaseScene::kPlay;
		}


		break;
	case PhaseScene::kPlay:

		CheackAllCollisions();
		
		

		UpdateEnemyPopCommands(currentStage_);
		for (Enemy* enemy : enemies_) {
			// エネミーの更新
			enemy->Update();
		}

		UpdateEnemyBullets();

		if (CheckAllEnemiesDead()) {
			NextStage();                       // ステージを進行させる
			//phaseScene_ = PhaseScene::kFadeIn; // フェードインフェーズに移行
		}
	
#ifdef _DEBUG
		// デバッグカメラの有効/無効を切り替える
		if (input_->TriggerKey(DIK_RETURN)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif

		// カメラの処理
		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		} else {
			// レールカメラの更新
			railCamera_->Update();
			viewProjection_.matView = railCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		}

		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();

		player_->Update();

		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phaseScene_ = PhaseScene::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			// 自キャラの座標にデスパーティクルを発生、初期化
			deathParticles_ = std::make_unique<DeathParticles>();
			deathParticles_->Initialize(modelParticle_.get(), &viewProjection_, deathParticlesPosition);

			if (deathParticles_) {
				deathParticles_->Update();
			}
		}

		if (Count_ == 10) {
			
			phaseScene_ = PhaseScene::kClear;
		} 

		
		ImGui::Begin("Data");
		ImGui::DragInt("enemyDefeatedCount_", &enemyDefeatedCount_);
		ImGui::End();
		break;
	case PhaseScene::kDeath:

		// パーティクル
		if (deathParticles_) {
			deathParticles_->Update();
		}
		UpdateEnemyPopCommands(currentStage_);

		for (Enemy* enemy : enemies_) {
			// エネミーの更新
			enemy->Update();
		}
		UpdateEnemyBullets();

		
		skydome_->Update();

#ifdef _DEBUG
		if (input_->TriggerKey(DIK_RETURN)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif
		// カメラの処理
		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			//  ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			railCamera_->Update();
			viewProjection_.matView = railCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
			viewProjection_.TransferMatrix();
		}
		if (deathParticles_ && deathParticles_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phaseScene_ = PhaseScene::kFadeOut;
		}

		break;
	case PhaseScene::kFadeOut:
		
		fade_->Update();
		
		if (fade_->IsFinished()) {
			finished_ = true; // ゲームシーンを終了
		}

		break;

	case PhaseScene::kClear:
		isClear = true;
	
		fade_->Start(Fade::Status::FadeOut, 1.0f);
		phaseScene_ = PhaseScene::kFadeOut;
		

	default:
		break;

	}
}
bool GameScene::CheckAllEnemiesDead() const {

	return deadCount_ == enemyCountInCurrentStage_; // 死亡した敵の数がステージごとの敵の数と一致するかチェック
}
void GameScene::IncrementEnemyDefeatedCount() {
	enemyDefeatedCount_++;
	// 必要に応じて、敵を倒した数をログに出力するなどの処理を追加
}

void GameScene::CheackAllCollisions() {
	// 判定対象AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定
	// 自キャラの座標
	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets_) {
		// 敵弾の座標
		posB = bullet->GetWorldPosition(); // 敵弾の座標を取得

		float distance = Length(posA - posB);

		// 球と球の交差判定
		if (distance <= (player_->GetRadius() + bullet->GetRadius())) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	// 自弾と敵キャラの当たり判定
	for (Enemy* enemy : enemies_) {
		posA = enemy->GetWorldPosition();
	
			for (PlayerBullet* bullet : playerBullets) {
				posB = bullet->GetWorldPosition();
				float distance = Length(posA - posB);

				if (distance <= enemy->GetRadius() + bullet->GetRadius()) {
				    if (!enemy->IsDead()) { // 敵が生きている場合のみ処理
					    enemy->OnCollision();
					    bullet->OnCollision();

					    isDeadCount = true;
				    }
					if (isDeadCount) {
						deadCount_++;
					    Count_++;
					}

				} else {
					isDeadCount = false;
				}
			}
		
	}
#pragma endregion

#pragma region 自弾と敵弾の当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		for (EnemyBullet* enemyBullet : enemyBullets_) {
			posA = playerBullet->GetWorldPosition();
			posB = enemyBullet->GetWorldPosition();

			float distance = Length(posA - posB);

			if (distance <= playerBullet->GetRadius() + enemyBullet->GetRadius()) {
				// 敵弾のコールバックを呼び出す
				enemyBullet->OnCollision();
				// 自弾のコールバックを呼び出す
				playerBullet->OnCollision();
			}
		}
	}
#pragma endregion
}
void GameScene::AddEnemy(Enemy* enemy) { enemies_.push_back(enemy); }
void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}
void GameScene::Bulletdelete() {
	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
}
void GameScene::EnemyDelete() {
	// 削除フラグが立った敵をリストから削除
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
}
void GameScene::UpdateEnemyBullets() {
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	//  弾の更新
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}
}

void GameScene::DrawEnemyBullets(ViewProjection& viewProjection) {
	for (EnemyBullet* bullet : enemyBullets_) {
		if (!bullet->IsDead()) { // 弾が生きている場合のみ描画
			bullet->Draw(viewProjection);
		}
	}
}
void GameScene::SpawnEnemy(const Vector3& position) {
	// 敵の生成
	Enemy* enemy = new Enemy();

	// 敵の初期化
	enemy->Initialize(model_,position,&viewProjection_);

	// プレイヤーとゲームシーンの設定
	enemy->SetPlayer(player_);
	enemy->SetGameScene(this);

	// 敵を追加
	AddEnemy(enemy);
	// 現在のステージに敵を追加
	enemyCountInCurrentStage_++;
}

void GameScene::LoadEnemyPopData(int stageNumber) {
	std::ostringstream filenameStream;
	filenameStream << "Resources./csv./stage" << stageNumber << "_EnemyPop.csv";
	std::string filename = filenameStream.str();

	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return;
	}

	enemyPopCommands.str("");
	enemyPopCommands.clear();
	enemyPopCommands << file.rdbuf();

	if (!enemyPopCommands) {
		std::cerr << "Failed to read enemy pop commands from file: " << filename << std::endl;
	}

	file.close();
}

void GameScene::UpdateEnemyPopCommands(int currentStage) {
	// 待機処理
	if (isWait_) {
		waitTimer_--;
		if (waitTimer_ <= 0) {
			isWait_ = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
		std::istringstream line_stream(line);
		std::string word;
		getline(line_stream, word, ',');

		if (word.find("//") == 0) {
			continue;
		} else if (word.find("STAGE") == 0) {
			getline(line_stream, word, ',');
			int stageNumber = atoi(word.c_str());
			if (stageNumber != currentStage) {
				continue;
			}
		} else if (word.find("POP") == 0) {
			float x, y, z;
			getline(line_stream, word, ',');
			x = (float)std::atof(word.c_str());
			getline(line_stream, word, ',');
			y = (float)std::atof(word.c_str());
			getline(line_stream, word, ',');
			z = (float)std::atof(word.c_str());
			SpawnEnemy(Vector3(x, y, z));
		} else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');
			int32_t waitTime = atoi(word.c_str());
			isWait_ = true;
			waitTimer_ = waitTime;
			break;

		}
	}
}

void GameScene::NextStage() {
	currentStage_++;
	deadCount_ = 0;
	enemyCountInCurrentStage_ = 0;
	
	LoadEnemyPopData(currentStage_);
}