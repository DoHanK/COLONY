#pragma once

enum ScenePlace {
	MainPlace, SubPlace
};
/// AI
//GoalState 정의
enum AIBehaviorState {
	//단일 목적
	Idle_Goal,
	Wait_Goal,
	Wander_Goal,
	FollowPath_Goal,
	Trace_Goal,
	Hitted_Goal,
	Deaded_Goal,
	Attack_Goal,
	Jump_Goal,
	//복합 목적
	Think_Goal,
	Explore_Goal,
	WanderAndWait_Goal,
	ExploreForPlayer_Goal,
	Wander_Player_Goal,
	TraceAndAttack_Goal,
};

//애니메이션 번호 정의
enum AlienAnimationName {
	Attack_1, //내려찍기
	Attack_2, //냥냥펀지 2번씩
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
	EndAnimation,
};
enum AlienAnimationTrack {
	NOW_TRACK,
	PRE_TRACK
};
enum AlienboneIndex {
	Alien_Spider
	,polySurface10
	,Root
	,DEF_HIPS					//바운딩 ㅇ
	,DEF_LEG_BACK_01_L
	,DEF_LEG_BACK_02_L
	,DEF_LEG_BACK_03_L
	,DEF_LEG_BACK_01_R
	,DEF_LEG_BACK_02_R
	,DEF_LEG_BACK_03_R
	,DEF_LEG_FRONT_01_L
	,DEF_LEG_FRONT_02_L
	,DEF_LEG_FRONT_03_L
	,DEF_LEG_FRONT_01_R
	,DEF_LEG_FRONT_02_R
	,DEF_LEG_FRONT_03_R
	,DEF_LEG_MIDDLE_01_L
	,DEF_LEG_MIDDLE_02_L
	,DEF_LEG_MIDDLE_03_L
	,DEF_LEG_MIDDLE_01_R
	,DEF_LEG_MIDDLE_02_R
	,DEF_LEG_MIDDLE_03_R
	,DEF_SPINE_1				//바운딩 ㅇ
	,DEF_SPINE_2
	,DEF_CHEST					//바운딩 ㅇ
	,DEF_NECK_1				//바운딩 ㅇ
	,DEF_NECK_2
	,DEF_HEAD					//바운딩 ㅇ
	,DEF_JAW
	,DEF_TONGUE
	,DEF_TONGUE_001
	,DEF_TONGUE_002
	,DEF_SHOLDER_L
	,DEF_ARM_L
	,DEF_FORARM_L
	,DEF_HAND_L
	,DEF_SHOLDER_R
	,DEF_ARM_R
	,DEF_FORARM_R
	,DEF_HAND_R
	,DEF_TAIL					//바운딩 ㅇ
	,DEF_TAIL_001				//바운딩 ㅇ
	,DEF_STING
	,End
};
enum AlienJumpState {
	JUMP_PREPARE,
	JUMPING,
	JUMP_LANDING,
	JUMP_END
};


//이동 속도
#define AlienAccel		3.0f
#define AlienMaxXZSpeed 2.0f
#define AlienSpinSpeed  180.0f
#define friction		2.0f

#define AlienJumpAccel	5.0f
#define AlienMaxJumpXZSpeed 20.0f

//AI 시각 범위
#define AISIGHTRANGE	15.f
#define AIFOV			60.f
#define AIFOVHEIGHT		0.4f

#define AISIGHTHEIGHT	0.9f

#define PlayerRange	    0.4f
////////////////////////////

//Player 
//애니메이션 번호 정의
enum PlayerAnimationName {
	IDLE_RIFLE,
	IDLE_GUNPLAY,
	IDLE_RELOAD,
	IDLE_PICK_UP,
	IDLE_JUMP,
	RUNNING_JUMP,
	BREAK_DANCE,
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
enum PlayerAnimationTrack {
	NOW_UPPERTRACK,
	NOW_LOWERTRACK,
	PRE_UPPERTRACK,
	PRE_LOWERTRACK
};

enum WeaponPosition {
	RIGHT_HAND,
	SPINE_BACK
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
static std::uniform_real_distribution<float> floatEndureDis(30.f, 45.f);
static std::uniform_real_distribution<float> floatSignDis(-1.0f, 1.0f);
static std::uniform_real_distribution<float> MonsterSizeDis(2.0f, 4.0f);
static std::uniform_int_distribution<>	 RandomItemType(0,6);
static std::uniform_int_distribution<> IdleRandom(0, 6);
static std::uniform_int_distribution<> JumpRandom(0, 200);
