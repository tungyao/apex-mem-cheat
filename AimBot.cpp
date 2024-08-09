#include "AimBot.h"

#include "d3dguix.h"
#include "Driver.h"
#include "Entity.h"
#include "Offset.h"
#include "Math.h"
uintptr_t GamePid = 0;
uintptr_t GameBaseAddress = 0;
uintptr_t entitylist = 0;

uintptr_t Spectators = 0;

uintptr_t nextAim = 0;
uintptr_t AimTarget = 0;
uintptr_t nextEntityInfoUpdate = 0;


float current_fov_limiter = 999.f;

bool printableOut = false;

int enable_aimbot = 1;
int enable_aimbot_lock_mode = 1;
int disable_aimbot_with_spectators = 0;
int disable_aimbot_lock_mode_with_spectators = 1;
int count_team_entities_as_spectators = 1;
int enable_rect_hack = 1;
uintptr_t nextBoneSwitch = 0;
uintptr_t StartTimeToAim = 0;
int CurrentTargetBone = 3;
int targets[] = {7, 5, 3, 2};
int action = 1;
int boneIndex = 0;
Vector lastSet;
bool TargetLocked = false;

uintptr_t milliseconds_now() {
    static LARGE_INTEGER s_frequency;
    static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / s_frequency.QuadPart;
    } else {
        return GetTickCount();
    }
}

Entity *getEntity(uintptr_t g_PID, uintptr_t ptr) {
    Protect(_ReturnAddress());
    auto *entity = new Entity();
    entity->ptr = ptr;
    Unprotect(Driver::read_memory);
    Driver::read_memory(g_PID, ptr, (uintptr_t) entity->buffer, TOFFSET(ENTITY_SIZE_DEF));
    Protect(Driver::read_memory);
    Unprotect(_ReturnAddress());
    return entity;
}

void InitMouse() {
    auto mouse = mouse_open();
    if (mouse) {
        printf("init mouse success");
    } else {
        printf("init mouse failed");
    }
}

void AutoBoneSwitch() {
    Protect(_ReturnAddress());
    Unprotect(milliseconds_now);
    if (nextBoneSwitch < milliseconds_now()) {
        boneIndex += action;
        if (boneIndex == 3) {
            action = -1;
        } else if (boneIndex == 0) {
            action = 1;
        }
        CurrentTargetBone = targets[boneIndex];
        nextBoneSwitch = milliseconds_now() + 300;
    }
    Protect(milliseconds_now);
    Unprotect(_ReturnAddress());
}


void calculateScreenDimensions(
    float distance,
    float fov,
    float aspectRatio,
    float enemyWidth,
    float enemyLength,
    float &outScreenWidth,
    float &outScreenHeight
) {
    // 敌人在摄像机视角中所占的角大小（以弧度为单位）
    float angleWidth = atan((enemyWidth / 2) / distance);
    float angleHeight = atan((enemyLength / 2) / distance);

    // 敌人在屏幕上的宽度和高度（以摄像机FOV为基准）
    float screenWidth = 2 * tan(angleWidth) * distance / tan(fov * M_PI / 180);
    float screenHeight = 2 * tan(angleHeight) * distance / tan(fov * M_PI / 180);

    // 根据屏幕宽高比调整尺寸
    if (screenWidth * aspectRatio > screenHeight) {
        outScreenWidth = screenWidth / aspectRatio;
        outScreenHeight = screenHeight;
    } else {
        outScreenWidth = screenWidth;
        outScreenHeight = screenHeight * aspectRatio;
    }
}


