#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/ConVar.h"
#include "../SDK/CglobalVars.h"
#include "../SDK/IViewRenderBeams.h"
#include "../FEATURES/Backtracking.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseWeapon.h"
#include "../FEATURES/AutoWall.h"
#include "../SDK/CTrace.h"	
#include "../FEATURES/Resolver.h"
#include "../SDK/CglobalVars.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Visuals.h"
#include "../UTILS/render.h"
#include "../SDK/IVDebugOverlay.h"
#include <string.h>
#include "../cheats/visuals/other_esp.h"
#include "../HOOKS/xor.h"

#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif

bool CVisuals::PrecacheModel(const char* szModelName)
{
	auto m_pModelPrecacheTable = INTERFACES::ClientStringTableContainer()->FindTable("modelprecache");

	if (m_pModelPrecacheTable)
	{
		INTERFACES::ModelInfo->FindOrLoadModel(szModelName);
		int idx = m_pModelPrecacheTable->AddString(false, szModelName);
		if (idx == INVALID_STRING_INDEX)
			return false;
	}
	return true;
}

//--- Misc Variable Initalization ---//
float flPlayerAlpha[255];
CColor breaking;
CColor backtrack;
static bool bPerformed = false, bLastSetting;
float fade_alpha[65];
float dormant_time[65];
int alpha[65];
CColor main_color;
CColor ammo;
SDK::CBaseEntity *BombCarrier;

std::vector<hitmarker_info> hitmarker;

void CVisuals::hitmarkerdynamic_paint() {

	float time = INTERFACES::Globals->curtime;

	for (int i = 0; i < hitmarker.size(); i++) {

		bool expired = time >= hitmarker.at(i).impact.time + 3.f;

		if (expired)
			hitmarker.at(i).alpha -= 1;

		if (expired && hitmarker.at(i).alpha <= 0) {

			hitmarker.erase(hitmarker.begin() + i);
			continue;
		}

		Vector pos3D = Vector(hitmarker.at(i).impact.x, hitmarker.at(i).impact.y, hitmarker.at(i).impact.z), pos2D;

		if (!RENDER::WorldToScreen(pos3D, pos2D))
			continue;

		INTERFACES::Surface->DrawSetColor(CColor(240, 200, 240, hitmarker.at(i).alpha));

		INTERFACES::Surface->DrawOutlinedCircle(pos2D.x, pos2D.y, 8, 50); // pos2D.x - (linesize / 4), pos2D.y - (linesize / 4)
		INTERFACES::Surface->DrawLine(pos2D.x - 6, pos2D.y - 6, pos2D.x - (3 / 4), pos2D.y - (3 / 4));
		INTERFACES::Surface->DrawLine(pos2D.x - 6, pos2D.y + 6, pos2D.x - (3 / 4), pos2D.y + (3 / 4));
		INTERFACES::Surface->DrawLine(pos2D.x + 6, pos2D.y + 6, pos2D.x + (3 / 4), pos2D.y + (3 / 4));
		INTERFACES::Surface->DrawLine(pos2D.x + 6, pos2D.y - 6, pos2D.x + (3 / 4), pos2D.y - (3 / 4));
	}
}

void CVisuals::Drawmodelsweapons()
{

}

void CVisuals::Drawmodels()
{
	if (SETTINGS::settings.CUSTOMMODEL)
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		SDK::CBaseEntity* local = (SDK::CBaseEntity*)INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		//auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
		for (int i = 0; i < INTERFACES::ClientEntityList->GetHighestEntityIndex(); i++)
		{
			SDK::CBaseEntity *entity = (SDK::CBaseEntity*)INTERFACES::ClientEntityList->GetClientEntity(i);
			SDK::player_info_t pinfo;
			if (entity == nullptr)
				continue;
			if (entity == local)
				continue;
			if (entity->GetIsDormant())
				continue;
			if (INTERFACES::Engine->GetPlayerInfo(i, &pinfo))
			{
				CustomModels(entity);
			}
		}
	}
}

void CVisuals::CustomModelsweapons(SDK::CBaseWeapon* weapon)
{
	if (INTERFACES::ModelInfo->GetModelIndex("m4a1.mdl"))
	{
		switch (SETTINGS::settings.m4a4model)
		{
		case 0:
			break;
		case 1:
			PrecacheModel("models/weapons/v_rif_m4a1.mdl");
			weapon->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/weapons/v_rif_m4a1.mdl"));
			break;
		}
	}
}

void CVisuals::CustomModels(SDK::CBaseEntity* entity)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	bool is_local_player = entity == local_player;
	bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;
	//auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	SDK::CBaseEntity* local = (SDK::CBaseEntity*)INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!is_teammate)
	{
		switch (SETTINGS::settings.customct)
		{
		case 0:
			break;

		case 1:
			PrecacheModel("models/player/custom_player/kuristaja/ash/ash.mdl");
			entity->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/player/custom_player/kuristaja/ash/ash.mdl"));
			break;

		case 2:
			PrecacheModel("models/player/custom_player/kuristaja/spiderman/spiderman.mdl");
			entity->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/player/custom_player/kuristaja/spiderman/spiderman.mdl"));
			break;
		case 3:
			PrecacheModel("models/player/custom_player/kuristaja/shrek/shrek.mdl");
			entity->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/player/custom_player/kuristaja/shrek/shrek.mdl"));
			break;
		case 4:
			PrecacheModel("models/player/custom_player/kaesar2018/thanos/thanos.mdl");
			entity->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/player/custom_player/kaesar2018/thanos/thanos.mdl"));
			break;
		}
	}
	else if (is_teammate)
	{
		switch (SETTINGS::settings.customtt)
		{
		case 0:
			break;

		case 1:
			PrecacheModel("models/player/custom_player/kuristaja/ash/ash.mdl");
			entity->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/player/custom_player/kuristaja/ash/ash.mdl"));
			break;
		case 2:
			PrecacheModel("models/player/custom_player/kuristaja/spiderman_classic/spiderman.mdl");
			entity->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/player/custom_player/kuristaja/spiderman_classic/spiderman.mdl"));
			break;
		case 3:
			PrecacheModel("models/player/custom_player/kuristaja/shrek/shrek.mdl");
			entity->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/player/custom_player/kuristaja/shrek/shrek.mdl"));
			break;
		case 4:
			PrecacheModel("models/player/custom_player/kaesar2018/thanos/thanos.mdl");
			entity->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/player/custom_player/kaesar2018/thanos/thanos.mdl"));
			break;
		}
	}
	/*else if (weapon->GetItemDefenitionIndex() == SDK::WEAPON_M4A1)
	{
		switch (SETTINGS::settings.m4a4model)
		{
		case 0:
			break;
		case 1:
			PrecacheModel("models/weapons/v_rif_m4a1.mdl");
			weapon->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/weapons/v_rif_m4a1.mdl"));
			break;
		}
	}
	/*else if (is_local_player)
	{
		switch (SETTINGS::settings.customlocal)
		{
		case 0:
			break;
			//couldn't get this to work, even using _arms.mdl if someone down the line gets this to work please HMU on discord - L1L_cHrOmOsOmE#3578.
			//if you are seeing this, i'm writing this on Febuary, 10th, 2019 (currently a sophmore) This is what ive devoted almost 2 months into doing, "coding". If thats even what im doing, i only get 2 hours a day to "game" AKA being on the computer, if i code all that time (usally what i do) thats how i spend my time, when i get home from school: work on my hack. when i get any free time: work on my hack. if you are reading this you must really be searching for some P shit that i have added / made, this is how i spent all my time and i hope people will lear from what i have dont and stop renaming my hacks (childware - cost $8 a month or some shit AND ITS 1:1 MY AYYWARE PASTA) have a good day :)
		case 1:
			PrecacheModel("models/player/custom_player/kuristaja/ash/ash_arms.mdl");
			entity->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/player/custom_player/kuristaja/ash/ash_arms.mdl"));
			break;
		case 2:
			PrecacheModel("models/player/custom_player/kuristaja/spiderman_classic/spiderman.mdl");
			entity->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/player/custom_player/kuristaja/spiderman_classic/spiderman.mdl"));
			break;
		case 3:
			PrecacheModel("models/player/custom_player/kuristaja/shrek/shrek.mdl");
			entity->SetModelIndex(INTERFACES::ModelInfo->GetModelIndex("models/player/custom_player/kuristaja/shrek/shrek.mdl"));
			break;
		}
	}*/
}


