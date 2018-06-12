//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Enemy01.h"
#include  "Task_Map2D.h"

namespace  Enemy01
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imageName = "EnemyImg01";
		DG::Image_Create(this->imageName, "./data/image/Enemy01.png");
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
		this->render2D_Priority[1] = 0.6f;
		this->hitBase = ML::Box2D(-28, -22, 56, 45);
		this->angle_LR = Left;

		this->motion = Stand;		//キャラ初期状態
		this->maxSpeed = 2.0f;		//最大移動速度（横）
		this->addSpeed = 0.5f;		//歩行加速度（地面の影響である程度打ち消される
		this->decSpeed = 0.1f;		//接地状態の時の速度減衰量（摩擦
		this->maxFallSpeed = 10.0f;	//最大落下速度
		this->jumpPow = -6.0f;		//ジャンプ力（初速）
		this->gravity = ML::Gravity(32) * 5; //重力加速度＆時間速度による加算量
		this->hp = 20;

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
		this->moveCnt++;
		this->animCnt++;
		if (unHitTime > 0)
		{
			unHitTime--;
		}
		//思考・状況判断
		this->Think();
		//現モーションに対応した制御
		this->Move();
		//めり込まない移動
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);

		{
			BChara::AttackInfo at = { 1,0,0 };
			if (this->Attack_Std("プレイヤ", at))
			{
				//
			}
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
	void Object::Received(BChara* from_, BChara::AttackInfo at)
	{
		if (unHitTime > 0)
		{
			return;
		}
		unHitTime = 30;

		this->hp -= at.power;
		if (CheckKill())
		{
			Kill();
			return;
		}


		if (from_->angle_LR == Left)
		{
			moveVec = ML::Vec2(-4, -9);
		}
		else
		{
			moveVec = ML::Vec2(4, -9);
		}

		this->UpdateMotion(Bound);
	}
	bool Object::CheckKill()
	{
		return hp <= 0 ? true : false;
	}
	//-----------------------------------------------------------------------------
	//思考＆状況判断　モーション決定
	void  Object::Think()
	{
		BChara::Motion  nm = this->motion;	//とりあえず今の状態を指定

											//思考（入力）や状況に応じてモーションを変更する事を目的としている。
											//モーションの変更以外の処理は行わない									
		switch (nm) {
		case  Stand:	//立っている
			if (CheckFoot()) { nm = Walk; }
			break;
		case  Walk:		//歩いている
			//床がない場合
			if (!CheckFroutFoot_LR()) { nm = Turn; }//方向転換
			//正面衝突判定
			if (CheckFrout_LR()) { nm = Turn; }
			break;
		case  Jump:		//上昇中
			if (moveVec.y >= 0) { nm = Fall; }
			break;
		case  Fall:		//落下中
			if (CheckFoot()) { nm = Stand; }
			break;
		case  Attack:	//攻撃中
			break;
		case  TakeOff:	//飛び立ち
			if (CheckFoot()) { nm = Stand; }
			break;
		case  Landing:	//空中からのスタート地点の場合
			if (!CheckFoot()) { nm = Fall; }
			break;
		case Turn:
			if (moveCnt >= 5)
			{
				nm = Stand;
			}
			break;
		case Bound:
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
		case Unnon: break;
		}
		//-----------------------------------------------------------------
		//モーション毎に固有の処理
		switch (this->motion) {
		case  Stand:	//立っている

			break;
		case  Walk:		//歩いている
			if (angle_LR == Left)
			{
				moveVec.x = max(-maxSpeed, moveVec.x - addSpeed);
			}
			if (angle_LR == Right)
			{
				moveVec.x = min(+maxSpeed, moveVec.x + addSpeed);
			}
			break;
		case  Fall:		//落下中
			if (angle_LR == Left)
			{
				moveVec.x = max(-maxSpeed, moveVec.x - addSpeed);
			}
			if (angle_LR == Right)
			{
				moveVec.x = min(+maxSpeed, moveVec.x + addSpeed);
			}
			break;
		case  Jump:		//上昇中
			if (angle_LR == Left)
			{
				moveVec.x = max(-maxSpeed, moveVec.x - addSpeed);
			}
			if (angle_LR == Right)
			{
				moveVec.x = min(+maxSpeed, moveVec.x + addSpeed);
			}
			break;
		case  Attack:	//攻撃中

			break;
		case Turn:
			///動きを変化させる
			///ifとifで行うとフレーム数が増加してしまうので後に行う切り替えが出来なくなります
			if (moveCnt == 3)
			{
				if (angle_LR == Left)
				{
					angle_LR = Right;
				}
				else
				{
					angle_LR = Left;
				}
			}
			break;
		}
	}
	//-----------------------------------------------------------------------------
	//アニメーション制御
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color dc(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-32, -24, 64, 48), ML::Box2D(0, 0, 64, 48), dc },		//停止
		{ ML::Box2D(-32, -32, 64, 64), ML::Box2D(128, 48, 64, 64), dc },	//歩行１
		};
		BChara::DrawInfo  rtv;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	ジャンプ------------------------------------------------------------------------
		case  Jump:		rtv = imageTable[0];	break;
			//	停止----------------------------------------------------------------------------
		case  Stand:	rtv = imageTable[0];	break;
			//	歩行----------------------------------------------------------------------------
		case  Walk:     rtv = imageTable[0];	break;
			//	落下----------------------------------------------------------------------------
		case  Fall:		rtv = imageTable[1];	break;
		}
		//	向きに応じて画像を左右反転する
		if (!this->angle_LR) {
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