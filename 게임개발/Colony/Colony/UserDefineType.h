#pragma once




/// AI
//GoalState 정의
enum AIBehaviorState {
	Think_Goal,
	Idle_Goal,
	Wait_Goal,
	Explore_Goal,
	FollowPath_Goal,
};
//애니메이션 번호 정의
enum AlienAnimationName {
	Attack_1,
	Attack_2,
	Attack_3,
	Attack_4,
	Attack_5,
	Attack_6,
	Attack_7,
	Attack_8,
	Death_1,
	Death_2,
	Death_3,
	Hit,
	Idle_1,
	Idle_2,
	Idle_3,
	Idle_4,
	Jump,
	Range_1,
	Range_2,
	Run_1,
	Run_2,
	Walk,
	Walk_back,
	Walk_left,
	Walk_right,

};
//이동 속도
#define AlienAccel		3.0f
#define AlienMaxXZSpeed 2.0f
#define AlienSpinSpeed  1.0f
#define friction		2.0f



////////////////////////////

//Player 
//애니메이션 번호 정의
enum PlayerAnimationName {
	IDLE_RIFLE,
	IDLE_GUNPLAY,
	IDLE_RELOAD,
	IDLE_PICK_UP,
	IDLE_JUMP,
	IDLE_JUMPING,
	IDLE_LANDING,
	WALK_FORWORD,
	WALK_FORWORD_LEFT,
	WALK_FORWORD_RIGHT,
	WALK_LEFT,
	WALK_RIGHT,
	WALK_BACKWORD,
	WALK_BACKWORD_LEFT,
	WALK_BACKWORD_RIGHT,
	WALK_GUNPLAY,
	WALK_RELOAD,
	WALK_PICK_UP,
	WALK_JUMP,
	WEAPON_SWITCH_BACK,
	RUNNING,
	IDLE_NORMAL
};



//Mapsize
#define H_MAPSIZE_X 250.f
#define H_MAPSIZE_Y 250.f
//MapCellSize
#define CELL_SIZE 1.6f 
//ShowNodeNum
#define ShowNodeNum 400





//랜덤값 생성
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> floatEndureDis(10.f, 30.f);