void ProcessPlayer(Entity *LPlayer, Entity *target, UINT64 entitylist, Box_T &box, int in) {
    Protect(_ReturnAddress());
    auto fptr = &Entity::Observing;
    Unprotect((void *) *(uintptr_t *) &fptr);
    intptr_t obser = target->Observing(GamePid, entitylist);
    Protect((void *) *(uintptr_t *) &fptr);
    if (obser == LPlayer->ptr) {
        if (target->getTeamId() != LPlayer->getTeamId() ||
            target->getTeamId() == LPlayer->getTeamId() && count_team_entities_as_spectators == 1) {
            Spectators++;
        }
    }
    if (obser != 0) {
        // Is an observer... nothing to do
        Unprotect(_ReturnAddress());
        return;
    }
    auto fptrBone = &Entity::getBonePosition;
    Unprotect((void *) *(uintptr_t *) &fptrBone);
    Vector BonePosition = target->getBonePosition(GamePid, 3);
    Protect((void *) *(uintptr_t *) &fptrBone);
    Vector LocalPlayerPosition = LPlayer->getPosition();
    // 距离目标的距离
    float dist = LocalPlayerPosition.DistTo(BonePosition) / 39.62f;
    if (dist > Max_Cheat_Distance || BonePosition.z > 22000.f) {
        Unprotect(_ReturnAddress());
        return;
    }

    int health = target->getHealth();
    if (health < 0 || health > 100) {
        Unprotect(_ReturnAddress());
        return;
    }

    int entity_team = target->getTeamId();
    if (entity_team < 0 || entity_team > 31) {
        Unprotect(_ReturnAddress());
        return;
    }

    if (enable_rect_hack == 1) {
        // 绘制敌人边框
        // target->buffer
        float w;
        float h;
        calculateScreenDimensions(dist, 96.f, screenAspectRatio, enemyWidth, enemyLength, w, h);
        int x[4] = {(int) BonePosition.x, (int) BonePosition.y, (int) w, (int) h};
        memcpy(box.data[in], x, sizeof(x));
    }


    if (enable_aimbot == 1) {
        if (target->isBleedOut() || !target->isOkLifeState()) {
            Unprotect(_ReturnAddress());
            return;
        }

        if (entity_team == LPlayer->getTeamId()) {
            Unprotect(_ReturnAddress());
            return;
        }

        Vector BreathAngles = LPlayer->GetBreathAngles();
        Vector LocalCamera = LPlayer->GetCamPos();
        Vector Angle = Math::CalcAngle(LocalCamera, BonePosition);
        float fov = (float) Math::GetFov(BreathAngles, Angle, dist);
        if (fov < current_fov_limiter) {
            AimTarget = target->ptr;
            current_fov_limiter = fov;
        }
    }
    Unprotect(_ReturnAddress());
}

void PredictPosition(Entity *LocalPlayer, Entity *target, Vector *BonePosition) {
    Protect(_ReturnAddress());
    uintptr_t current_weapon = LocalPlayer->CurrentWeapon(GamePid, entitylist);
    if (current_weapon != 0) {
        //if weapon found apply gravity & speed calculation
        float bulletSpeed = Driver::read<float>(GamePid, current_weapon + TOFFSET(OFFSET_BULLET_SPEED));
        float bulletGravity = Driver::read<float>(GamePid, current_weapon + TOFFSET(OFFSET_BULLET_GRAVITY));

        if (bulletSpeed > 1.0f) {
            //fix for charge rifle
            Vector muzzle = LocalPlayer->GetCamPos();
            float Time = BonePosition->DistTo(muzzle) / bulletSpeed;
            BonePosition->z += (700.f * bulletGravity * 0.5f) * (Time * Time);
            Vector velocity_delta = (target->GetVelocity() * Time);
            BonePosition->x += velocity_delta.x;
            BonePosition->y += velocity_delta.y;
            BonePosition->z += velocity_delta.z;
        }
    }
    Unprotect(_ReturnAddress());
}

void UpdatePlayersInfo(Entity *LocalPlayer) {
    Protect(_ReturnAddress());

    current_fov_limiter = 999.f;
    AimTarget = 0;
    Spectators = 0;
    Box_T box{};
    for (int i = 0; i <= 40; i++) {
        uintptr_t centity = Driver::read<uintptr_t>(GamePid, entitylist + ((uintptr_t) i << 5));
        if (centity == 0) continue;
        if (LocalPlayer->ptr == centity) continue;

        Unprotect(getEntity);
        Entity *Target = getEntity(GamePid, centity);
        Protect(getEntity);

        auto fptr = &Entity::isPlayer;
        Unprotect((void *) *(uintptr_t *) &fptr);
        if (!Target->isPlayer()) {
            Protect((void *) *(uintptr_t *) &fptr);
            delete Target;
            continue;
        }
        Protect((void *) *(uintptr_t *) &fptr);

        Unprotect(ProcessPlayer);
        ProcessPlayer(LocalPlayer, Target, entitylist, box, i);
        Protect(ProcessPlayer);
        delete Target;
    }
    Protect(pushBox);
    pushBox(box);
    Unprotect(pushBox);
    Unprotect(_ReturnAddress());
}