void CVisuals::set_hitmarker_time(float time)
{
	global::flHurtTime = time;
}

void CVisuals::penetration_reticle() {

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer()); if (!local_player) return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex())); if (!weapon) return;

	Vector aimPoint = local_player->GetEyePosition();

	int screen_x, screen_y;
	INTERFACES::Engine->GetScreenSize(screen_x, screen_y);

	if (AutoWall->GetDamagez(aimPoint) >= 1.f)//SETTINGS::settings.damage_val)
		RENDER::DrawFilledRect(screen_x / 2, screen_y / 2, 2, 2, CColor(0, 255, 0));
	else
		RENDER::DrawFilledRect(screen_x / 2, screen_y / 2, 2, 2, CColor(255, 0, 0));
}

void CVisuals::DrawDamageIndicator()
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player->GetHealth() > 0)
		return;

	float CurrentTime = local_player->GetTickBase() * INTERFACES::Globals->interval_per_tick;

	for (int i = 0; i < dmg_indicator.size(); i++) {
		if (dmg_indicator[i].earse_time < CurrentTime) {
			dmg_indicator.erase(dmg_indicator.begin() + i);
			continue;
		}

		if (!dmg_indicator[i].initializes) {
			dmg_indicator[i].Position = dmg_indicator[i].player->GetBonePosition(6);
			dmg_indicator[i].initializes = true;
		}

		if (CurrentTime - dmg_indicator[i].last_update > 0.001f) {
			dmg_indicator[i].Position.z -= (0.01f * (CurrentTime - dmg_indicator[i].earse_time));
			dmg_indicator[i].last_update = CurrentTime;

			Vector ScreenPosition;

			if (RENDER::WorldToScreen(dmg_indicator[i].Position, ScreenPosition)) {
				RENDER::DrawF(ScreenPosition.x, ScreenPosition.y, FONTS::menu_window_font4, true, true, CColor(SETTINGS::settings.dmg_ccolor[0] * 255, SETTINGS::settings.dmg_ccolor[1] * 255, SETTINGS::settings.dmg_ccolor[2] * 255, SETTINGS::settings.dmg_ccolor[3] * 255), std::to_string(dmg_indicator[i].dmg).c_str());
			}
		}

	}
}
void DrawLBYCircleTimer(int x, int y, int size, float amount_full, CColor fill)
{
	int texture = INTERFACES::Surface->CreateNewTextureID(true);
	INTERFACES::Surface->DrawSetTexture(texture);
	INTERFACES::Surface->DrawSetColor(fill);

	SDK::Vertex_t vertexes[100];
	for (int i = 0; i < 100; i++) {
		float angle = ((float)i / -100) * (M_PI * (2 * amount_full));
		vertexes[i].Init(Vector2D(x + (size * sin(angle)), y + (size * cos(angle))));
	}

	INTERFACES::Surface->DrawTexturedPolygon(100, vertexes, true);
}
void CVisuals::DrawLBY(SDK::CBaseEntity* entity, CColor color, CColor dormant)
{
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	/*if (Clientvariables->Visuals.bAmmo)
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 14);
	else*/
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);
	int enemy_hp = entity->GetHealth();
	int hp_red = 255 - (enemy_hp * 2.55);
	int hp_green = enemy_hp * 2.55;
	CColor health_color = CColor(hp_red, hp_green, 1, alpha[entity->GetIndex()]);

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	bool is_local_player = entity == local_player;
	bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

	if (is_local_player)
		return;

	if (is_teammate)
		return;

	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));

	if (!c_baseweapon)
		return;

	if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		int height = (pos.y - top.y);

		float offset = (height / 4.f) + 5;
		UINT hp = height - (UINT)((height * 3) / 100);

		static float next_lby_update[65];
		static float last_lby[65];
		if (entity->GetVelocity().Length2D() > 28)
			next_lby_update[entity->GetIndex()] = entity->GetSimTime() + 1.10f;
		else
		{
			if (next_lby_update[entity->GetIndex()] <= entity->GetSimTime())
				next_lby_update[entity->GetIndex()] = entity->GetSimTime() + 1.1f;

		}

		last_lby[entity->GetIndex()] = entity->GetLowerBodyYaw();


		float time_remain_to_update = next_lby_update[entity->GetIndex()] - entity->GetSimTime();
		float box_w = (float)fabs(height / 2);
		float width;
		width = (((time_remain_to_update * (box_w / 1.1f))));

		//	..RENDER::DrawEmptyRect((pos.x - box_w / 2) + 1, top.y + height + 3, (pos.x - box_w / 2) + box_w + 1, top.y + height + 6, dormant); //outline
		//RENDER::DrawFilledRect((pos.x - box_w / 2) + 2, top.y + height + 4, (pos.x - box_w / 2) + width + 1, top.y + height + 6, color); //ammo

		RENDER::DrawEmptyRect((pos.x - box_w / 2) + 1, pos.y + 3, (pos.x - (box_w / 2)) + box_w + 1, pos.y + 6, dormant);
		RENDER::DrawFilledRect((pos.x - box_w / 2) + 2, pos.y + 4, (pos.x - (box_w / 2)) + width + 1, pos.y + 6, color);
	}
}
#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
void CVisuals::Draw()
{
	if (!INTERFACES::Engine->IsInGame()) {
		global::flHurtTime = 0.f;
		return;
	}
	if (SETTINGS::settings.Viewmodelchanger)
		viewmodelxyz();

	DrawCrosshair();
	for (int i = 1; i <= 65; i++)
	{
		auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!entity) continue;
		if (!local_player) continue;

		bool is_local_player = entity == local_player;
		bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

		if (is_local_player) continue;
		if (entity->GetHealth() <= 0) continue;
		if (entity->GetVecOrigin() == Vector(0, 0, 0)) continue;

		//--- Colors ---//
		if (entity->GetIsDormant() && flPlayerAlpha[i] > 0) flPlayerAlpha[i] -= .3;
		else if (!entity->GetIsDormant() && flPlayerAlpha[i] < 255) flPlayerAlpha[i] = 255;

		float playerAlpha = flPlayerAlpha[i];
		int enemy_hp = entity->GetHealth();
		int hp_red = 255 - (enemy_hp * 2.55);
		int hp_green = enemy_hp * 2.55;
		CColor health_color = CColor(hp_red, hp_green, 1, playerAlpha);
		CColor dormant_color = CColor(100, 100, 100, playerAlpha);
		CColor box_color, still_health, alt_color, zoom_color, name_color, weapon_color, distance_color, arrow_col;

		static auto alpha = 0.f; static auto plus_or_minus = false;
		if (alpha <= 0.f || alpha >= 255.f) plus_or_minus = !plus_or_minus;
		alpha += plus_or_minus ? (255.f / 7 * 0.015) : -(255.f / 7 * 0.015); alpha = clamp(alpha, 0.f, 255.f);
		float arrow_colour[3] = { SETTINGS::settings.fov_col[0] * 255, SETTINGS::settings.fov_col[1] * 255, SETTINGS::settings.fov_col[2] * 255 };
		float arrowteam_colour[3] = { SETTINGS::settings.arrowteam_col[0] * 255, SETTINGS::settings.arrowteam_col[1] * 255, SETTINGS::settings.arrowteam_col[2] * 255 };

		if (entity->GetIsDormant())
		{
			main_color = dormant_color;
			still_health = health_color;
			alt_color = CColor(20, 20, 20, playerAlpha);
			zoom_color = dormant_color;
			breaking = dormant_color;
			backtrack = dormant_color;
			box_color = dormant_color;
			ammo = dormant_color;
			name_color = dormant_color;
			weapon_color = dormant_color;
			distance_color = dormant_color;
			arrow_col = dormant_color;
		}
		else if (!entity->GetIsDormant())
		{
			main_color = CColor(255, 255, 255, playerAlpha);
			still_health = health_color;
			alt_color = CColor(0, 0, 0, 165);
			zoom_color = CColor(150, 150, 220, 165);
			breaking = CColor(220, 150, 150, 165);
			backtrack = CColor(155, 220, 150, 165);
			box_color = CColor(SETTINGS::settings.box_col[0] * 255, SETTINGS::settings.box_col[1] * 255, SETTINGS::settings.box_col[2] * 255);
			ammo = CColor(61, 135, 255, 165);
			name_color = CColor(SETTINGS::settings.name_col[0] * 255, SETTINGS::settings.name_col[1] * 255, SETTINGS::settings.name_col[2] * 255);
			weapon_color = CColor(SETTINGS::settings.weapon_col[0] * 255, SETTINGS::settings.weapon_col[1] * 255, SETTINGS::settings.weapon_col[2] * 255);
			distance_color = CColor(SETTINGS::settings.distance_col[0] * 255, SETTINGS::settings.distance_col[1] * 255, SETTINGS::settings.distance_col[2] * 255);
			arrow_col = CColor(SETTINGS::settings.fov_col[0] * 255, SETTINGS::settings.fov_col[1] * 255, SETTINGS::settings.fov_col[2] * 255);
		}
		Vector min, max, pos, pos3D, top, top3D; entity->GetRenderBounds(min, max);
		pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10); top3D = pos3D + Vector(0, 0, max.z + 10);

		if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
		{
			if (!is_teammate)
			{
				if (SETTINGS::settings.box_bool) DrawBox(entity, box_color, pos, top);
				if (SETTINGS::settings.name_bool) DrawName(entity, name_color, i, pos, top);
				if (SETTINGS::settings.weap_bool) DrawWeapon(entity, weapon_color, i, pos, top);
				if (SETTINGS::settings.weap_ammo) DrawWeapon2(entity, weapon_color, i, pos, top);
				if (SETTINGS::settings.health_bool) DrawHealth(entity, still_health, alt_color, pos, top);
				//if (SETTINGS::settings.ammo_bool) DrawAmmo(entity, ammo, alt_color, pos, top);
				if (SETTINGS::settings.lby_bar) DrawLBY(entity, CColor(0, 0, 255), alt_color);
				if (SETTINGS::settings.dmg_bool) DrawDamageIndicator();
			}
			else if (is_teammate)
			{
				if (SETTINGS::settings.boxteam) DrawBox(entity, CColor(SETTINGS::settings.boxteam_col[0] * 255, SETTINGS::settings.boxteam_col[1] * 255, SETTINGS::settings.boxteam_col[2] * 255), pos, top);
				if (SETTINGS::settings.nameteam) DrawName(entity, CColor(SETTINGS::settings.nameteam_col[0] * 255, SETTINGS::settings.boxteam_col[1] * 255, SETTINGS::settings.boxteam_col[2] * 255), i, pos, top);
				if (SETTINGS::settings.weaponteam) DrawWeapon(entity, CColor(SETTINGS::settings.weaponteam_col[0] * 255, SETTINGS::settings.boxteam_col[1] * 255, SETTINGS::settings.boxteam_col[2] * 255), i, pos, top);
				if (SETTINGS::settings.healthteam) DrawHealth(entity, still_health, alt_color, pos, top);
				if (SETTINGS::settings.ammoteam) DrawAmmo(entity, ammo, alt_color, pos, top);
			}
			DrawInfo(entity, main_color, zoom_color, pos, top);
		}
		if (!is_teammate)
		{
			if (SETTINGS::settings.arrowtrypes == 1)
				if (SETTINGS::settings.fov_bool) DrawFovArrows(entity, CColor(arrow_colour[0] * 255, arrow_colour[1] * 255, arrow_colour[2] * 255, alpha));
			if (SETTINGS::settings.arrowtrypes == 0)
				if (SETTINGS::settings.fov_bool) DrawFovArrows(entity, CColor(arrow_colour[0] * 255, arrow_colour[1] * 255, arrow_colour[2] * 255, 255));
		}
		else if (is_teammate)
		{
			if (SETTINGS::settings.arrowtrypes == 1)
				if (SETTINGS::settings.arrowteam) DrawFovArrows(entity, CColor(arrowteam_colour[0] * 255, arrowteam_colour[1] * 255, arrowteam_colour[2] * 255, alpha));
			if (SETTINGS::settings.arrowtrypes == 0)
				if (SETTINGS::settings.arrowteam) DrawFovArrows(entity, CColor(arrowteam_colour[0] * 255, arrowteam_colour[1] * 255, arrowteam_colour[2] * 255, 255));
		}
	}
	//Watermark();
}

