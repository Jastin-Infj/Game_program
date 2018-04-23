//-------------------------------------------------------------------
//vC
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map2D.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[Xฬ๚ป
	bool  Resource::Initialize()
	{
		this->imageName = "PlayerImg";
		DG::Image_Create(this->imageName, "./data/image/ฏ-01.png");
		this->fontName = "FontA";
		DG::Font_Create(this->fontName, "lr SVbN", 8, 16);
		return true;
	}
	//-------------------------------------------------------------------
	//\[Xฬ๐๚
	bool  Resource::Finalize()
	{
		DG::Image_Erase(this->imageName);
		DG::Font_Erase(this->fontName);
		return true;
	}
	//-------------------------------------------------------------------
	//u๚ปv^XNถฌษP๑พฏsค
	bool  Object::Initialize()
	{
		//X[p[NX๚ป
		__super::Initialize(defGroupName, defName, true);
		//\[XNXถฌor\[XคL
		this->res = Resource::Create();

		//f[^๚ป
		this->render2D_Priority[1] = 0.6f;
		this->moveStep = 3;
		this->controllerName = "P1";

		//^XNฬถฌ

		return  true;
	}
	//-------------------------------------------------------------------
	//uIนv^XNมลษP๑พฏsค
	bool  Object::Finalize()
	{
		//f[^^XN๐๚


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//๘ซpฌ^XNฬถฌ
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[ษsค
	void  Object::UpDate()
	{
		auto  in = DI::GPad_GetState(this->controllerName);
		//ฺฎ
		if (in.LStick.L.down) { this->pos.x -= 1; }
		if (in.LStick.R.down) { this->pos.x += 1; }
		if (in.LStick.U.down) { this->pos.y -= 1; }
		if (in.LStick.D.down) { this->pos.y += 1; }
		//Jฺฎ
		if (in.RStick.L.on) { ge->camera2D.x -= 1; }
		if (in.RStick.R.on) { ge->camera2D.x += 1; }
		if (in.RStick.U.on) { ge->camera2D.y -= 1; }
		if (in.RStick.D.on) { ge->camera2D.y += 1; }
		//เฯX
		if (in.B1.down) { this->moveStep = min(this->moveStep + 1, 15); }
		if (in.B2.down) { this->moveStep = max(this->moveStep - 1, 1); }
	}
	//-------------------------------------------------------------------
	//uQc`ๆvPt[ษsค
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(0,0,16,16);
		ML::Box2D  src(0, 0, 256, 256);
		draw.Offset(this->pos * 16);
		//XN[ฮ
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		DG::Image_Draw(this->res->imageName, draw, src);

		//
		ML::Box2D   textDraw(300, 0, 200, 32);
		stringstream  ss;
		ss << "สu(" << this->pos.x << "," << this->pos.y << ")\n" << "ฺฎเ" << this->moveStep;
		string  str;
		str = ss.str();
		DG::Font_DrawF(this->res->fontName, textDraw, str, DG::x4);
	}

	//
	//ศบอ๎{IษฯXsvศ\bh
	//
	//-------------------------------------------------------------------
	//^XNถฌ๛
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//Q[GWษo^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//CjVCYษธsตฝ็Kill
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
	//\[XNXฬถฌ
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