void SmoothType_Asist(float fov, float TargetDistance, Vector *Delta, float smooth_multiplier) {
    Protect(_ReturnAddress());
    float smooth = 0.f;
    if (TargetDistance < 10.f) {
        smooth = 6.f + (smooth_multiplier - 1.f) * 3.f;
    } else {
        smooth = 6.f + (2.f + (smooth_multiplier - 1.f)) * fov;
    }
    if (smooth > 0.1f) {
        Delta->x /= smooth;
        Delta->y /= smooth;
        Delta->z /= smooth;
    }
    Unprotect(_ReturnAddress());
}

void SmoothType_TargetLock(float fov, float TargetDistance, Vector *Delta, float smooth_multiplier) {
    Protect(_ReturnAddress());
    if (!TargetLocked) {
        Unprotect(milliseconds_now);
        uintptr_t transcurrido = milliseconds_now() - StartTimeToAim;
        Protect(milliseconds_now);
        int max_time = (int) smooth_multiplier * 200;
        if (max_time > 1000) {
            max_time = 1000;
        }
        int restante = (int) (max_time - transcurrido);
        if (restante > 13) {
            float smooth = restante / 15.f;
            if (smooth > 1.f) {
                Delta->x /= smooth;
                Delta->y /= smooth;
                Delta->z /= smooth;
            }
        } else {
            //time passed
            TargetLocked = true;
        }
    }
    Unprotect(_ReturnAddress());
}

int AimAngles(Entity *LocalPlayer, Entity *target, Vector *out) {
    Protect(_ReturnAddress());
    Vector LocalCamera = LocalPlayer->GetCamPos();
    auto fptr = &Entity::getBonePosition;
    Unprotect((void *) *(uintptr_t *) &fptr);
    Vector BonePosition = target->getBonePosition(GamePid, CurrentTargetBone);
    Protect((void *) *(uintptr_t *) &fptr);
    Vector EntityPosition = target->getPosition();
    if (BonePosition.x == 0 || BonePosition.y == 0 || //check wrong player position and bone position
        LocalCamera.x == 0 || LocalCamera.y == 0 || //check wrong camera
        (BonePosition.x == EntityPosition.x && BonePosition.y == EntityPosition.y) //checks wrong bone position
    ) {
        Unprotect(_ReturnAddress());
        return 0;
    }
    Unprotect(PredictPosition);
    PredictPosition(LocalPlayer, target, &BonePosition);
    Protect(PredictPosition);

    Vector CalculatedAngles = Math::CalcAngle(LocalCamera, BonePosition);
    Vector ViewAngles = LocalPlayer->GetViewAngles();
    Vector DynBreath = LocalPlayer->GetBreathAngles();

    if (DynBreath.x == 0 || DynBreath.y == 0 || //Something was wrong
        ViewAngles.x == 0 || ViewAngles.y == 0) {
        Unprotect(_ReturnAddress());
        return 0;
    }
    Math::NormalizeAngles(DynBreath);

    Vector LocalPlayerPosition = LocalPlayer->getPosition();
    float TargetDistance = LocalPlayerPosition.DistTo(EntityPosition) / 39.62f;

    double fov = Math::GetFov(DynBreath, CalculatedAngles, TargetDistance);
    //fov based in distance to the target and angles (like create an sphere around the target, fov is the radius
    if (fov > 6.f || TargetDistance > Max_Cheat_Distance) {
        Unprotect(_ReturnAddress());
        return 0;
    }

    Vector Delta = CalculatedAngles - DynBreath;
    Math::NormalizeAngles(Delta);

    Unprotect(AutoBoneSwitch);
    AutoBoneSwitch();
    Protect(AutoBoneSwitch);

    Vector RecoilVec = LocalPlayer->GetRecoil();
    if (RecoilVec.x != 0 || RecoilVec.y != 0) {
        Delta -= (RecoilVec * 0.05f); //only a little as we are already fixing the recoil with breath angles
        Math::NormalizeAngles(Delta);
    }

    float fov2 = (float) Math::GetFov2(DynBreath, CalculatedAngles);
    if ((!(enable_aimbot_lock_mode == 1)) || Spectators > 0 && disable_aimbot_lock_mode_with_spectators == 1) {
        Unprotect(SmoothType_Asist);
        SmoothType_Asist(fov2, TargetDistance, &Delta, SMOOTH);
        Protect(SmoothType_Asist);
    } else {
        Unprotect(SmoothType_TargetLock);
        SmoothType_TargetLock(fov2, TargetDistance, &Delta, SMOOTH);
        Protect(SmoothType_TargetLock);
    }

    Math::NormalizeAngles(Delta);


    Vector SmoothedAngles = ViewAngles + Delta;
    Math::NormalizeAngles(SmoothedAngles);
    if (lastSet == SmoothedAngles) {
        Unprotect(_ReturnAddress());
        return 2;
    }
    out->x = SmoothedAngles.x;
    out->y = SmoothedAngles.y;
    out->z = SmoothedAngles.z;
    Unprotect(_ReturnAddress());
    return 1;
}

