#pragma once

//#define OFFSET_RAN 0//0x34LLU
#define TOFFSET(d1) d1 //((d1)+offset_ran_var)

extern unsigned long long offset_ran_var;

#define ENTITY_SIZE_DEF				0x42E0			//biggest entity offset

#define OFFSET_ENTITYLIST			0x1f14e08		//cl_entitylist
#define OFFSET_LOCAL_ENT			0x23e7ff8		//LocalPlayer


#define OFFSET_TEAM					0x0338			//m_iTeamNum
#define OFFSET_HEALTH				0x0328			//m_iHealth
#define OFFSET_NAME					0x0481			//m_iName
#define OFFSET_SIG_NAME				0x0478			//m_iSignifierName
#define OFFSET_SHIELD				0x01a0			//m_shieldHealth
#define OFFSET_MAX_SHIELD			0x01a4			//m_shieldHealth +0x4

#define OFFSET_LIFE_STATE			0x0690			//m_lifeState
#define OFFSET_BLEED_OUT_STATE		0x2760			//m_bleedoutState

#define OFFSET_ITEM_ID				0x1568			//m_customScriptInt

#define OFFSET_BULLET_SPEED			0x2b50			//m_flProjectileSpeed
#define OFFSET_BULLET_GRAVITY		0x1e14			//m_flProjectileScale

#define OFFSET_CURRENT_WEAPON		0x1944			//m_latestPrimaryWeapons
#define OFFSET_ORIGIN				0x0004			//m_vecAbsOrigin
#define OFFSET_BONES				0x0db0			//m_nForceBone + 0x50-0x8 -> 48 8B 97 ?? ?? ?? ?? 48 8D 04 5B 48 C1
#define OFFSET_AIMPUNCH				0x2438			//m_vecPunchWeapon_Angle
#define OFFSET_CAMERAPOS			0x1ee0			//40 57 48 83 EC 20 F3 0F 10 0A 48 8B F9 0F 2E 89 first ucomiss result
#define OFFSET_VIEWANGLES			0x2534			//m_ammoPoolCapacity - 0x14  -> 0F 57 C0 8B 86 ?? ?? ?? ?? F3 segundo uso de un 0x24AX
#define OFFSET_BREATH_ANGLES		(OFFSET_VIEWANGLES - 0x10)
#define OFFSET_OBSERVER_MODE		0x3524			//m_iObserverMode
#define OFFSET_OBSERVING_TARGET		0x3530			//m_hObserverTarget

#define OFFSET_HELMET_TYPE			0x46c0			//m_helmetType

#define GLOW_CONTEXT 0x350 //Script_Highlight_SetCurrentContext
#define GLOW_LIFE_TIME 0x32C //Script_Highlight_SetLifeTime + 4
#define GLOW_DISTANCE 0x33C //Script_Highlight_SetFarFadeDist
#define GLOW_TYPE 0x2AC //Script_Highlight_GetState + 4
#define GLOW_COLOR 0x1D0 //Script_CopyHighlightState mov tcx nº7
#define GLOW_VISIBLE_TYPE 0x360 //Script_Highlight_SetVisibilityType
#define GLOW_FADE 0x310 //Script_Highlight_GetCurrentInsideOpacity 3º result of 3 offsets consecutive or first + 8~



#define OFFSET_MATRIX				0x11a350		//ViewMatrix
#define OFFSET_RENDER				0x40d5d98 	 	//0x769a3d8