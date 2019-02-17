#pragma once

#include "..\SDK\GameEvents.h"

namespace SDK
{
	class CUserCmd;
	class CBaseEntity;
	class CBaseWeapon;
}

class CVisuals {
public:
	bool PrecacheModel(const char* szModelName);
	void hitmarkerdynamic_paint();
	void Drawmodels();
	void Drawmodelsweapons();
	void CustomModels(SDK::CBaseEntity* entity);
	void CustomModelsweapons(SDK::CBaseWeapon* entity);
	void Draw();
	void penetration_reticle();
	void ClientDraw();
	void apply_clantag();
	void DrawInaccuracy();
	void DrawBulletBeams();
	void ModulateWorld();
	void DrawCircleLBY(CColor color);
	void ModulateSky();
	void awallcrsshiar();
	void DrawWeapon2(SDK::CBaseEntity * entity, CColor color, int index, Vector pos, Vector top);
	void set_hitmarker_time(float time);
	//void Watermark();
	void DrawDamageIndicator();
	struct damage_indicator_t {
		int dmg;
		bool initializes;
		float earse_time;
		float last_update;
		SDK::CBaseEntity * player;
		Vector Position;
	};
	void AsusProps();
	std::vector<damage_indicator_t> dmg_indicator;
	void DrawIndicator();
	void Clantag();
	void watermark();
	void viewmodelxyz();
	//void aimlines();
	//void LagCompHitbox(SDK::CBaseEntity* entity, int index);
private:
	bool disconnect = true;
	void DrawBox(SDK::CBaseEntity* entity, CColor color, Vector pos, Vector top);
	void DrawName(SDK::CBaseEntity* entity, CColor color, int index, Vector pos, Vector top);
	void DrawWeapon(SDK::CBaseEntity * entity, CColor color, int index, Vector pos, Vector top);
	void DrawHealth(SDK::CBaseEntity * entity, CColor color, CColor dormant, Vector pos, Vector top);
	void BombPlanted(SDK::CBaseEntity * entity);
	void DrawDropped(SDK::CBaseEntity * entity);
	void DrawAmmo(SDK::CBaseEntity * entity, CColor color, CColor dormant, Vector pos, Vector top);
	float resolve_distance(Vector src, Vector dest);
	void DrawDistance(SDK::CBaseEntity * entity, CColor color, Vector pos, Vector top);
	void DrawInfo(SDK::CBaseEntity * entity, CColor color, CColor alt, Vector pos, Vector top);
	void DrawFovArrows(SDK::CBaseEntity* entity, CColor color);
	void DrawCrosshair();
	void DrawLBY(SDK::CBaseEntity * entity, CColor color, CColor dormant);
	void DrawHitmarker();
	void DrawBorderLines();
public:
	std::vector<std::pair<int, float>>				Entities;
	std::deque<UTILS::BulletImpact_t>				Impacts;
};

extern CVisuals* visuals;