void CVisuals::ClientDraw()
{
	//if (SETTINGS::settings.spread_bool == 2) DrawInaccuracy();
	//if (SETTINGS::settings.spread_bool == 2) DrawInaccuracy1();
	if (SETTINGS::settings.scope_bool) DrawBorderLines();
	penetration_reticle();
	DrawIndicator();
	if (SETTINGS::settings.hitmarkerscreen) DrawHitmarker();
	//misc_visuals();
	//Clantag();
	static SDK::ConVar* crosshair = INTERFACES::cvar->FindVar("weapon_debug_spread_show");
	if (SETTINGS::settings.forcehair)
	{
		crosshair->SetValue(3);
	}
	else {
		crosshair->SetValue(0);
	}
}

std::string str_to_upper(std::string strToConvert)
{
	std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

	return strToConvert;
}

void CVisuals::DrawBox(SDK::CBaseEntity* entity, CColor color, Vector pos, Vector top)
{
	float alpha = flPlayerAlpha[entity->GetIndex()];
	int height = (pos.y - top.y), width = height / 2;

	RENDER::DrawEmptyRect(pos.x - width / 2, top.y, (pos.x - width / 2) + width, top.y + height, color);
	RENDER::DrawEmptyRect((pos.x - width / 2) + 1, top.y + 1, (pos.x - width / 2) + width - 1, top.y + height - 1, CColor(20, 20, 20, alpha));
	RENDER::DrawEmptyRect((pos.x - width / 2) - 1, top.y - 1, (pos.x - width / 2) + width + 1, top.y + height + 1, CColor(20, 20, 20, alpha));
}

