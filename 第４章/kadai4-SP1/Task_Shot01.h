#pragma once
#pragma warning(disable:4996)
//----------------------------------------------------------------
//-----------------------�^�C�g�����------------------------------
//----------------------------------------------------------------
#include "BChara.h"

namespace Shot01
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const string defGroupName("Shot");
	const string defName("01");
	//---------------------------------------

	class Resource
	{
	private:
		bool Initialize();
		bool Finalize();
		//�R���X�g���N�^
		Resource();
	public:
		//�f�X�g���N�^
		~Resource();
		typedef shared_ptr<Resource> SP;
		typedef weak_ptr<Resource> WP;
		static WP instance;
		//�����[�X�N���X�̐���
		static Resource::SP Create();
		//���\�[�X�N���X�̕ϐ��錾
		string imageName;
	};
	//-------------------------------------
	class Object : public BChara
	{
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//�������� ������true�Ń^�X�N�V�X�e���֎����o�^
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		//�R���X�g���N�^
		Object();
		bool B_Initialize();
		bool B_Finalize();
		bool Initialize();
		void UpDate();
		void Render2D_AF();
		bool Finalize();
	public:

		//�ϐ��̒ǉ�
		ML::Vec2 pos;		//�L�����N�^�ʒu
		ML::Box2D hitBace;	//�����蔻��
		ML::Vec2 moveVec;	//�ړ��x�N�g��
		int moveCnt;		//�s���J�E���^

		
	};
}