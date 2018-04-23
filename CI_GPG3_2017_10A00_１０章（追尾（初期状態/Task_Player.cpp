//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include "Task_Map2D.h"
#include  "Task_Shot00.h"


namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imageName = "PlayerImg";
		DG::Image_Create(this->imageName, "./data/image/CharaTV.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		DG::Image_Erase(this->imageName);
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();

		//���f�[�^������
		this->controllerName = "P1";
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = ML::Box2D(-20, -20, 40, 40);
		this->moveSpeed = 4;

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		auto in = DI::GPad_GetState(this->controllerName);

		this->moveVec = ML::Vec2(cos(in.LStick.angle), sin(in.LStick.angle)) * in.LStick.volume * this->moveSpeed;
		if (in.LStick.volume > 0) {
			this->angle = in.LStick.angle;//���X�e�B�b�N�̕������L�����̌�����
		}
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);


		//�e������
		if (in.B1.down) {
			//�e�𐶐�
			auto  shot = Shot00::Object::Create(true);
			shot->set(this->pos, this->angle, 8);
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(-32, -32, 64, 64);
		draw.Offset(this->pos);
		ML::Box2D  src(0, 0, 64, 64);

		//�X�N���[���Ή�
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		//��]�Ή�(�^�񒆂���ɉ�]�j
		DG::Image_Rotation(this->res->imageName, this->angle,
			ML::Vec2(draw.w / 2.0f, draw.h / 2.0f));
		DG::Image_Draw(this->res->imageName, draw, src);
	}

	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
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
	//���\�[�X�N���X�̐���
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