void CVisuals::DrawName(SDK::CBaseEntity* entity, CColor color, int index, Vector pos, Vector top)
{
	SDK::player_info_t ent_info; INTERFACES::Engine->GetPlayerInfo(index, &ent_info);

	int height = (pos.y - top.y), width = height / 2;
	RENDER::DrawF(pos.x, top.y - 7, FONTS::visuals_name_font, true, true, color, ent_info.szName);
}

float CVisuals::resolve_distance(Vector src, Vector dest)
{
	Vector delta = src - dest;
	float fl_dist = ::sqrtf((delta.Length()));
	if (fl_dist < 1.0f) return 1.0f;
	return fl_dist;
}

void CVisuals::DrawDistance(SDK::CBaseEntity* entity, CColor color, Vector pos, Vector top)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	SDK::player_info_t ent_info;
	Vector vecOrigin = entity->GetVecOrigin(), vecOriginLocal = local_player->GetVecOrigin();

	char dist_to[32]; int height = (pos.y - top.y), width = height / 2;

	sprintf_s(dist_to, "%.0f ft", resolve_distance(vecOriginLocal, vecOrigin));
	RENDER::DrawF(pos.x, SETTINGS::settings.ammo_bool ? pos.y + 12 : pos.y + 8, FONTS::visuals_esp_font, true, true, color, dist_to);
}


//void CVisuals::Watermark()
//{
	//int screen_width, screen_height;
	//INTERFACES::Engine->GetScreenSize(screen_width, screen_height);

//	RENDER::DrawFilledRect(global::ScreenWeight - 193, 4, global::ScreenWeight - 4, 23, CColor(0, 0, 0, 180));
//	RENDER::Textf(global::ScreenWeight - 187, 5, CColor(255, 255, 255, 255), FONTS::visuals_esp_font, XorStr("nnware | Fps: %03d | Ping: %03d"), UTILS::GetFps(), getping());
//}

std::string fix_item_name(std::string name)
{
	if (name[0] == 'C')
		name.erase(name.begin());

	auto startOfWeap = name.find("Weapon");
	if (startOfWeap != std::string::npos)
		name.erase(name.begin() + startOfWeap, name.begin() + startOfWeap + 6);

	return name;
}

void CVisuals::DrawWeapon2(SDK::CBaseEntity* entity, CColor color, int index, Vector pos, Vector top)
{
	SDK::player_info_t ent_info; INTERFACES::Engine->GetPlayerInfo(index, &ent_info);

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto weapon = INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex());
	if (!weapon) return;

	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));
	if (!c_baseweapon) return;

	bool is_teammate = local_player->GetTeam() == entity->GetTeam(), distanceThing, distanceThing2;
	if (SETTINGS::settings.weap_ammo) distanceThing = true; else distanceThing = false; if (SETTINGS::settings.weaponteam) distanceThing2 = true; else distanceThing2 = false;
	//int height = (pos.y - top.y), width = height / 2, distanceOn = distanceThing ? pos.y + 26 : pos.y + 18, distanceOn2 = distanceThing2 ? pos.y + 26 : pos.y + 18;
	int height = (pos.y - top.y), width = height / 2, distanceOn = distanceThing ? pos.y + 16 : pos.y + 12, distanceOn2 = distanceThing2 ? pos.y + 16 : pos.y + 12;

	RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, " (" + std::to_string(c_baseweapon->GetLoadedAmmo()) + ")");
}

void CVisuals::DrawWeapon(SDK::CBaseEntity* entity, CColor color, int index, Vector pos, Vector top)
{
	SDK::player_info_t ent_info; INTERFACES::Engine->GetPlayerInfo(index, &ent_info);

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto weapon = INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex());
	if (!weapon) return;

	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));
	if (!c_baseweapon) return;

	bool is_teammate = local_player->GetTeam() == entity->GetTeam(), distanceThing, distanceThing2;
	if (SETTINGS::settings.ammo_bool) distanceThing = true; else distanceThing = false; if (SETTINGS::settings.ammoteam) distanceThing2 = true; else distanceThing2 = false;
	//int height = (pos.y - top.y), width = height / 2, distanceOn = distanceThing ? pos.y + 14 : pos.y + 10, distanceOn2 = distanceThing2 ? pos.y + 14 : pos.y + 10;
	int height = (pos.y - top.y), width = height / 2, distanceOn = distanceThing ? pos.y + 28 : pos.y + 20, distanceOn2 = distanceThing2 ? pos.y + 28 : pos.y + 20;

	if (c_baseweapon->is_revolver())
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, "R8 REVOLVER");
	else if (c_baseweapon->GetItemDefenitionIndex() == SDK::definition_index::WEAPON_USP_SILENCER)
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, "USP-S");
	else if (c_baseweapon->GetItemDefenitionIndex() == SDK::definition_index::WEAPON_M4A1_SILENCER)
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, "M4A1-S");
	else
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, fix_item_name(weapon->GetClientClass()->m_pNetworkName));

}

void CVisuals::DrawHealth(SDK::CBaseEntity* entity, CColor color, CColor dormant, Vector pos, Vector top)
{
	int enemy_hp = entity->GetHealth(),
		hp_red = 255 - (enemy_hp * 2.55),
		hp_green = enemy_hp * 2.55,
		height = (pos.y - top.y),
		width = height / 2;

	float offset = (height / 4.f) + 5;
	UINT hp = height - (UINT)((height * enemy_hp) / 100);

	RENDER::DrawEmptyRect((pos.x - width / 2) - 6, top.y, (pos.x - width / 2) - 3, top.y + height, dormant);
	RENDER::DrawLine((pos.x - width / 2) - 4, top.y + hp, (pos.x - width / 2) - 4, top.y + height, color);
	RENDER::DrawLine((pos.x - width / 2) - 5, top.y + hp, (pos.x - width / 2) - 5, top.y + height, color);

	if (entity->GetHealth() < 100)
		RENDER::DrawF((pos.x - width / 2) - 4, top.y + hp, FONTS::visuals_esp_font, true, true, main_color, std::to_string(enemy_hp));
}

void CVisuals::BombPlanted(SDK::CBaseEntity* entity)
{
	BombCarrier = nullptr;

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	Vector vOrig; Vector vScreen;
	vOrig = entity->GetVecOrigin();
	SDK::CCSBomb* Bomb = (SDK::CCSBomb*)entity;

	float flBlow = Bomb->GetC4BlowTime();
	float TimeRemaining = flBlow;// -(INTERFACES::globals->interval_per_tick * local_player->GetTickBase());
	char buffer[64];
	sprintf_s(buffer, "B - %.1fs", TimeRemaining);
	RENDER::DrawF(10, 10, FONTS::visuals_lby_font, false, false, CColor(124, 195, 13, 255), buffer);
}

