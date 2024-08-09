#ifndef AIMBOT_H
#define AIMBOT_H

#include "d3dguix.h"
#include "Entity.h"
#include "mouse.h"


const float enemyWidth = 20.0f; // 敌人的宽度
const float enemyLength = 100.0f; // 敌人的长度
const float screenAspectRatio = 16.0f / 9.0f; // 屏幕宽高比
void InitMouse();

void AutoBoneSwitch();

void ProcessPlayer(Entity *LPlayer, Entity *target, UINT64 entitylist, Box_T &box, int in);

void PredictPosition(Entity *LocalPlayer, Entity *target, Vector *BonePosition);

void UpdatePlayersInfo(Entity *LocalPlayer);

void SmoothType_Asist(float fov, float TargetDistance, Vector *Delta, float smooth_multiplier);

void SmoothType_TargetLock(float fov, float TargetDistance, Vector *Delta, float smooth_multiplier);

int AimAngles(Entity *LocalPlayer, Entity *target, Vector *out);

int AimAngles(Entity *LocalPlayer, Entity *target, Vector *out);

void CheatLoop();
#endif //AIMBOT_H
