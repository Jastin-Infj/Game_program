//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Enemy01.h"
#include  "Task_Map2D.h"

namespace  Enemy01
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imageName = "EnemyImg01";
		DG::Image_Create(this->imageName, "./data/image/Enemy01.png");
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
		this->render2D_Priority[1] = 0.6f;
		this->hitBase = ML::Box2D(-28, -22, 56, 45);
		this->angle_LR = Left;

		this->motion = Stand;		//�L�����������
		this->maxSpeed = 2.0f;		//�ő�ړ����x�i���j
		this->addSpeed = 0.5f;		//���s�����x�i�n�ʂ̉e���ł�����x�ł��������
		this->decSpeed = 0.1f;		//�ڒn��Ԃ̎��̑��x�����ʁi���C
		this->maxFallSpeed = 10.0f;	//�ő嗎�����x
		this->jumpPow = -6.0f;		//�W�����v�́i�����j
		this->gravity = ML::Gravity(32) * 5; //�d�͉����x�����ԑ��x�ɂ����Z��
		this->hp = 20;

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

		{
			BChara::AttackInfo at = { 1,0,0 };
			if (this->Attack_Std("�v���C��", at))
			{
				//
			}
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
	//�v�l���󋵔��f�@���[�V��������
	void  Object::Think()
	{
		BChara::Motion  nm = this->motion;	//�Ƃ肠�������̏�Ԃ��w��

											//�v�l�i���́j��󋵂ɉ����ă��[�V������ύX���鎖��ړI�Ƃ��Ă���B
											//���[�V�����̕ύX�ȊO�̏����͍s��Ȃ�									
		switch (nm) {
		case  Stand:	//�����Ă���
			if (CheckFoot()) { nm = Walk; }
			break;
		case  Walk:		//�����Ă���
			//�����Ȃ��ꍇ
			if (!CheckFroutFoot_LR()) { nm = Turn; }//�����]��
			//���ʏՓ˔���
			if (CheckFrout_LR()) { nm = Turn; }
			break;
		case  Jump:		//�㏸��
			if (moveVec.y >= 0) { nm = Fall; }
			break;
		case  Fall:		//������
			if (CheckFoot()) { nm = Stand; }
			break;
		case  Attack:	//�U����
			break;
		case  TakeOff:	//��ї���
			if (CheckFoot()) { nm = Stand; }
			break;
		case  Landing:	//�󒆂���̃X�^�[�g�n�_�̏ꍇ
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
		//���[�V�����X�V
		this->UpdateMotion(nm);						//���[�V�������؂�ւ��
	}
	//-----------------------------------------------------------------------------
	//	���[�V�����ɑΉ���������
	//(���[�V�����͕ύX���Ȃ��j
	void  Object::Move()
	{
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
		case Unnon: break;
		}
		//-----------------------------------------------------------------
		//���[�V�������ɌŗL�̏���
		switch (this->motion) {
		case  Stand:	//�����Ă���

			break;
		case  Walk:		//�����Ă���
			if (angle_LR == Left)
			{
				moveVec.x = max(-maxSpeed, moveVec.x - addSpeed);
			}
			if (angle_LR == Right)
			{
				moveVec.x = min(+maxSpeed, moveVec.x + addSpeed);
			}
			break;
		case  Fall:		//������
			if (angle_LR == Left)
			{
				moveVec.x = max(-maxSpeed, moveVec.x - addSpeed);
			}
			if (angle_LR == Right)
			{
				moveVec.x = min(+maxSpeed, moveVec.x + addSpeed);
			}
			break;
		case  Jump:		//�㏸��
			if (angle_LR == Left)
			{
				moveVec.x = max(-maxSpeed, moveVec.x - addSpeed);
			}
			if (angle_LR == Right)
			{
				moveVec.x = min(+maxSpeed, moveVec.x + addSpeed);
			}
			break;
		case  Attack:	//�U����

			break;
		case Turn:
			///������ω�������
			///if��if�ōs���ƃt���[�������������Ă��܂��̂Ō�ɍs���؂�ւ����o���Ȃ��Ȃ�܂�
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
	//�A�j���[�V��������
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color dc(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-32, -24, 64, 48), ML::Box2D(0, 0, 64, 48), dc },		//��~
		{ ML::Box2D(-32, -32, 64, 64), ML::Box2D(128, 48, 64, 64), dc },	//���s�P
		};
		BChara::DrawInfo  rtv;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	�W�����v------------------------------------------------------------------------
		case  Jump:		rtv = imageTable[0];	break;
			//	��~----------------------------------------------------------------------------
		case  Stand:	rtv = imageTable[0];	break;
			//	���s----------------------------------------------------------------------------
		case  Walk:     rtv = imageTable[0];	break;
			//	����----------------------------------------------------------------------------
		case  Fall:		rtv = imageTable[1];	break;
		}
		//	�����ɉ����ĉ摜�����E���]����
		if (!this->angle_LR) {
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