void CVisuals::DrawDropped(SDK::CBaseEntity* entity)
{
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);

	SDK::CBaseWeapon* weapon_cast = (SDK::CBaseWeapon*)entity;

	if (!weapon_cast)
		return;

	auto weapon = INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex());
	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));

	if (!c_baseweapon)
		return;

	if (!weapon)
		return;

	SDK::CBaseEntity* plr = INTERFACES::ClientEntityList->GetClientEntityFromHandle((HANDLE)weapon_cast->GetOwnerHandle());
	if (!plr && RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		std::string ItemName = fix_item_name(weapon->GetClientClass()->m_pNetworkName);
		int height = (pos.y - top.y);
		int width = height / 2;
		RENDER::DrawF(pos.x, pos.y, FONTS::visuals_esp_font, true, true, WHITE, ItemName.c_str()); //numpad_menu_font
	}
}

void CVisuals::DrawAmmo(SDK::CBaseEntity* entity, CColor color, CColor dormant, Vector pos, Vector top)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));
	if (!c_baseweapon) return;

	int height = (pos.y - top.y);

	float offset = (height / 4.f) + 5;
	UINT hp = height - (UINT)((height * 3) / 100);

	auto animLayer = entity->GetAnimOverlay(1);
	if (!animLayer.m_pOwner)
		return;

	auto activity = entity->GetSequenceActivity(animLayer.m_nSequence);

	int iClip = c_baseweapon->GetLoadedAmmo();
	int iClipMax = c_baseweapon->get_full_info()->max_clip;

	float box_w = (float)fabs(height / 2);
	float width;
	if (activity == 967 && animLayer.m_flWeight != 0.f)
	{
		float cycle = animLayer.m_flCycle;
		width = (((box_w * cycle) / 1.f));
	}
	else
		width = (((box_w * iClip) / iClipMax));

	RENDER::DrawFilledRect((pos.x - box_w / 2), top.y + height + 3, (pos.x - box_w / 2) + box_w + 2, top.y + height + 7, dormant); //outline
	RENDER::DrawFilledRect((pos.x - box_w / 2) + 1, top.y + height + 4, (pos.x - box_w / 2) + width + 1, top.y + height + 6, color); //ammo
}

void CVisuals::DrawInfo(SDK::CBaseEntity* entity, CColor color, CColor alt, Vector pos, Vector top)
{
	std::vector<std::pair<std::string, CColor>> stored_info;

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	bool is_local_player = entity == local_player;
	bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

	if (SETTINGS::settings.money_bool && !is_teammate)
		stored_info.push_back(std::pair<std::string, CColor>("$" + std::to_string(entity->GetMoney()), backtrack));
	else if (SETTINGS::settings.moneyteam && is_teammate)
		stored_info.push_back(std::pair<std::string, CColor>("$" + std::to_string(entity->GetMoney()), backtrack));

	if (SETTINGS::settings.info_bool && !is_teammate) //enemy
	{
		if (entity->GetArmor() > 0)
			stored_info.push_back(std::pair<std::string, CColor>(entity->GetArmorName(), color));

		if (entity->GetIsScoped())
			stored_info.push_back(std::pair<std::string, CColor>("Zoom", alt));

		//if (entity->SetFlashDuration() > 0)
		//stored_info.push_back(std::pair<std::string, CColor>("Flashed", alt));

		if (SETTINGS::settings.resolve_bool)
			if (using_fake_angles[entity->GetIndex()])
				stored_info.push_back(std::pair<std::string, CColor>("Fake", color));

		/*if (SETTINGS::settings.resolve_bool && local_player->GetHealth() > 0)
		{
			if (resolve_type[entity->GetIndex()] == 1)
				stored_info.push_back(std::pair<std::string, CColor>("lby", color)); //moving
			else if (resolve_type[entity->GetIndex()] == 2)
				stored_info.push_back(std::pair<std::string, CColor>("rand", color));
			else if (resolve_type[entity->GetIndex()] == 3)
				stored_info.push_back(std::pair<std::string, CColor>("update", color)); //lby updates
			else if (resolve_type[entity->GetIndex()] == 4)
				stored_info.push_back(std::pair<std::string, CColor>("brute", color)); //bruteforce
			else if (resolve_type[entity->GetIndex()] == 5)
				stored_info.push_back(std::pair<std::string, CColor>("log", color)); //logged angle
			else if (resolve_type[entity->GetIndex()] == 6)
				stored_info.push_back(std::pair<std::string, CColor>("inverse", color)); //inverse
			else if (resolve_type[entity->GetIndex()] == 7)
				stored_info.push_back(std::pair<std::string, CColor>("backtrack", color)); //backtracking lby
		}*/
	}
	else if (SETTINGS::settings.flagsteam && is_teammate) //teammate
	{
		if (entity->GetArmor() > 0)
			stored_info.push_back(std::pair<std::string, CColor>(entity->GetArmorName(), color));

		//if (entity->SetFlashDuration() > 0)
		//stored_info.push_back(std::pair<std::string, CColor>("Flashed", alt));

		if (entity->GetIsScoped())
			stored_info.push_back(std::pair<std::string, CColor>("Zoom", alt));
	}

	int height = (pos.y - top.y), width = height / 2, i = 0;
	for (auto Text : stored_info)
	{
		RENDER::DrawF((pos.x + width / 2) + 5, top.y + i, FONTS::visuals_esp_font, false, false, Text.second, Text.first);
		i += 8;
	}
}

void CVisuals::DrawInaccuracy()
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (!weapon) return;

	int W, H, cW, cH;
	INTERFACES::Engine->GetScreenSize(W, H);
	cW = W / 2; cH = H / 2;
	if (local_player->IsAlive())
	{
		auto accuracy = (weapon->GetInaccuracy() + weapon->GetSpreadCone()) * 500.f;
		if (!weapon->is_grenade() && !weapon->is_knife())
			RENDER::DrawFilledCircle(cW, cH, accuracy + 3, 30, CColor(0, 0, 0, 85));
	}
}