void CheatLoop() {
    Protect(_ReturnAddress());
    entitylist = GameBaseAddress + TOFFSET(OFFSET_ENTITYLIST);
    uintptr_t lastAimTarget = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        //Sleep(300);//每300ms更新一次实体列表,降低CPU开销

        uintptr_t lptr = Driver::read<uintptr_t>(GamePid, GameBaseAddress + TOFFSET(OFFSET_LOCAL_ENT));
        if (lptr == 0) break;

        Unprotect(getEntity);
        Entity *LocalPlayer = getEntity(GamePid, lptr);
        Protect(getEntity);


        //(char*)(LocalPlayer->buffer + OFFSET_NAME)

        auto fptr = &Entity::isPlayer;
        Unprotect((void *) *(uintptr_t *) &fptr);
        if (!LocalPlayer->isPlayer()) {
            Protect((void *) *(uintptr_t *) &fptr);
            delete LocalPlayer;
            break;
        }
        Protect((void *) *(uintptr_t *) &fptr);

        Unprotect(milliseconds_now);
        if (nextEntityInfoUpdate < milliseconds_now()) {
            Protect(milliseconds_now);

            Unprotect(UpdatePlayersInfo);
            UpdatePlayersInfo(LocalPlayer);
            Protect(UpdatePlayersInfo);

            if (Spectators > 0 && printableOut) {
                char sp_str[] = {'S', 'p', 'e', 'c', 't', 'a', 't', 'o', 'r', 's', ':', ' ', '%', 'l', 'l', 'u', '\0'};
                printf(sp_str, Spectators);
                memset(sp_str, 0, sizeof(sp_str));
            }

            Unprotect(milliseconds_now);
            nextEntityInfoUpdate = milliseconds_now() + 200; //update info every 200ms
            Protect(milliseconds_now);
        }
        if (enable_aimbot == 1 && Spectators == 0) {
            Unprotect(milliseconds_now);
            bool key_pressed = (GetKeyState(VK_RBUTTON) & 0x8000);
            if (AimTarget > 0 && key_pressed && nextAim < milliseconds_now() && (
                    Spectators > 0 && !(disable_aimbot_with_spectators == 1) || Spectators == 0)) {
                Protect(milliseconds_now);

                if (lastAimTarget != AimTarget) {
                    TargetLocked = false;
                    Unprotect(milliseconds_now);
                    StartTimeToAim = milliseconds_now();
                    Protect(milliseconds_now);
                    lastAimTarget = AimTarget;
                }
                Unprotect(getEntity);
                Entity *target = getEntity(GamePid, AimTarget);
                Protect(getEntity);

                Vector result = {0.f, 0.f, 0.f};

                Unprotect(AimAngles);
                int status = AimAngles(LocalPlayer, target, &result);
                Protect(AimAngles);
                if (status == 1) {
                    // 1 = movement needed, 2 = view already there, 0 = some out of aimbot params
                    moveR(result.x,result.y);
                    // LocalPlayer->SetViewAngles(GamePid, result);
                } else if (status == 0) {
                    TargetLocked = false;
                    Unprotect(milliseconds_now);
                    StartTimeToAim = milliseconds_now();
                    Protect(milliseconds_now);
                }

                delete target;
                Unprotect(milliseconds_now);
                nextAim = milliseconds_now() + 16; //60 movements per second
                Protect(milliseconds_now);
            } else if (!key_pressed || AimTarget == 0) {
                TargetLocked = false;
                Unprotect(milliseconds_now);
                StartTimeToAim = milliseconds_now();
                Protect(milliseconds_now);

                ProtectedSleep(2);
            }
        }
    }
    Unprotect(_ReturnAddress());
}
