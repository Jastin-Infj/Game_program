//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Map2D.h"
#include "Task_Shot01.h"
namespace  Shot01
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imageName = "Shot01Img";
		DG::Image_Create(imageName, "./data/image/Shot00.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		DG::Image_Erase(imageName);
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
		render2D_Priority[1] = 0.4f;
		pos.x = 0;
		pos.y = 0;
		hitBace = ML::Box2D(-8, -8, 16, 16);
		moveVec = ML::Vec2(0, 0);
		moveCnt = 0;
		//★タスクの生成

		//背景タスク
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
		moveCnt++;
		//限界の時間に到達したら消滅する
		if (moveCnt >= 100)
		{
			Kill();
			return;
		}
		//移動処理
		pos += moveVec;
		//重力加速
		moveVec.y += ML::Gravity(32) * 5;
		//移動先で障害物に接触したら消滅
		//マップが存在するか調べてからアクセス
		if (auto map = ge->GetTask_One_GN<Map2D::Object>("フィールド", "マップ"))
		{
			ML::Box2D hit = hitBace.OffsetCopy(pos);
			if (map->CheckHit(hit))
			{
				//消滅を依頼する
				Kill();
				return;
			}
		}

		//敵対象と衝突判定とダメージを与える処理
		//未実装


	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(-8, -8, 16, 16);
		draw.Offset(pos);
		ML::Box2D src(0, 0, 32, 32);
		//スクロール対応
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		DG::Image_Draw(res->imageName, draw,src);
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