void CVisuals::DrawBulletBeams()
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (!INTERFACES::Engine->IsInGame() || !local_player) { Impacts.clear(); return; }
	if (Impacts.size() > 30) Impacts.pop_back();

	for (int i = 0; i < Impacts.size(); i++)
	{
		auto current = Impacts.at(i);
		if (!current.pPlayer) continue;
		if (current.pPlayer->GetIsDormant()) continue;

		bool is_local_player = current.pPlayer == local_player;
		bool is_teammate = local_player->GetTeam() == current.pPlayer->GetTeam() && !is_local_player;

		if (current.pPlayer == local_player)
			current.color = CColor(SETTINGS::settings.bulletlocal_col[0] * 255, SETTINGS::settings.bulletlocal_col[1] * 255, SETTINGS::settings.bulletlocal_col[2] * 255, SETTINGS::settings.bulletlocal_col[3] * 255);
		else if (current.pPlayer != local_player && !is_teammate)
			current.color = CColor(SETTINGS::settings.bulletenemy_col[0] * 255, SETTINGS::settings.bulletenemy_col[1] * 255, SETTINGS::settings.bulletenemy_col[2] * 255, SETTINGS::settings.bulletenemy_col[3] * 255);
		else if (current.pPlayer != local_player && is_teammate)
			current.color = CColor(SETTINGS::settings.bulletteam_col[0] * 255, SETTINGS::settings.bulletteam_col[1] * 255, SETTINGS::settings.bulletteam_col[2] * 255, SETTINGS::settings.bulletteam_col[3] * 255);

		SDK::BeamInfo_t beamInfo;
		beamInfo.m_nType = SDK::TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/laserbeam.vmt";
		beamInfo.m_nModelIndex = INTERFACES::ModelInfo->GetModelIndex("sprites/laserbeam.vmt");
		//beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = SETTINGS::settings.bulletlife;
		beamInfo.m_flWidth = SETTINGS::settings.bulletsize;
		beamInfo.m_flEndWidth = SETTINGS::settings.bulletsize;
		beamInfo.m_flFadeLength = 3.0f;
		beamInfo.m_flAmplitude = 0.f;
		beamInfo.m_flBrightness = 255;
		beamInfo.m_flSpeed = 1.f;
		beamInfo.m_nStartFrame = 1;
		beamInfo.m_flFrameRate = 60;
		beamInfo.m_flRed = current.color.RGBA[0];
		beamInfo.m_flGreen = current.color.RGBA[1];
		beamInfo.m_flBlue = current.color.RGBA[2];
		beamInfo.m_nSegments = 4;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = 0;

		beamInfo.m_vecStart = current.pPlayer->GetVecOrigin() + current.pPlayer->GetViewOffset();
		beamInfo.m_vecEnd = current.vecImpactPos;

		auto beam = INTERFACES::ViewRenderBeams->CreateBeamPoints(beamInfo);
		if (beam) INTERFACES::ViewRenderBeams->DrawBeam(beam);

		Impacts.erase(Impacts.begin() + i);
	}
}

void CVisuals::DrawCrosshair()
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto crosshair = INTERFACES::cvar->FindVar("crosshair");
	if (SETTINGS::settings.xhair_type == 0)
	{
		crosshair->SetValue("1");
		return;
	}
	else
		crosshair->SetValue("0");

	int W, H, cW, cH;
	INTERFACES::Engine->GetScreenSize(W, H);

	cW = W / 2; cH = H / 2;

	int dX = W / 120.f, dY = H / 120.f;
	int drX, drY;

	if (SETTINGS::settings.xhair_type == 2)
	{
		drX = cW - (int)(dX * (((local_player->GetPunchAngles().y * 2.f) * 0.45f) + local_player->GetPunchAngles().y));
		drY = cH + (int)(dY * (((local_player->GetPunchAngles().x * 2.f) * 0.45f) + local_player->GetPunchAngles().x));
	}
	else
	{
		drX = cW;
		drY = cH;
	}

	INTERFACES::Surface->DrawSetColor(BLACK);
	INTERFACES::Surface->DrawFilledRect(drX - 4, drY - 2, drX - 4 + 8, drY - 2 + 4);
	INTERFACES::Surface->DrawFilledRect(drX - 2, drY - 4, drX - 2 + 4, drY - 4 + 8);

	INTERFACES::Surface->DrawSetColor(WHITE);
	INTERFACES::Surface->DrawFilledRect(drX - 3, drY - 1, drX - 3 + 6, drY - 1 + 2);
	INTERFACES::Surface->DrawFilledRect(drX - 1, drY - 3, drX - 1 + 2, drY - 3 + 6);
}

void CVisuals::DrawFovArrows(SDK::CBaseEntity* entity, CColor color)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;
	if (entity->GetIsDormant()) return;

	Vector screenPos, client_viewangles;
	int screen_width = 0, screen_height = 0;
	float radius = 300;

	if (UTILS::IsOnScreen(aimbot->get_hitbox_pos(entity, SDK::HitboxList::HITBOX_HEAD), screenPos)) return;

	INTERFACES::Engine->GetViewAngles(client_viewangles);
	INTERFACES::Engine->GetScreenSize(screen_width, screen_height);

	const auto screen_center = Vector(screen_width / 2.f, screen_height / 2.f - SETTINGS::settings.randoslider, 0);
	const auto rot = DEG2RAD(client_viewangles.y - UTILS::CalcAngle(local_player->GetEyePosition(), aimbot->get_hitbox_pos(entity, SDK::HitboxList::HITBOX_HEAD)).y - 90);

	std::vector<SDK::Vertex_t> vertices;

	vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot) * radius, screen_center.y + sinf(rot) * SETTINGS::settings.arrowradius)));
	vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot + DEG2RAD(2)) * (radius - SETTINGS::settings.arrowsizesh), screen_center.y + sinf(rot + DEG2RAD(2)) * (SETTINGS::settings.arrowradius - SETTINGS::settings.arrowsizesh))));
	vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot - DEG2RAD(2)) * (radius - SETTINGS::settings.arrowsizesh), screen_center.y + sinf(rot - DEG2RAD(2)) * (SETTINGS::settings.arrowradius - SETTINGS::settings.arrowsizesh))));

	RENDER::TexturedPolygon(3, vertices, color);
}

