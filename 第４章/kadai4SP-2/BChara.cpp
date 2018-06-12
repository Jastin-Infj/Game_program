//-----------------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-----------------------------------------------------------------------------
#include "BChara.h"
#include  "MyPG.h"
#include  "Task_Map2D.h"

//-----------------------------------------------------------------------------
//���[�V�������X�V�i�ύX�Ȃ��̏ꍇ	false)
bool  BChara::UpdateMotion(Motion  nm_)
{
	if (nm_ == this->motion) {
		return  false;
	}
	else {
		this->preMotion = this->motion;
		this->preMoveCnt = this->moveCnt;
		this->motion = nm_;		//���[�V�����ύX
		this->moveCnt = 0;		//�s���J�E���^�N���A
		this->animCnt = 0;		//�A�j���[�V�����J�E���^�̃N���A
		return  true;
	}
}

//-----------------------------------------------------------------------------
//����ڐG����
bool  BChara::CheckHead()
{
	//�����蔻�����ɂ��ē����`�𐶐�
	ML::Box2D  head(this->hitBase.x,
		this->hitBase.y - 1,
		this->hitBase.w,
		1);
	head.Offset((int)this->pos.x, (int)this->pos.y);


	auto   map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (nullptr == map) { return false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
	return map->CheckHit(head);
}
//-----------------------------------------------------------------------------
//�߂荞�܂Ȃ��ړ�����
void BChara::CheckMove(ML::Vec2&  e_)
{
	//�}�b�v�����݂��邩���ׂĂ���A�N�Z�X
	auto   map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (nullptr == map) { return; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j

	//�����ɑ΂���ړ�
	while (e_.x != 0) {
		float  preX = this->pos.x;
		if (e_.x >= 1) { this->pos.x += 1;		e_.x -= 1; }
		else if (e_.x <= -1) { this->pos.x -= 1;		e_.x += 1; }
		else { this->pos.x += e_.x;		e_.x = 0; }
		ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
		if (true == map->CheckHit(hit)) {
			this->pos.x = preX;		//�ړ����L�����Z��
			break;
		}
	}
	//�c���ɑ΂���ړ�
	while (e_.y != 0) {
		float  preY = this->pos.y;
		if (e_.y >= 1) { this->pos.y += 1;		e_.y -= 1; }
		else if (e_.y <= -1) { this->pos.y -= 1;		e_.y += 1; }
		else { this->pos.y += e_.y;		e_.y = 0; }
		ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
		if (true == map->CheckHit(hit)) {
			this->pos.y = preY;		//�ړ����L�����Z��
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//�����ڐG����
bool  BChara::CheckFoot()
{
	//�����蔻�����ɂ��đ�����`�𐶐�
	ML::Box2D  foot(this->hitBase.x,
		this->hitBase.y + this->hitBase.h,
		this->hitBase.w,
		1);
	foot.Offset(this->pos);

	auto   map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (nullptr == map) { return false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
	return map->CheckHit(foot);
}
//���ʏՓ˔���
bool BChara::CheckFrout_LR()
{
	ML::Box2D foot;

	//�����蔻�����ɂ��ċ�`�𐶐�����
	if (angle_LR == Left)
	{
		foot = ML::Box2D(hitBase.x - 1, hitBase.y, 1, hitBase.h);
		foot.Offset(this->pos);
	}
	if (angle_LR == Right)
	{
		foot = ML::Box2D(hitBase.x + hitBase.w + 1, hitBase.y, 1, hitBase.h);
		foot.Offset(this->pos);
	}
	auto   map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (nullptr == map) { return false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
	return map->CheckHit(foot);
}
bool BChara::CheckFroutFoot_LR()
{
	ML::Box2D sideview;
	if (angle_LR == Left)
	{
		sideview = ML::Box2D(hitBase.x - 1, hitBase.y + hitBase.h, 1, 1);
		sideview.Offset(this->pos);
	}
	if (angle_LR == Right)
	{
		sideview = ML::Box2D(hitBase.x + hitBase.w, hitBase.y + hitBase.h, 1, 1);
		sideview.Offset(this->pos);
	}
	auto   map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (nullptr == map) { return false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
	return map->CheckHit(sideview);
}
void BChara::Received(BChara* from_, AttackInfo at_)
{
	ML::MsgBox("Received ��������Ă��܂���");
}
void BChara::Render_Std(const string& img_)
{
	BChara::DrawInfo  di = this->Anim();
	di.draw.Offset(this->pos);
	//�X�N���[���Ή�
	di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
	DG::Image_Draw(img_, di.draw, di.src , di.color);
}
BChara::DrawInfo BChara::Anim()
{
	DrawInfo di = {};
	ML::MsgBox("Anim ��������Ă��܂���");
	return di;
}
bool BChara::CheckHit(const ML::Box2D& hit_)
{
	ML::Box2D me = hitBase.OffsetCopy(this->pos);
	return me.Hit(hit_);
}
bool BChara::Attack_Std(const char* name, AttackInfo at_)
{
	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
	{
		auto targets = ge->GetTask_Group_G<BChara>(name);
		for (auto it = (*targets).begin(); it != (*targets).end(); ++it)
		{
			//�^�[�Q�b�g�ɓ������Ă��邩�𔻒�
			if ((*it)->CheckHit(me))
			{
				(*it)->Received(this, at_);
				return true;
			}
		}
	}
	return false;
}
