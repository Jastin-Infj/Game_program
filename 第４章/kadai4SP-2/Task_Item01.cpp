//-------------------------------------------------------------------
//vC
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Item01.h"

namespace  Item01
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[XΜϊ»
	bool  Resource::Initialize()
	{
		this->imageName = "Item01Img";
		DG::Image_Create(this->imageName, "./data/image/Item01.png");
		return true;
	}
	//-------------------------------------------------------------------
	//\[XΜπϊ
	bool  Resource::Finalize()
	{
		DG::Image_Erase(this->imageName);
		return true;
	}
	//-------------------------------------------------------------------
	//uϊ»v^XNΆ¬ΙPρΎ―s€
	bool  Object::Initialize()
	{
		//X[p[NXϊ»
		__super::Initialize(defGroupName, defName, true);
		//\[XNXΆ¬or\[X€L
		this->res = Resource::Create();

		//f[^ϊ»
		this->render2D_Priority[1] = 0.7f;
		this->hitBase = ML::Box2D(-16, -16, 32, 32);
		this->motion = Stand;
		return  true;
	}
	//-------------------------------------------------------------------
	//uIΉv^XNΑΕΙPρΎ―s€
	bool  Object::Finalize()
	{
		//f[^^XNπϊ

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//ψ«p¬^XNΜΆ¬
			Kill();
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[Ιs€
	void  Object::UpDate()
	{
		this->moveCnt++;
		this->animCnt++;
		if (this->unHitTime > 0)
		{
			this->unHitTime--;
		}

		Motion nm = this->motion;
		switch (motion)
		{
		case BChara::Stand:
			break;
		case BChara::Lose:
			this->pos.y -= 3;
			if (moveCnt > 20)
			{
				this->Kill();
			}
			break;
		}


	}
	//-------------------------------------------------------------------
	//uQc`ζvPt[Ιs€
	void  Object::Render2D_AF()
	{
		this->Render_Std(this->res->imageName);
	}
	//-----------------------------------------------------------------------------
	void Object::Received(BChara* from_, BChara::AttackInfo at_)
	{
		if (motion != Stand)
		{
			return;
		}
		this->UpdateMotion(Lose);
		from_->jumpPow += -1;
	}
	//-----------------------------------------------------------------------------
	//Aj[V§δ
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color dc(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(0, 0, 32, 32), dc },						//β~
		{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(0, 0, 32, 32), ML::Color(0.3f,1,1,1) },		//gp
		};
		BChara::DrawInfo  rtv;
		switch (motion)
		{
		case BChara::Stand:
			rtv = imageTable[0];
			break;
		case BChara::Lose:
			rtv = imageTable[1];
			break;
		}
		return rtv;
	}
	//
	//ΘΊΝξ{IΙΟXsvΘ\bh
	//
	//-------------------------------------------------------------------
	//^XNΆ¬ϋ
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//Q[GWΙo^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//CjVCYΙΈs΅½ηKill
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
	//\[XNXΜΆ¬
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