void CVisuals::DrawIndicator()
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;
	if (local_player->GetHealth() <= 0) return;

	float breaking_lby_fraction = fabs(MATH::NormalizeYaw(global::real_angles.y - local_player->GetLowerBodyYaw())) / 180.f;
	float lby_delta = abs(MATH::NormalizeYaw(global::real_angles.y - local_player->GetLowerBodyYaw()));

	int screen_width, screen_height;
	INTERFACES::Engine->GetScreenSize(screen_width, screen_height);

	int iY = 88;
	//if (SETTINGS::settings.stop_bool)
	//{
	//	iY += 22;
	//	RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, SETTINGS::settings.stop_flip ? CColor(0, 255, 0) : CColor(255, 0, 0), "STOP");
	//}
	if (SETTINGS::settings.overrideenable)
	{
		iY += 22; bool overridekeyenabled;
		if (SETTINGS::settings.overridemethod == 0)
			RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, SETTINGS::settings.overridething ? CColor(0, 255, 0) : CColor(255, 0, 0), "OVERRIDE");
		else if (SETTINGS::settings.overridemethod == 1)
		{
			GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.overridekey)) ?
				RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, CColor(0, 255, 0), "OVERRIDE") :
				RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, CColor(255, 0, 0), "OVERRIDE");
		}
	}

	if (SETTINGS::settings.showlbydelta)
	{
		RENDER::DrawF(10, screen_height / 2 - 10, FONTS::menu_checkbox_font, false, false, CColor((1.f - breaking_lby_fraction) * 255.f, breaking_lby_fraction * 255.f, 0), "LBY Delta: " + std::to_string(lby_delta));
	}

	if (SETTINGS::settings.aa_bool && SETTINGS::settings.lbyenable)
	{
		iY += 22;
		RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, CColor((1.f - breaking_lby_fraction) * 255.f, breaking_lby_fraction * 255.f, 0), "LBY");

		DrawCircleLBY(CColor(255, 255, 255));
	}
	if (SETTINGS::settings.antiaim_arrow)
	{
		auto client_viewangles = Vector();
		INTERFACES::Engine->GetViewAngles(client_viewangles);
		const auto screen_center = Vector2D(screen_width / 2.f, screen_height / 2.f);
		float aa_arrow_width = SETTINGS::settings.aa_arrows_width;
		float aa_arrow_height = SETTINGS::settings.aa_arrows_height;

		constexpr auto radius = 100.f;
		auto draw_arrow = [&](float rot, CColor color) -> void
		{
			std::vector<SDK::Vertex_t> vertices;
			vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot) * radius, screen_center.y + sinf(rot) * radius)));
			vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot + DEG2RAD(aa_arrow_width)) * (radius - aa_arrow_height), screen_center.y + sinf(rot + DEG2RAD(aa_arrow_width)) * (radius - aa_arrow_height))));
			vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot - DEG2RAD(aa_arrow_width)) * (radius - aa_arrow_height), screen_center.y + sinf(rot - DEG2RAD(aa_arrow_width)) * (radius - aa_arrow_height))));
			RENDER::TexturedPolygon(3, vertices, color);
		};

		static auto alpha = 0.f; static auto plus_or_minus = false;
		if (alpha <= 0.f || alpha >= 255.f) plus_or_minus = !plus_or_minus;
		alpha += plus_or_minus ? (255.f / 7 * 0.015) : -(255.f / 7 * 0.015); alpha = clamp(alpha, 0.f, 255.f);

		//auto fake_color = CColor(255, 0, 0, alpha);
		//const auto fake_rot = DEG2RAD(client_viewangles.y - global::fake_angles.y - 90);
		//draw_arrow(fake_rot, fake_color);


		auto real_color = CColor(0, 255, 0, alpha);
		const auto real_rot = DEG2RAD(client_viewangles.y - global::real_angles.y - 90);
		draw_arrow(real_rot, real_color);
	}
	if (SETTINGS::settings.indicator_bool2)
	{
		iY += 36;

		if (flipaa)
		{
			RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, CColor(75, 0, 130, 255), "RIGHT");
		}
		else
		{
			RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, CColor(75, 0, 130, 255), "LEFT");
		}
	}
	if (SETTINGS::settings.indicator_bool)
	{
		//static float rainbow;
		//rainbow += 0.003f;
		//if (rainbow > 1.f) rainbow = 0.f;

		if (SETTINGS::settings.flip_int != -1 && UTILS::INPUT::input_handler.GetKeyState(SETTINGS::settings.flip_int) & 1)
			flipaa = !flipaa;

		iY += 22;

		if (flipaa)
		{
			RENDER::DrawF((screen_width / 2) + 40, screen_height / 2, FONTS::menu_window_font2, true, true, CColor(SETTINGS::settings.aa_ccolor[0] * 255, SETTINGS::settings.aa_ccolor[1] * 255, SETTINGS::settings.aa_ccolor[2] * 255, SETTINGS::settings.aa_ccolor[3] * 255), ">"); //real(right)
			RENDER::DrawF((screen_width / 2) - 40, screen_height / 2, FONTS::menu_window_font2, true, true, CColor(255, 255, 255), "<");

		}
		else
		{
			RENDER::DrawF((screen_width / 2) - 40, screen_height / 2, FONTS::menu_window_font2, true, true, CColor(SETTINGS::settings.aa_ccolor[0] * 255, SETTINGS::settings.aa_ccolor[1] * 255, SETTINGS::settings.aa_ccolor[2] * 255, SETTINGS::settings.aa_ccolor[3] * 255), "<"); //real(left)
			RENDER::DrawF((screen_width / 2) + 40, screen_height / 2, FONTS::menu_window_font2, true, true, CColor(255, 255, 255), ">");
		}
	}
}

void CVisuals::ModulateWorld()
{
	float prop_alpha = 1.f;

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	static auto sv_skyname = INTERFACES::cvar->FindVar("sv_skyname");
	sv_skyname->nFlags &= ~FCVAR_CHEAT;

	static auto static_val = 100;
	auto night_val = SETTINGS::settings.daytimevalue;

	if (SETTINGS::settings.asus_props_value != prop_alpha || disconnect)
	{
		for (SDK::MaterialHandle_t i = INTERFACES::MaterialSystem->FirstMaterial(); i != INTERFACES::MaterialSystem->InvalidMaterial(); i = INTERFACES::MaterialSystem->NextMaterial(i))
		{
			SDK::IMaterial *pMaterial = INTERFACES::MaterialSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "StaticProp textures"))
			{
				pMaterial->AlphaModulate(prop_alpha);
			}
		}
		prop_alpha = (1 - (SETTINGS::settings.asus_props_value));
	}

	if (SETTINGS::settings.asus_props_value != prop_alpha || disconnect)
	{
		for (SDK::MaterialHandle_t i = INTERFACES::MaterialSystem->FirstMaterial(); i != INTERFACES::MaterialSystem->InvalidMaterial(); i = INTERFACES::MaterialSystem->NextMaterial(i))
		{
			SDK::IMaterial *pMaterial = INTERFACES::MaterialSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "StaticProp textures"))
			{
				pMaterial->AlphaModulate(prop_alpha);
			}
		}
		prop_alpha = (1 - (SETTINGS::settings.asus_props_value / 100));
	}

	if (static_val != night_val)
	{
		static_val = night_val;
		for (auto i = INTERFACES::MaterialSystem->FirstMaterial(); i != INTERFACES::MaterialSystem->InvalidMaterial(); i = INTERFACES::MaterialSystem->NextMaterial(i))
		{
			auto pMaterial = INTERFACES::MaterialSystem->GetMaterial(i);
			if (!pMaterial) continue;

			auto finalnightval = night_val / 100.f;
			if (strstr(pMaterial->GetTextureGroupName(), "World"))
				pMaterial->ColorModulate(finalnightval, finalnightval, finalnightval);
			else if (strstr(pMaterial->GetTextureGroupName(), "SkyBox"))
				night_val == 100.f ? sv_skyname->SetValue("vertigoblue_hdr") : sv_skyname->SetValue("sky_csgo_night02");

		}
	}
}

void CVisuals::DrawCircleLBY(CColor color)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	int screen_width, screen_height;
	INTERFACES::Engine->GetScreenSize(screen_width, screen_height);

	static float next_lby_update[65];
	//static float last_lby[65];

	const float curtime = UTILS::GetCurtime();

	if (local_player->GetVelocity().Length2D() > 0.1 && !global::is_fakewalking)
		return;

	auto animstate = local_player->GetAnimState();
	if (!animstate)
		return;
	static float last_lby[65];
	if (local_player->GetHealth() > 0)
	{
		if (last_lby[local_player->GetIndex()] != local_player->GetLowerBodyYaw())
		{
			last_lby[local_player->GetIndex()] = local_player->GetLowerBodyYaw();
			next_lby_update[local_player->GetIndex()] = curtime + 1.1 + INTERFACES::Globals->interval_per_tick;
		}

		if (next_lby_update[local_player->GetIndex()] < curtime)
		{
			next_lby_update[local_player->GetIndex()] = curtime + 1.1;
		}

		float time_remain_to_update = next_lby_update[local_player->GetIndex()] - local_player->GetSimTime();
		float time_update = next_lby_update[local_player->GetIndex()];

		float fill;
		fill = (((time_remain_to_update)));
		static float add = 0.000f;
		add = 1.1 - fill;

		int iY = 88;

		/*
		iY += 22;
		RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, CColor((1.f - breaking_lby_fraction) * 255.f, breaking_lby_fraction * 255.f, 0), "LBY");
		*/

		iY += 20;

		RENDER::DrawFilledCircle(96, screen_height - iY, 12, 60, CColor(30, 30, 30));
		DrawLBYCircleTimer(96, screen_height - iY, 10, add, CColor(0, 190, 0, 255));
		RENDER::DrawFilledCircle(96, screen_height - iY, 7, 60, CColor(40, 40, 40));
	}
}

