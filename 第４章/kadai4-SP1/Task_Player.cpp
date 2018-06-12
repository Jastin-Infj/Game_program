//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map2D.h"
#include  "Task_Shot01.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imageName = "PlayerImg";
		DG::Image_Create(this->imageName, "./data/image/chara02.png");
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
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = ML::Box2D(-16, -40, 32, 80);
		this->angle_LR = Right;
		this->controllerName = "P1";

		this->motion = Stand;		//�L�����������
		this->hp = 10;				//HP�̐ݒ�
		this->maxSpeed = 5.0f;		//�ő�ړ����x�i���j
		this->addSpeed = 1.0f;		//���s�����x�i�n�ʂ̉e���ł�����x�ł��������
		this->decSpeed = 0.5f;		//�ڒn��Ԃ̎��̑��x�����ʁi���C
		this->maxFallSpeed = 10.0f;	//�ő嗎�����x
		this->jumpPow = -10.0f;		//�W�����v�́i�����j
		this->gravity = ML::Gravity(32) * 5; //�d�͉����x�����ԑ��x�ɂ����Z��

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
		this->moveCnt++;
		this->animCnt++;
		if (unHitTime > 0)
		{
			unHitTime--;
		}
		//�v�l�E�󋵔��f
		this->Think();
		//�����[�V�����ɑΉ���������
		this->Move();
		//�߂荞�܂Ȃ��ړ�
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);

		//�����蔻��
		{
			ML::Box2D me = hitBase.OffsetCopy(pos);
			auto targets = ge->GetTask_Group_G<BChara>("�A�C�e��");
			for (auto it = targets->begin(); it != targets->end(); ++it)
			{
				if ((*it)->CheckHit(me))
				{
					//����Ƀ_���[�W�̏������s�킹��
					BChara::AttackInfo at = { 0,0,0 };
					(*it)->Received(this, at);
					break;
				}
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
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (this->unHitTime > 0)
		{
			if ((unHitTime / 4) % 2 == 0)
			{
				return;		//�W�t���[�����S�t���[���摜��\�����Ȃ�
			}
		}
		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//�X�N���[���Ή�
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		DG::Image_Draw(this->res->imageName, di.draw, di.src);
	}
	//-----------------------------------------------------------------------------
	//�v�l���󋵔��f�@���[�V��������
	void  Object::Think()
	{
		auto  in = DI::GPad_GetState(this->controllerName);
		BChara::Motion  nm = this->motion;	//�Ƃ肠�������̏�Ԃ��w��

		//�v�l�i���́j��󋵂ɉ����ă��[�V������ύX���鎖��ړI�Ƃ��Ă���B
		//���[�V�����̕ύX�ȊO�̏����͍s��Ȃ�
		switch (nm) {
		case  Stand:	//�����Ă���
			//���s
			if (in.LStick.L.on) { nm = Walk; }
			if (in.LStick.R.on) { nm = Walk; }
			if (in.B4.down) { nm = Attack; }
			//�W�����v
			if (in.B1.down) { nm = Jump; }
			if (!CheckFoot()) { nm = Fall; }//���� ��Q�@����
			break;
		case  Walk:		//�����Ă���
			if (in.LStick.L.off && in.LStick.R.off) { nm = Stand; }
			if (in.B1.down) { nm = Jump; }
			if (in.B4.down) { nm = Attack; }
			if (!CheckFoot()) { nm = Fall; }
			break;
		case  Jump:		//�㏸��
			if (in.B1.down) { nm = Jump2; }
			if (moveVec.y > 0) { nm = Fall; }		//�㏸�������Ƃ�
			if (CheckFoot()) { nm = Fall; }			//�n�ʂɓ�������
			if (moveVec.y == 0) { nm = Fall; }		//�V��ɓ�������
			break;
		case Jump2:
			if (moveVec.y > 0) { nm = Fall2; }		//��������
			if (CheckFoot()) { nm = Fall2; }		//�n�ʂɓ�������
			if (moveVec.y == 0) { nm = Fall2; }		//�V��ɓ�������
			break;
		case  Fall:		//������
			///Player���n�ʂɗ����[�V����������܂�
			if (in.B1.down) { nm = Jump2; }			//�W�����v�Q
			if (CheckFoot()) { nm = Landing; }		//Player�̏�Ԃ�ω�������@�����@��Q�@�L��
			break;
		case Fall2:
			if (CheckFoot()) { nm = Landing; }
			break;
		case  Attack:	//�U����
			if (moveCnt == 8)
			{
				nm = Stand;
			}
			break;
		case  TakeOff:	//��ї���
			if (in.B1.on)
			{
				if (moveCnt >= 30) { nm = Jump; }
			}
			else if (in.B1.off) { nm = Jump; }

			if (!CheckFoot()) { nm = Fall; }
			break;
		case  Landing:	//���n
			if (moveCnt >= 3) { nm = Stand; }
			break;
		case  Bound:
			if (moveCnt >= 12 && CheckFoot())
			{
				nm = Stand;
			}
			break;
		}
		//���[�V�����X�V
		this->UpdateMotion(nm);						//���[�V�������؂�ւ��
	}
	//-----------------------------------------------------------------------------
	//	���[�V�����ɑΉ���������
	//(���[�V�����͕ύX���Ȃ��j
	void  Object::Move()
	{
		auto  in = DI::GPad_GetState(this->controllerName);
		//�d�͉���
		switch (this->motion) {
		default:
			//�㏸���������͑����ɒn�ʂ�����
			if (this->moveVec.y < 0 ||
				this->CheckFoot() == false) {
				//�������x�̕ω�
				this->moveVec.y = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
			}
			//�n�ʂɐڐG���Ă���
			else {
				this->moveVec.y = 0.0f;
			}
			break;
			//�d�͉����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۖ����j
		case Unnon:	break;
		}

		//�ړ����x����
		switch (this->motion) {
		default:
			//�ړ��x�N�g����X����0�ł͂Ȃ��Ƃ��A�O�ɋߕt���Ă�������
			if (moveVec.x < 0)
			{
				moveVec.x = min(moveVec.x + decSpeed, 0);
			}
			else
			{
				moveVec.x = max(moveVec.x - decSpeed, 0);
			}
			break;
			//�ړ����x�����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۖ����j
		case Bound:
		case Unnon:	break;
		}
		//-----------------------------------------------------------------
		//���[�V�������ɌŗL�̏���
		switch (this->motion) {
		case  Stand:	//�����Ă���
			break;
		case  Walk:		//�����Ă���
			//������ω�������E�ړ��x�N�g����ݒ肷��
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
		case  Fall:		//������
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
		case  Jump:		//�㏸��
			if (moveCnt == 0)
			{
				//�����̐ݒ�
				moveVec.y = this->jumpPow;
			}
			if (CheckHead())				//�V��ɐڐG���Ă���
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
				//�����̐ݒ�
				moveVec.y = this->jumpPow * 0.8f;
			}
			if (CheckHead())				//�V��ɐڐG���Ă���
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
		case  Attack:	//�U����
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
			return;			//���G���Ԓ��̓_���[�W���󂯂Ȃ�
		}
		this->unHitTime = 400;

		hp -= at_.power;
		if (hp <= 0)
		{
			this->Kill();
		}

		//������΂�
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
	//�A�j���[�V��������
	BChara::DrawInfo  Object::Anim()
	{
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-16, -40, 32, 80), ML::Box2D(0, 0, 32, 80), ML::Color(1, 1, 1, 1) },	//��~
			{ ML::Box2D(-4, -40, 32, 80), ML::Box2D(32, 0, 32, 80), ML::Color(1, 1, 1, 1) },	//���s�P
			{ ML::Box2D(-20, -40, 48, 80), ML::Box2D(64, 0, 48, 80), ML::Color(1, 1, 1, 1) },	//���s�Q
			{ ML::Box2D(-20, -40, 48, 80), ML::Box2D(112, 0, 48, 80), ML::Color(1, 1, 1, 1) },	//���s�R
			{ ML::Box2D(-24, -40, 48, 80), ML::Box2D(48, 80, 48, 80), ML::Color(1, 1, 1, 1) },	//�W�����v
			{ ML::Box2D(-24, -40, 48, 80), ML::Box2D(96, 80, 48, 80), ML::Color(1, 1, 1, 1) },	//����
			{ ML::Box2D(-24, -24, 48, 64), ML::Box2D(0, 80, 48, 64), ML::Color(1, 1, 1, 1) },	//��ї����O
			{ ML::Box2D(-24, -24, 48, 64), ML::Box2D(144, 80, 48, 64), ML::Color(1, 1, 1, 1) },	//���n
			{ML::Box2D(-24,-24,48,80),ML::Box2D(176,0,48,80),ML::Color(1,1,1,1)},
		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	�W�����v------------------------------------------------------------------------
		case  Jump:		rtv = imageTable[4];	break;
		case  Jump2:	rtv = imageTable[4];	break;
			//	��~----------------------------------------------------------------------------
		case  Stand:	rtv = imageTable[0];	break;
			//	���s----------------------------------------------------------------------------
		case  Walk:
			work = this->animCnt / 8;
			work %= 3;
			rtv = imageTable[work + 1];
			break;
			//	����----------------------------------------------------------------------------
		case  Fall:		rtv = imageTable[5];	break;
		case  Fall2:	rtv = imageTable[5];	break;
		case  TakeOff:  rtv = imageTable[6];	break;
		case  Landing:  rtv = imageTable[7];	break;
		case  Bound:	rtv = imageTable[8];	break;
		}
		//	�����ɉ����ĉ摜�����E���]����
		if (false == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
			////	�\���ʒu�̋t�]
			//rtv.draw.x = -(rtv.draw.x + rtv.draw.w);
			////	�摜�ʒu�̋t�]
			//rtv.src.x = (rtv.src.x + rtv.src.w);
			//rtv.src.w = -rtv.src.w;
		}
		return rtv;
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