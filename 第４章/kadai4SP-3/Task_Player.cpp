//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map2D.h"
#include  "Task_Shot01.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imageName = "PlayerImg";
		DG::Image_Create(this->imageName, "./data/image/chara02.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		DG::Image_Erase(this->imageName);
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = ML::Box2D(-16, -40, 32, 80);
		this->angle_LR = Right;
		this->controllerName = "P1";

		this->motion = Stand;		//キャラ初期状態
		this->hp = 10;				//HPの設定
		this->maxSpeed = 5.0f;		//最大移動速度（横）
		this->addSpeed = 1.0f;		//歩行加速度（地面の影響である程度打ち消される
		this->decSpeed = 0.5f;		//接地状態の時の速度減衰量（摩擦
		this->maxFallSpeed = 10.0f;	//最大落下速度
		this->jumpPow = -10.0f;		//ジャンプ力（初速）
		this->gravity = ML::Gravity(32) * 5; //重力加速度＆時間速度による加算量

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		this->UpDate_Std();
		//めり込まない移動
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);

		{
			BChara::AttackInfo at = {};
			if (Attack_Std("アイテム", at))
			{
				//接触していた場合、自分に対して何か行う場合は、ここに記述
			}
		}
	}
	void Object::Shot()
	{
		auto  in = DI::GPad_GetState(this->controllerName);

		auto shot01 = Shot01::Object::Create(true);
		shot01->pos = this->pos;
		if (angle_LR == Right)
		{
			shot01->moveVec = ML::Vec2(5, -6);
			shot01->angle_LR = Right;
		}
		else
		{
			shot01->moveVec = ML::Vec2(-5, -6);
			shot01->angle_LR = Left;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (this->unHitTime > 0)
		{
			if ((unHitTime / 4) % 2 == 0)
			{
				return;		//８フレーム中４フレーム画像を表示しない
			}
		}
		this->Render_Std(this->res->imageName);
	}
	//-----------------------------------------------------------------------------
	//思考＆状況判断　モーション決定
	void  Object::Think()
	{
		auto  in = DI::GPad_GetState(this->controllerName);
		BChara::Motion  nm = this->motion;	//とりあえず今の状態を指定

		//思考（入力）や状況に応じてモーションを変更する事を目的としている。
		//モーションの変更以外の処理は行わない
		switch (nm) {
		case  Stand:	//立っている
			//歩行
			if (in.LStick.L.on) { nm = Walk; }
			if (in.LStick.R.on) { nm = Walk; }
			if (in.B4.down) { nm = Attack; }
			//ジャンプ
			if (in.B1.down) { nm = Jump; }
			if (!CheckFoot()) { nm = Fall; }//足元 障害　無し
			break;
		case  Walk:		//歩いている
			if (in.LStick.L.off && in.LStick.R.off) { nm = Stand; }
			if (in.B1.down) { nm = Jump; }
			if (in.B4.down) { nm = Attack; }
			if (!CheckFoot()) { nm = Fall; }
			break;
		case  Jump:		//上昇中
			if (in.B1.down) { nm = Jump2; }
			if (moveVec.y > 0) { nm = Fall; }		//上昇をしたとき
			if (CheckFoot()) { nm = Fall; }			//地面に当たった
			if (moveVec.y == 0) { nm = Fall; }		//天井に当たった
			break;
		case Jump2:
			if (moveVec.y > 0) { nm = Fall2; }		//落下する
			if (CheckFoot()) { nm = Fall2; }		//地面に当たった
			if (moveVec.y == 0) { nm = Fall2; }		//天井に当たった
			break;
		case  Fall:		//落下中
			///Playerが地面に立つモーションが入ります
			if (in.B1.down) { nm = Jump2; }			//ジャンプ２
			if (CheckFoot()) { nm = Landing; }		//Playerの状態を変化させる　足元　障害　有り
			break;
		case Fall2:
			if (CheckFoot()) { nm = Landing; }
			break;
		case  Attack:	//攻撃中
			if (moveCnt == 8)
			{
				nm = Stand;
			}
			break;
		case  TakeOff:	//飛び立ち
			if (in.B1.on)
			{
				if (moveCnt >= 30) { nm = Jump; }
			}
			else if (in.B1.off) { nm = Jump; }

			if (!CheckFoot()) { nm = Fall; }
			break;
		case  Landing:	//着地
			if (moveCnt >= 3) { nm = Stand; }
			break;
		case  Bound:
			if (moveCnt >= 12 && CheckFoot())
			{
				nm = Stand;
			}
			break;
		}
		//モーション更新
		this->UpdateMotion(nm);						//モーションが切り替わる
	}
	//-----------------------------------------------------------------------------
	//	モーションに対応した処理
	//(モーションは変更しない）
	void  Object::Move()
	{
		auto  in = DI::GPad_GetState(this->controllerName);
		//重力加速
		switch (this->motion) {
		default:
			//上昇中もしくは足元に地面が無い
			if (this->moveVec.y < 0 ||
				this->CheckFoot() == false) {
				//落下速度の変化
				this->moveVec.y = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
			}
			//地面に接触している
			else {
				this->moveVec.y = 0.0f;
			}
			break;
			//重力加速を無効化する必要があるモーションは下にcaseを書く（現在対象無し）
		case Unnon:	break;
		}

		//移動速度減衰
		switch (this->motion) {
		default:
			//移動ベクトルのX軸が0ではないとき、０に近付けていく処理
			if (moveVec.x < 0)
			{
				moveVec.x = min(moveVec.x + decSpeed, 0);
			}
			else
			{
				moveVec.x = max(moveVec.x - decSpeed, 0);
			}
			break;
			//移動速度減衰を無効化する必要があるモーションは下にcaseを書く（現在対象無し）
		case Bound:
		case Unnon:	break;
		}
		//-----------------------------------------------------------------
		//モーション毎に固有の処理
		switch (this->motion) {
		case  Stand:	//立っている
			break;
		case  Walk:		//歩いている
			//向きを変化させる・移動ベクトルを設定する
			if (in.LStick.L.on)
			{
				angle_LR = Left;
				moveVec.x = -maxSpeed;
			}
			if (in.LStick.R.on)
			{
				angle_LR = Right;
				moveVec.x = maxSpeed;
			}
			break;
		case  Fall:		//落下中
			if (in.LStick.L.on)
			{
				moveVec.x = -maxSpeed;
			}
			if (in.LStick.R.on)
			{
				moveVec.x = maxSpeed;
			}
			break;
		case Fall2:
			if (in.LStick.L.on)
			{
				moveVec.x = -maxSpeed;
			}
			if (in.LStick.R.on)
			{
				moveVec.x = maxSpeed;
			}
			break;
		case  Jump:		//上昇中
			if (moveCnt == 0)
			{
				//初速の設定
				moveVec.y = this->jumpPow;
			}
			if (CheckHead())				//天井に接触している
			{
				moveVec.y = 0;
			}
			if (in.LStick.L.on)
			{
				moveVec.x = -maxSpeed;
			}
			if (in.LStick.R.on)
			{
				moveVec.x = maxSpeed;
			}
			break;
		case Jump2:
			if (moveCnt == 0)
			{
				//初速の設定
				moveVec.y = this->jumpPow * 0.8f;
			}
			if (CheckHead())				//天井に接触している
			{
				moveVec.y = 0;
			}
			if (in.LStick.L.on)
			{
				moveVec.x = -maxSpeed;
			}
			if (in.LStick.R.on)
			{
				moveVec.x = maxSpeed;
			}
			break;
		case  Attack:	//攻撃中
			if (moveCnt == 4)
			{
				Shot();
			}
			break;
		}
	}
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		if (unHitTime > 0)
		{
			return;			//無敵時間中はダメージを受けない
		}
		this->unHitTime = 400;

		hp -= at_.power;
		if (hp <= 0)
		{
			this->Kill();
		}

		//吹き飛ばす
		if (pos.x > from_->pos.x)
		{
			moveVec = ML::Vec2(4, -9);
		}
		else
		{
			moveVec = ML::Vec2(-4, -9);
		}
		UpdateMotion(Bound);
	}
	//-----------------------------------------------------------------------------
	//アニメーション制御
	BChara::DrawInfo  Object::Anim()
	{
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-16, -40, 32, 80), ML::Box2D(0, 0, 32, 80), ML::Color(1, 1, 1, 1) },	//停止
			{ ML::Box2D(-4, -40, 32, 80), ML::Box2D(32, 0, 32, 80), ML::Color(1, 1, 1, 1) },	//歩行１
			{ ML::Box2D(-20, -40, 48, 80), ML::Box2D(64, 0, 48, 80), ML::Color(1, 1, 1, 1) },	//歩行２
			{ ML::Box2D(-20, -40, 48, 80), ML::Box2D(112, 0, 48, 80), ML::Color(1, 1, 1, 1) },	//歩行３
			{ ML::Box2D(-24, -40, 48, 80), ML::Box2D(48, 80, 48, 80), ML::Color(1, 1, 1, 1) },	//ジャンプ
			{ ML::Box2D(-24, -40, 48, 80), ML::Box2D(96, 80, 48, 80), ML::Color(1, 1, 1, 1) },	//落下
			{ ML::Box2D(-24, -24, 48, 64), ML::Box2D(0, 80, 48, 64), ML::Color(1, 1, 1, 1) },	//飛び立つ直前
			{ ML::Box2D(-24, -24, 48, 64), ML::Box2D(144, 80, 48, 64), ML::Color(1, 1, 1, 1) },	//着地
			{ML::Box2D(-24,-24,48,80),ML::Box2D(176,0,48,80),ML::Color(1,1,1,1)},
		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	ジャンプ------------------------------------------------------------------------
		case  Jump:		rtv = imageTable[4];	break;
		case  Jump2:	rtv = imageTable[4];	break;
			//	停止----------------------------------------------------------------------------
		case  Stand:	rtv = imageTable[0];	break;
			//	歩行----------------------------------------------------------------------------
		case  Walk:
			work = this->animCnt / 8;
			work %= 3;
			rtv = imageTable[work + 1];
			break;
			//	落下----------------------------------------------------------------------------
		case  Fall:		rtv = imageTable[5];	break;
		case  Fall2:	rtv = imageTable[5];	break;
		case  TakeOff:  rtv = imageTable[6];	break;
		case  Landing:  rtv = imageTable[7];	break;
		case  Bound:	rtv = imageTable[8];	break;
		}
		//	向きに応じて画像を左右反転する
		if (false == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
			////	表示位置の逆転
			//rtv.draw.x = -(rtv.draw.x + rtv.draw.w);
			////	画像位置の逆転
			//rtv.src.x = (rtv.src.x + rtv.src.w);
			//rtv.src.w = -rtv.src.w;
		}
		return rtv;
	}
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//リソースクラスの生成
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}