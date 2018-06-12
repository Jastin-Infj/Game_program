#pragma once
#pragma warning(disable:4996)
//----------------------------------------------------------------
//-----------------------タイトル画面------------------------------
//----------------------------------------------------------------
#include "BChara.h"

namespace Shot01
{
	//タスクに割り当てるグループ名と固有名
	const string defGroupName("Shot");
	const string defName("01");
	//---------------------------------------

	class Resource
	{
	private:
		bool Initialize();
		bool Finalize();
		//コンストラクタ
		Resource();
	public:
		//デストラクタ
		~Resource();
		typedef shared_ptr<Resource> SP;
		typedef weak_ptr<Resource> WP;
		static WP instance;
		//リリースクラスの生成
		static Resource::SP Create();
		//リソースクラスの変数宣言
		string imageName;
	};
	//-------------------------------------
	class Object : public BChara
	{
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		//コンストラクタ
		Object();
		bool B_Initialize();
		bool B_Finalize();
		bool Initialize();
		void UpDate();
		void Render2D_AF();
		bool Finalize();
	public:

		//変数の追加
		ML::Vec2 pos;		//キャラクタ位置
		ML::Box2D hitBace;	//当たり判定
		ML::Vec2 moveVec;	//移動ベクトル
		int moveCnt;		//行動カウンタ

		
	};
}