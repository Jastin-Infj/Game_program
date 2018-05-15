#pragma warning(disable:4996)
#pragma once
//-----------------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-----------------------------------------------------------------------------
#include "GameEngine_Ver3_7.h"

class BChara : public BTask
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;
public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラクタ共通メンバ変数
	ML::Vec2    pos;		//キャラクタ位置
	ML::Box2D   hitBase;	//あたり判定範囲
	ML::Vec2	moveVec;	//移動ベクトル
	int			moveCnt;	//行動カウンタ
	//左右の向き（2D横視点ゲーム専用）
	enum Angle_LR { Left, Right };
	Angle_LR	angle_LR;
	WP			target;
	//キャラクタの行動状態フラグ
	enum Motion
	{
		Unnon = -1,	//	無効(使えません）
		Stand,		//	停止
		Walk,		//	歩行
		Attack,		//	攻撃
		Jump,		//	ジャンプ
		Jump2,		//　ジャンプ２
		Fall,		//	落下
		Fall2,		//　落下２
		TakeOff,	//	飛び立つ瞬間
		Landing,	//	着地
		Turn,		//	ターンする
		Bound,		//　ダメージを受けて吹き飛んでいる
	};
	Motion			motion;			//	現在の行動を示すフラグ
	Motion          preMotion;		//前のモーションを保存する
	int             preMoveCnt;		//前のフレームを保存する
	int				animCnt;		//アニメーションカウンタ
	float			jumpPow;		//	ジャンプ初速
	float			maxFallSpeed;	//	落下最大速度
	float			gravity;		//	フレーム単位の加算量
	float			maxSpeed;		//	左右方向への移動の加算量
	float			addSpeed;		//	左右方向への移動の加算量
	float			decSpeed;		//	左右方向への移動の減衰量
	int             hp;				//体力
	//追加関数//
	int unHitTime;


	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加する事★★
	BChara()
		: pos(0, 0)
		, hitBase(0, 0, 0, 0)
		, moveVec(0, 0)
		, moveCnt(0)
		, angle_LR(Right)
		, motion(Stand)
		, jumpPow(0)
		, maxFallSpeed(0)
		, gravity(0)
		, maxSpeed(0)
		, addSpeed(0)
		, decSpeed(0)
		, hp(1)
		, unHitTime(0)
	{
    }
	virtual  ~BChara(){}

	//	アニメーション情報構造体
	struct DrawInfo {
		ML::Box2D		draw, src;
		ML::Color		color;
	};
	//攻撃情報構造体
	struct AttackInfo
	{
		int	            power;		//攻撃の威力
		int             hit;		//命中精度
		int             element;	//攻撃の属性
	};

	//キャラクタ共通メソッド
	//めり込まない移動処理
	virtual  void  CheckMove(ML::Vec2&  est_);
	//足元接触判定
	virtual  bool  CheckFoot();
	//頭上接触判定
	virtual  bool  CheckHead();
	//横軸判定
	virtual bool   CheckFrout_LR();
	//正面足元チェック
	virtual bool   CheckFroutFoot_LR();
	//接触時の応答処理
	virtual void   Received(BChara* from_, AttackInfo at_);
	//接触判定
	virtual bool   CheckHit(const ML::Box2D& hit_);
	//モーションを更新（変更なしの場合	false)
	bool  UpdateMotion(Motion  nm_);
};