void CVisuals::viewmodelxyz()
{
	if (SETTINGS::settings.Viewmodelchanger)
	{
		INTERFACES::cvar->get_convar("viewmodel_offset_x")->set_value(SETTINGS::settings.viewmodel_x);
		INTERFACES::cvar->get_convar("viewmodel_offset_y")->set_value(SETTINGS::settings.viewmodel_y);
		INTERFACES::cvar->get_convar("viewmodel_offset_z")->set_value(SETTINGS::settings.viewmodel_z - 10);
	}
}

void CVisuals::ModulateSky()
{
	/*static bool nightmode_performed = false, nightmode_lastsetting;

	if (!INTERFACES::Engine->IsConnected() || !INTERFACES::Engine->IsInGame())
	{
		if (nightmode_performed)
			nightmode_performed = false;
		return;
	}

	if (nightmode_performed != SETTINGS::settings.night_bool)
	{
		nightmode_lastsetting = SETTINGS::settings.night_bool;
		nightmode_performed = false;
	}

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (!local_player)
	{
		if (nightmode_performed) nightmode_performed = false;
		return;
	}

	if (nightmode_lastsetting != SETTINGS::settings.night_bool)
	{
		nightmode_lastsetting = SETTINGS::settings.night_bool;
		nightmode_performed = false;
	}

	if (!nightmode_performed)
	{

		if (SETTINGS::settings.night_bool) sv_skyname->SetValue("sky_csgo_night02");
		else sv_skyname->SetValue("vertigoblue_hdr");
	}*/
}

void CVisuals::DrawHitmarker()
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (local_player->GetHealth() <= 0)
		return;

	static int lineSize = 6;

	static float alpha = 0;
	float step = 255.f / 0.3f * INTERFACES::Globals->frametime;


	if (global::flHurtTime + 0.4f >= INTERFACES::Globals->curtime)
		alpha = 255.f;
	else
		alpha -= step;

	if (alpha > 0) {
		int screenSizeX, screenCenterX;
		int screenSizeY, screenCenterY;
		INTERFACES::Engine->GetScreenSize(screenSizeX, screenSizeY);

		screenCenterX = screenSizeX / 2;
		screenCenterY = screenSizeY / 2;
		CColor col = CColor(255, 255, 255, alpha);
		RENDER::DrawLine(screenCenterX - lineSize * 2, screenCenterY - lineSize * 2, screenCenterX - (lineSize), screenCenterY - (lineSize), col);
		RENDER::DrawLine(screenCenterX - lineSize * 2, screenCenterY + lineSize * 2, screenCenterX - (lineSize), screenCenterY + (lineSize), col);
		RENDER::DrawLine(screenCenterX + lineSize * 2, screenCenterY + lineSize * 2, screenCenterX + (lineSize), screenCenterY + (lineSize), col);
		RENDER::DrawLine(screenCenterX + lineSize * 2, screenCenterY - lineSize * 2, screenCenterX + (lineSize), screenCenterY - (lineSize), col);
	}
}

void CVisuals::DrawBorderLines()
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto weapon = INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex());
	if (!weapon) return;

	int screen_x;
	int screen_y;
	int center_x;
	int center_y;
	INTERFACES::Engine->GetScreenSize(screen_x, screen_y);
	INTERFACES::Engine->GetScreenSize(center_x, center_y);
	center_x /= 2; center_y /= 2;

	if (local_player->GetIsScoped())
	{
		if (SETTINGS::settings.scope_bool == 1)
		{
			RENDER::DrawLine(0, center_y, screen_x, center_y, CColor(0, 0, 0, 255));
			RENDER::DrawLine(center_x, 0, center_x, screen_y, CColor(0, 0, 0, 255));
		}
	}
}
bool IsInGame()
{
	return INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame();
}

int getping()
{
	if (!IsInGame())
		return 0;

	auto nci = INTERFACES::Engine->GetNetChannelInfo();
	if (!nci)
		return 0;

	float m_AvgLatency = nci->GetAvgLatency(0);

	return (int)(m_AvgLatency * 1000.0f);
}

int getfps()
{
	return static_cast<int>(1.f / INTERFACES::Globals->frametime);
}
void CVisuals::watermark()
{

	static float rainbow;
	rainbow += 0.0005;
	if (rainbow > 1.f) rainbow = 0.f;

	int screen_width, screen_height;
	INTERFACES::Engine->GetScreenSize(screen_width, screen_height);

	RENDER::DrawFilledRect(screen_width - 200, 4, screen_width - 4, 23, CColor(0, 0, 0, 255));
	RENDER::DrawFilledRect(screen_width - 203, 4, screen_width - 4, 23, CColor::FromHSB(rainbow, 1.f, 1.f));
	//RENDER::Textf(screen_width - 187, 5, CColor(0, 0, 0, 255), FONTS::menu_window_font4, XorStr("nnware | Fps: %03d | Ping: %03d"), getfps(), getping());
	RENDER::Textf(screen_width - 195, 4, CColor(255, 255, 255, 255), FONTS::menu_window_font3, XorStr("nnware | Fps: %03d | Ping: %03d"), getfps(), getping());
}
void setClanTag(const char* tag, const char* name)//190% paste
{
	static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(((DWORD)UTILS::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx")));
	pSetClanTag(tag, name);
}
int kek = 0;
int autism = 0;
const char* clantagAnim[31] = {
	"              ", "             n", "            nn",
	"           nnw", "          nnwa", "         nnwar",
	"        nnware", "       nnware.", "      nnware.g",
	"     nnware.gq", "    nnware.gq ", "   nnware.gq B",
	"  nnware.gq BE", " nnware.gq BET", "nnware.gq BETA",
	"nware.gq BETA ", "ware.gq BETA  ", "are.gq BETA   ",
	"re.gq BETA    ", "re.gq BETA    ", "e.gq BETA     ",
	".gq BETA      ", "gq BETA       ", "gq BETA       ",
	"q BETA        ", " BETA         ", "BETA          ",
	"ETA           ", "TA            ", "A             ",
	"              "
};
void CVisuals::Clantag()
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	static size_t lastTime = 0;
	if (GetTickCount() > lastTime)
	{
		kek++;
		if (kek > 10) {
			autism++; if (autism > 30) autism = 0;
			setClanTag(clantagAnim[autism], "nnware.gq BETA");
			lastTime = GetTickCount() + 350;
		}

		if (kek > 11) kek = 0;
	}
}

CVisuals* visuals = new CVisuals();