//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Sprite.h"
#include  "Task_Map2D.h"

namespace  Sprite
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imageName = "SpriteImg";
		DG::Image_Create(imageName, "./data/image/妖精.png");
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
		//ターゲットのアドレス値の取得
		if (auto tg = this->target.lock())
		{
			ML::Vec2 toVec;
			if(tg->angle_LR == Player::Object::Angle_LR::Left)
			{
				toVec = tg->pos - pos - ML::Vec2(100, 0);
			}
			else
			{
				toVec = tg->pos - pos + ML::Vec2(100, 0);
			}

			//カメラの位置を再調整
			{
				//自分を画面の何処に置くか（今回は画面中央）
				int  px = ge->camera2D.w / 2;
				int  py = ge->camera2D.h / 2;
				//自分を画面中央に置いた時のカメラの左上座標を求める
				int  cpx = int(this->pos.x) - px;
				int  cpy = int(this->pos.y) - py;
				//カメラの座標を更新
				ge->camera2D.x = cpx;
				ge->camera2D.y = cpy;
			}
			//マップ外を写さないようにする調整処理
			auto   map = ge->GetTask_One_GN<Map2D::Object>("フィールド", "マップ");
			if (nullptr != map) {
				map->AjastCameraPos();
			}
			
			//ターゲットに５％近づく
			pos += toVec * 0.05f;
			
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(-16, -16, 32, 32);
		draw.Offset(pos);
		ML::Box2D src(0, 0, 32, 32);

		//スクロール対応
		draw.Offset(-ge->camera2D.x , -ge->camera2D.y);
		DG::Image_Draw(this->res->imageName, draw, src);
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