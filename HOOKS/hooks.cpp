#include "..\includes.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "../ImGui/dx9/imgui_impl_dx9.h"
#include "hooks.h"
#include "../SDK/imdlcache.h"
#include "../UTILS/interfaces.h"
#include "../UTILS/offsets.h"
#include "../UTILS/NetvarHookManager.h"
#include "../UTILS/render.h"
#include "../FEATURES/NewEventLog.h"
#include "../SDK/CInput.h"
#include "../SDK/IClient.h"
#include "../SDK/CPanel.h"
#include "../SDK/ConVar.h"
#include "../SDK/CGlowObjectManager.h"
#include "../SDK/IEngine.h"
#include "../SDK/CTrace.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/RecvData.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/ModelRender.h"
#include "../SDK/RenderView.h"
#include "../SDK/CTrace.h"
#include "../SDK/CViewSetup.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CPrediction.h"
#include "../FEATURES/Movement.h"
#include "../FEATURES/Visuals.h"
#include "../FEATURES/Chams.h"
#include "../FEATURES/AntiAim.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Backtracking.h"
#include "../FEATURES/FakeWalk.h"
#include "../cheats/visuals/player_esp.h"
#include "../cheats/visuals/other_esp.h"
#include "../cheats/exploits/exploits.h"
#include "../FEATURES/FakeLag.h"
#include "../FEATURES/EnginePred.h"
#include "../FEATURES/EventListener.h"
#include "../FEATURES/GrenadePrediction.h"
#include "../SDK/NetChannel.h"
#include "../FEATURES/Legitbot.h"
#include "../FEATURES/Flashlight.h"
#include "../FEATURES/GloveChanger.h"
#include "../FEATURES/SkinChanger.h"
#include "../FEATURES/D9Visuals.h"
#include "..\night_mode.h"
#include "..\FEATURES\custom_font.h"
#include "../shit.h"
#include <intrin.h>
#include "convar_mkurtl.h"
#include "../SDK/imdlcache.h"
#include "../radionstuff.h"
#include "../bass.h"
#include "../XorStr.hpp"


//#pragma comment(lib, "bass.lib")	
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

CMenu* g_Menu = new CMenu();

typedef MDLHandle_t(__thiscall* iFindMdl)(void*, char*);
iFindMdl oFindMDL;
MDLHandle_t __fastcall hkFindMDL(void*, void*, char*);

namespace INIT
{
	HMODULE Dll;
	HWND Window;
	WNDPROC OldWindow;
}

namespace MenuTabs
{
	void Tab1();
	void Tab2();
	void Tab3();
	void Tab4();
	void Tab5();
}

enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),

	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	//	MATERIAL_VAR_UNUSED					  = (1 << 9),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
	//	MATERIAL_VAR_UNUSED					  = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31),

	// NOTE: Only add flags here that either should be read from
	// .vmts or can be set directly from client code. Other, internal
	// flags should to into the flag enum in IMaterialInternal.h
};

template<class T>
static T* FindHudElement(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**>(UTILS::FindSignature("client_panorama.dll", "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(UTILS::FindSignature("client_panorama.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	return (T*)find_hud_element(pThis, name);
}

static bool menu_open = false;
static bool tabsOnTop = false;
static bool cvarsoff = true;
static bool d3d_init = false;
bool PressedKeys[256] = {};
const char* merixids[] =
{
	"1","2","3","4","5","6", "7", "8", "9",
	"Q","W","E","R","T","Y","U","I","O","P",
	"A","S","D","F","G","H","J","K","L",
	"Z","X","C","V","B","N","M",".","\\","|", "/","}","{","[","]",
	"<",">","?","'"
};
static char ConfigNamexd[64] = { 0 };
static char clantag_name[64] = { 0 };
static bool should_reset = false;
namespace ImGui
{

	static auto vector_getterxd = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	IMGUI_API bool ListBoxConfigArray(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		return ListBox(label, currIndex, vector_getterxd,
			static_cast<void*>(&values), values.size(), 10);
	}

	IMGUI_API bool ComboBoxArrayxd(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getterxd,
			static_cast<void*>(&values), values.size());
	}
}
/*
	 Dingma
   Dingma what
 Dingma car door
			-calvin
*/
HSTREAM stream;
ImFont* bigmenu_font;
ImFont* menu_font;
ImFont* smallmenu_font;
ImFont* coolfont;
ImFont* coolfont2;
ImFont* font_menu;
ImFont* font_menu2;
//--- Other Globally Used Variables ---///
static bool tick = false;
static int ground_tick;
Vector vecAimPunch, vecViewPunch;
Vector* pAimPunch = nullptr;
Vector* pViewPunch = nullptr;

//--- Declare Signatures and Patterns Here ---///
static auto CAM_THINK = UTILS::FindSignature("client_panorama.dll", "85 C0 75 30 38 86");
static auto linegoesthrusmoke = UTILS::FindPattern("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0", "xxxxxxxx????xxx");

//--- Tick Counting ---//
void ground_ticks()
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (local_player->GetFlags() & FL_ONGROUND)
		ground_tick++;
	else
		ground_tick = 0;
}

namespace HOOKS
{
	CreateMoveFn original_create_move;
	PaintTraverseFn original_paint_traverse;
	PaintFn original_paint;
	FrameStageNotifyFn original_frame_stage_notify;
	DrawModelExecuteFn original_draw_model_execute;
	SceneEndFn original_scene_end;
	TraceRayFn original_trace_ray;
	OverrideViewFn original_override_view;
	RenderViewFn original_render_view;
	SvCheatsGetBoolFn original_get_bool;
	GetViewmodelFOVFn original_viewmodel_fov;

	vfunc_hook fireevent;
	vfunc_hook directz;
	vfunc_hook HookMethod;

	VMT::VMTHookManager iclient_hook_manager;
	VMT::VMTHookManager panel_hook_manager;
	VMT::VMTHookManager paint_hook_manager;
	VMT::VMTHookManager model_render_hook_manager;
	VMT::VMTHookManager scene_end_hook_manager;
	VMT::VMTHookManager render_view_hook_manager;
	VMT::VMTHookManager trace_hook_manager;
	VMT::VMTHookManager net_channel_hook_manager;
	VMT::VMTHookManager override_view_hook_manager;
	VMT::VMTHookManager input_table_manager;
	VMT::VMTHookManager get_bool_manager;

	std::string sPanel = ("FocusOverlayPanel");

	template<class T, class U>
	T fine(T in, U low, U high)
	{
		if (in <= low)
			return low;

		if (in >= high)
			return high;

		return in;
	}

	bool __stdcall HookedCreateMove(float sample_input_frametime, SDK::CUserCmd* cmd)
	{
		if (!cmd || cmd->command_number == 0)
			return false;

		uintptr_t* FPointer; __asm { MOV FPointer, EBP }
		byte* SendPacket = (byte*)(*FPointer - 0x1C);
		if (!SendPacket) return false;

		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player) return false;

		global::should_send_packet = *SendPacket;
		global::originalCMD = *cmd;
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			//GrenadePrediction::instance().Tick(cmd->buttons);
			if (SETTINGS::settings.aim_type == 0)
				slowwalk->SlowWalk(cmd);

			if (SETTINGS::settings.nocoruchdelay)
			exploits->duck(cmd);

			if (!INTERFACES::Engine->IsVoiceRecording())
				fakelag->do_fakelag(cmd);

			static SDK::ConVar* impacts = INTERFACES::cvar->FindVar("sv_showimpacts");
			if (SETTINGS::settings.impacts)
			{

				impacts->SetValue(1);

			}
			else {

				impacts->SetValue(0);

			}

			if (SETTINGS::settings.autozeus_bool)
				aimbot->autozeus(cmd);

			if (SETTINGS::settings.autoknife_bool)
				aimbot->autoknife(cmd);

			if (SETTINGS::settings.bhop_bool) movement->bunnyhop(cmd);
			if (SETTINGS::settings.duck_bool) movement->duckinair(cmd);
			if (SETTINGS::settings.strafe_bool) movement->autostrafer(cmd);
			if (SETTINGS::settings.backtrack_chams) backtracking->run_legit(cmd);
			if (SETTINGS::settings.misc_clantag)
			{
				int iLastTime;
				if (int(INTERFACES::Globals->curtime) != iLastTime)
				{
					global::hasclantagged = true;
					static std::string cur_clantag = /*clantag_name*/ " nnware.gq ";
					//if (should_reset)
					//    cur_clantag = clantag_name;
					//todo: fix custom clantag staying static after force updating
					static int old_time,
						i = 0;

					if (i > 32)
					{
						auto marquee = [](std::string& panicova_zlomena_noha) -> void
						{
							std::string temp_string = panicova_zlomena_noha;
							panicova_zlomena_noha.erase(0, 1);
							panicova_zlomena_noha += temp_string[0];
						};
						marquee(cur_clantag);
						auto setclantag = [](const char* tag) -> void
						{
							static auto ClanTagOffset = UTILS::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", "xxxxxxxxx");
							if (ClanTagOffset)
							{
								auto tag_ = std::string(tag);
								if (strlen(tag) > 0) {
									auto newline = tag_.find("\\n");
									auto tab = tag_.find("\\t");
									if (newline != std::string::npos) {
										tag_.replace(newline, newline + 2, "\n");
									}
									if (tab != std::string::npos) {
										tag_.replace(tab, tab + 2, "\t");
									}
								}
								static auto dankesttSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(ClanTagOffset);
								dankesttSetClanTag(tag_.data(), tag_.data());
							}
						};
						setclantag(cur_clantag.c_str());
						i = 0;
					}
					else
					{
						i++;
					}
				}

				iLastTime = int(INTERFACES::Globals->curtime);
			}
			else if (global::hasclantagged || should_reset && global::hasclantagged) {
				auto cl_clanid = INTERFACES::cvar->FindVar("cl_clanid");
				cl_clanid->SetValue(-1);
				global::hasclantagged = false;
				should_reset = false;
			}
			prediction->run_prediction(cmd); {

				if (SETTINGS::settings.aim_bool)
				{
					if (SETTINGS::settings.fake_bool)
					{
						if (local_player->IsAlive())
						{
							for (int i = 1; i < INTERFACES::Globals->maxclients; i++)
							{
								auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
								if (!entity || entity == local_player || entity->GetClientClass()->m_ClassID != !entity->IsAlive()) continue;



								float simtime_delta = entity->GetSimTime() - *reinterpret_cast<float*>(uintptr_t(entity) + OFFSETS::m_flSimulationTime) + 0x4;

								int choked_ticks = HOOKS::fine(TIME_TO_TICKS(simtime_delta), 1, 15);
								Vector lastOrig;

								if (lastOrig.Length() != entity->GetVecOrigin().Length())
									lastOrig = entity->GetVecOrigin();

								float delta_distance = (entity->GetVecOrigin() - lastOrig).LengthSqr();
								if (delta_distance > 4096.f)
								{
									Vector velocity_per_tick = entity->GetVelocity() * INTERFACES::Globals->interval_per_tick;
									auto new_origin = entity->GetVecOrigin() + (velocity_per_tick * choked_ticks);
									entity->SetAbsOrigin(new_origin);
								}
							}
						}
					}
					aimbot->run_aimbot(cmd);
					backtracking->backtrack_player(cmd);
				}

				if (SETTINGS::settings.aim_type == 1 && SETTINGS::settings.aim_bool)
				{
					if (GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.legittrigger_key)) && SETTINGS::settings.legittrigger_bool)
						legitbot->triggerbot(cmd);
					backtracking->run_legit(cmd);
				}

				if (SETTINGS::settings.aa_bool)
				{
					antiaim->do_antiaim(cmd);
					antiaim->fix_movement(cmd);

					auto net_channel = INTERFACES::Engine->GetNetChannel();
					if (!net_channel)
						return;

					if (!global::should_send_packet && SETTINGS::settings.desync_yaw)
						cmd->viewangles.y += net_channel->m_nChokedPackets == 0 ? -58 : 180;
					//ground_ticks();
				}
			} prediction->end_prediction(cmd);

			if (!global::should_send_packet) {
				global::real_angles = cmd->viewangles;
				global::angles = cmd->viewangles;
			}
			else
			{
				global::FakePosition = local_player->GetAbsOrigin();
				global::fake_angles = cmd->viewangles;
			}

		}
		*SendPacket = global::should_send_packet;
		cmd->move = antiaim->fix_movement(cmd, global::originalCMD);
		if (SETTINGS::settings.aa_pitch < 2 || SETTINGS::settings.aa_pitch1_type < 2 || SETTINGS::settings.aa_pitch2_type < 2 || SETTINGS::settings.aa_pitch3_type < 2)
			UTILS::ClampLemon(cmd->viewangles);

		return false;
	}
	void render_scope() {

		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player) return;

		int
			screen_x,
			screen_y,
			center_x,
			center_y;

		INTERFACES::Engine->GetScreenSize(screen_x, screen_y); INTERFACES::Engine->GetScreenSize(center_x, center_y);
		center_x /= 2; center_y /= 2;

		static float rainbow;
		rainbow += 0.00001;

		if (rainbow > 1.f)
			rainbow = 0.f;

		if (local_player->GetIsScoped()) {

			CColor color;

			if (SETTINGS::settings.scope_type == 0)
				color = CColor::FromHSB(rainbow, 1.f, 1.f);
			else
				color = CColor(0, 0, 0);

			RENDER::DrawLine(0, center_y, screen_x, center_y, color);
			RENDER::DrawLine(center_x, 0, center_x, screen_y, color);
		}
	}

	void __stdcall HookedPaintTraverse(int VGUIPanel, bool ForceRepaint, bool AllowForce)
	{
		if (SETTINGS::settings.scope_bool) render_scope();
		std::string panel_name = INTERFACES::Panel->GetName(VGUIPanel);
		if (panel_name == "HudZoom" && SETTINGS::settings.scope_bool) return;
		//player_visuals->paint_traverse();
		if (panel_name == "MatSystemTopPanel")
		{
			/*int screen_width, screen_height;
			INTERFACES::Engine->GetScreenSize(screen_width, screen_height);

			auto DrawInject = [screen_width, screen_height]()
			{
				static constexpr float intro_length_time = 3.f, intro_max_height_offset = -25.f, intro_max_width_offset = -15.f;

				const float current_time = GetTickCount() * 0.001f, time_delta = current_time - GLOBAL::cheat_start_time;
				if (time_delta < intro_length_time)
				{
					static constexpr char thulium_text[] = "I N J E C T E D";

					const int text_width = RENDER::GetTextSize(FONTS::welcome_font, thulium_text).x, text_len = strlen(thulium_text);
					const int center_width = screen_width * 0.5, center_height = screen_height * 0.5;
					const int current_letter = ((time_delta / intro_length_time) * text_len) + 1;
					const float time_per_letter = intro_length_time / static_cast<float>(text_len);
					for (int i = 0; i < text_len; i++)
					{
						if (i >= current_letter)
							continue;

						const float frac = ((fmod(time_delta, time_per_letter) * 0.5f + (i == current_letter - 2 ? time_per_letter * 0.5f : 0)) / time_per_letter);

						int alpha = 0;
						if (current_letter > i + 2)
							alpha = 255;
						else
							alpha = frac * 255.f;

						const int cur_width = center_width + (text_width * 0.5f) - (text_width / static_cast<float>(text_len)) * static_cast<float>(text_len - i);
						if (i == current_letter - 1 || i == current_letter - 2)
						{
							const float frac_2 = (frac > 0.5f ? 1.f - frac : frac) * 2.f;
							RENDER::DrawF(cur_width + (frac_2 * intro_max_width_offset), center_height + (frac_2 * intro_max_height_offset), FONTS::welcome_font, false, true, CColor::FromHSB(rainbow, 1.f, 1.f), std::string(1, thulium_text[i]));
						}
						else
							RENDER::DrawF(cur_width, center_height, FONTS::welcome_font, false, true, CColor::FromHSB(rainbow, 1.f, 1.f), std::string(1, thulium_text[i]));
					}
				}
			};*/

			//DrawInject(); // draw intro \/

			//other shit for wrong hake menu
			
			//all we need really (may as well inlude the other shit for pasters XD)
			visuals->watermark();// draw watermark


		}
		cother_esp other_esp;
		if (SETTINGS::settings.playerhitmarker)
		visuals->hitmarkerdynamic_paint();
		//other_esp.render_tracer();

		//player_visuals->paint_traverse();

		auto local_player = static_cast<SDK::CBaseEntity*>(INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer()));

		if (!local_player)
			return;

		if (SETTINGS::settings.killfeed) {
			if (local_player->IsAlive()) {

				if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame()) {

					static DWORD* _death_notice = FindHudElement<DWORD>("CCSGO_HudDeathNotice");
					static void(__thiscall *_clear_notices)(DWORD) = (void(__thiscall*)(DWORD))UTILS::FindSignature("client_panorama.dll", "55 8B EC 83 EC 0C 53 56 8B 71 58");

					if (global::m_round_changed) {

						_death_notice = FindHudElement<DWORD>("CCSGO_HudDeathNotice");
						if (_death_notice - 10)
							_clear_notices(((DWORD)_death_notice - 10));

						global::m_round_changed = false;
					}

					if (_death_notice)
						*(float*)((DWORD)_death_notice + 0x50) = SETTINGS::settings.killfeed ? 100 : 90;
				}
			}
		}
		else
		{

		}

		SDK::ConVar* mat_fullbright = INTERFACES::cvar->FindVar("mat_fullbright");

		if (SETTINGS::settings.full_bright)
			mat_fullbright->SetValue(true);
		else
			mat_fullbright->SetValue(false);

		if (panel_name == "FocusOverlayPanel")
		{
			if (FONTS::ShouldReloadFonts())
				FONTS::InitFonts();

			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			{

				GrenadePrediction::instance().Paint();

				//FEATURES::MISC::in_game_logger.Do();
				auto matpostprocess = INTERFACES::cvar->FindVar("mat_postprocess_enable");
				matpostprocess->fnChangeCallback = 0;
				matpostprocess->SetValue(SETTINGS::settings.matpostprocessenable);

				//cnight_mode night;
				//night.do_nightmode();

				visuals->ModulateWorld();

				visuals->Drawmodels();
				visuals->Drawmodelsweapons();

				if (SETTINGS::settings.Enabled)
				{
					visuals->Draw();
					visuals->ClientDraw();
				}
			}
		}


		//for (int i = 0; i < 4; i++)
		//{
			//for (int j = 0; j < 4; j++)
			//{
			//	viewMatrix[i][j] = INTERFACES::Engine->WorldToScreenMatrix()[i][j];
		//	}
	//	}
		original_paint_traverse(INTERFACES::Panel, VGUIPanel, ForceRepaint, AllowForce);

		const char* pszPanelName = INTERFACES::Panel->GetName(VGUIPanel);

		if (!strstr(pszPanelName, sPanel.data()))
			return;

		INTERFACES::Panel->SetMouseInputEnabled(VGUIPanel, menu_open);
	}

	void FixThemAnim(SDK::CBaseEntity* entity)
	{
		if (!entity)
			return;

		auto old_curtime = INTERFACES::Globals->curtime;
		auto old_frametime = INTERFACES::Globals->frametime;
		auto old_fraction = entity->GetAnimState()->m_flUnknownFraction = 0.f;

		INTERFACES::Globals->curtime = entity->GetSimTime();
		INTERFACES::Globals->frametime = INTERFACES::Globals->interval_per_tick;

		SDK::CAnimationLayer backup_layers[15];
		std::memcpy(backup_layers, entity->GetAnimOverlays(), (sizeof(SDK::CAnimationLayer) * 15));

		if (entity->GetAnimState())
			entity->GetAnimState()->m_iLastClientSideAnimationUpdateFramecount = INTERFACES::Globals->framecount - 1;

		entity->GetClientSideAnimation2() = true;
		entity->UpdateClientSideAnimation();
		entity->GetClientSideAnimation2() = false;

		std::memcpy(entity->GetAnimOverlays(), backup_layers, (sizeof(SDK::CAnimationLayer) * 15));

		INTERFACES::Globals->curtime = old_curtime;
		INTERFACES::Globals->frametime = old_frametime;

		/*if (global::is_fakewalking) {
			entity->GetAnimState()->m_vVelocityX = old_velocity;
			entity->GetAnimState()->m_vVelocityY = old_velocity2;
			entity->GetAnimState()->m_flFeetSpeedUnknownForwardOrSideways = old_velocity3;
			entity->GetAnimState()->m_flFeetSpeedForwardsOrSideWays = old_velocity4;
			entity->GetAnimState()->m_flUnknownFloat3 = old_velocity5;
			entity->GetAnimState()->m_flUnknownFloat2 = old_velocity6;
			entity->GetAnimState()->m_flUnknownFloat1 = old_velocity7;
		}*/

		entity->GetAnimState()->m_flUnknownFraction = old_fraction; //fix them legs

		entity->SetAngle2(Vector(0.f, entity->GetAnimState()->m_flGoalFeetYaw, 0.f));
	}

	void __fastcall HookedFrameStageNotify(void* ecx, void* edx, int stage)
	{

		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player) return;
		Vector vecAimPunch, vecViewPunch;
		Vector* pAimPunch = nullptr; Vector* pViewPunch = nullptr;

		auto GetCorrectDistance = [&local_player](float ideal_distance) -> float {

			Vector inverse_angles;
			INTERFACES::Engine->GetViewAngles(inverse_angles);

			inverse_angles.x *= -1.f, inverse_angles.y += 180.f;

			Vector direction;
			MATH::AngleVectors(inverse_angles, &direction);

			SDK::CTraceWorldOnly filter;
			SDK::trace_t trace;
			SDK::Ray_t ray;

			ray.Init(local_player->GetVecOrigin() + local_player->GetViewOffset(), (local_player->GetVecOrigin() + local_player->GetViewOffset()) + (direction * (ideal_distance + 5.f)));
			INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

			return ideal_distance * trace.flFraction;
		};

		switch (stage)
		{
		case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			{
				GloveChanger();
				xdSkinchanger();

				for (int i = 1; i <= 65; i++)
				{
					auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
					if (!entity) continue;

					bool is_local_player = entity == local_player;
					bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

					if (is_local_player) continue;
					if (is_teammate) continue;
					if (entity->GetHealth() <= 0) continue;
					if (entity->GetIsDormant()) continue;

					//player_resolver resolver;
					// new resolvor
					//resolver.run();

					if (SETTINGS::settings.aim_type == 0 && SETTINGS::settings.resolve_bool)
						resolver->resolve(entity);
				}
			} break;
		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
			break;
		case FRAME_RENDER_START:
			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			{
				if (in_tp)
				{
					SDK::CBaseAnimState* animstate = local_player->GetAnimState();

					if (!animstate)
						return;

					if (animstate->m_bInHitGroundAnimation && animstate->m_flHeadHeightOrOffsetFromHittingGroundAnimation)
						*reinterpret_cast<Vector*>(reinterpret_cast<DWORD>(local_player) + 0x31D8) = Vector(-10, global::angles.y, 0.f);
					else
						*reinterpret_cast<Vector*>(reinterpret_cast<DWORD>(local_player) + 0x31D8) = Vector(global::angles.x, global::angles.y, 0.f);

					INTERFACES::pPrediction->SetLocalViewAngles(global::real_angles);
					if (global::is_fakewalking) {
					    local_player->UpdateClientSideAnimation(); //update our client side animation
						local_player->GetAnimState()->m_flUnknownFraction = 0.f; //replace leg shuffling with leg sliding
					}
					else
					FixThemAnim(local_player); //do that premium anim fix m8
					//INTERFACES::pPrediction->SetLocalViewAngles(global::real_angles);

				}
				for (int i = 1; i <= 65; i++)
				{
					auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
					if (!entity) continue;
					if (entity == local_player) continue;

					*(int*)((uintptr_t)entity + 0xA30) = INTERFACES::Globals->framecount;
					*(int*)((uintptr_t)entity + 0xA28) = 0;
				}
			} break;

		case FRAME_NET_UPDATE_START:
			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			{
				if (SETTINGS::settings.beam_bool)
					visuals->DrawBulletBeams();

				for (int i = 1; i <= 12; i++)
				{
					g_soundesp.draw();
				}
			} break;
		case FRAME_NET_UPDATE_END:
			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			{
				for (int i = 1; i < 65; i++)
				{
					auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);

					if (!entity)
						continue;

					if (!local_player)
						continue;

					bool is_local_player = entity == local_player;
					bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

					if (is_local_player)
						continue;

					if (is_teammate)
						continue;

					if (entity->GetHealth() <= 0)
						continue;

					if (SETTINGS::settings.aim_type == 0)
						backtracking->DisableInterpolation(entity);
				}
			}
			break;
		case FRAME_RENDER_END:
			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame()) {

			}
		}
		original_frame_stage_notify(ecx, stage);
	}

	void __fastcall HookedDrawModelExecute(void* ecx, void* edx, SDK::IMatRenderContext* context, const SDK::DrawModelState_t& state, const SDK::ModelRenderInfo_t& render_info, matrix3x4_t* matrix)
	{
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			std::string ModelName = INTERFACES::ModelInfo->GetModelName(render_info.pModel);

			auto local_player = static_cast<SDK::CBaseEntity*>(INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer()));

			if (in_tp && render_info.entity_index == local_player->GetIndex() && local_player->GetIsScoped())
				INTERFACES::RenderView->SetBlend(SETTINGS::settings.transparency_amnt);

			if (SETTINGS::settings.NoFlash)
			{
				SDK::IMaterial* Flash = INTERFACES::MaterialSystem->FindMaterial("effects\\flashbang", "ClientEffect textures");
				SDK::IMaterial* FlashWhite = INTERFACES::MaterialSystem->FindMaterial("effects\\flashbang_white", "ClientEffect textures");
				Flash->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, true);
				FlashWhite->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, true);
			}

			if (ModelName.find("v_rif_ak47") != std::string::npos)
			{



				/*SDK::IMaterial* material = INTERFACES::MaterialSystem->FindMaterial(ModelName.c_str(), TEXTURE_GROUP_MODEL);
				if (!material) return;
				material->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, true);
				material->ColorModulate();
				INTERFACES::ModelRender->ForcedMaterialOverride(material);*/
			}
			/*for (int i = 1; i <= INTERFACES::Globals->maxclients; i++) {

				auto entity = static_cast<SDK::CBaseEntity*>(INTERFACES::ClientEntityList->GetClientEntity(i));

				if (!entity)
					continue;

				if (!entity->IsAlive())
					continue;

				glow g;

				if (entity->SetupBones(g.playerMatrix, 128, BONE_USED_BY_HITBOX, INTERFACES::Globals->curtime)) {

					SDK::IMaterial* material = INTERFACES::MaterialSystem->FindMaterial("dev/glow_armsrace.vmt", nullptr);

					INTERFACES::RenderView->SetBlend(0.f);
					INTERFACES::ModelRender->ForcedMaterialOverride(material);
					entity->DrawModel(0x1, 150);
					INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
				}
			}*/
		}
		original_draw_model_execute(ecx, context, state, render_info, matrix);
		INTERFACES::RenderView->SetBlend(1.f);
	}
	/*
notignorezmaterial->ColorModulate(CColor(SETTINGS::settings.localchams_col[0] * 255, SETTINGS::settings.localchams_col[1] * 255, SETTINGS::settings.localchams_col[2] * 255, SETTINGS::settings.localchams_col[3] * 255));
notignorezmaterial->ColorModulate(CColor(SETTINGS::settings.vmodel_col[0] * 255, SETTINGS::settings.vmodel_col[1] * 255, SETTINGS::settings.vmodel_col[2] * 255, SETTINGS::settings.vmodel_col[3] * 255));
ignorezmaterial->ColorModulate(CColor(SETTINGS::settings.imodel_col[0] * 255, SETTINGS::settings.imodel_col[1] * 255, SETTINGS::settings.imodel_col[2] * 255, SETTINGS::settings.imodel_col[3] * 255));
notignorezmaterial->ColorModulate(CColor(SETTINGS::settings.vmodel_col[0] * 255, SETTINGS::settings.vmodel_col[1] * 255, SETTINGS::settings.vmodel_col[2] * 255, SETTINGS::settings.vmodel_col[3] * 255));
notignorezmaterial->ColorModulate(CColor(SETTINGS::settings.teamvis_color[0] * 255, SETTINGS::settings.teamvis_color[1] * 255, SETTINGS::settings.teamvis_color[2] * 255, SETTINGS::settings.teamvis_color[3] * 255));
ignorezmaterial->ColorModulate(CColor(SETTINGS::settings.teaminvis_color[0] * 255, SETTINGS::settings.teaminvis_color[1] * 255, SETTINGS::settings.teaminvis_color[2] * 255, SETTINGS::settings.teaminvis_color[3] * 255));
*/
	void __fastcall HookedSceneEnd(void* ecx, void* edx)
	{
		original_scene_end(ecx);
		static SDK::IMaterial* ignorez = chams->CreateMaterialBasic(true, true, false);
		static SDK::IMaterial* notignorez = chams->CreateMaterialBasic(false, true, false);
		static SDK::IMaterial* ignorez_metallic = chams->CreateMaterialMetallic(true, true, false);
		static SDK::IMaterial* notignorez_metallic = chams->CreateMaterialMetallic(false, true, false);

		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{

			//static SDK::IMaterial* savanna = INTERFACES::MaterialSystem->("models/weapons/savanna", TEXTURE_GROUP_OTHER, false);

			CColor color = CColor(SETTINGS::settings.glow_col[0] * 255, SETTINGS::settings.glow_col[1] * 255, SETTINGS::settings.glow_col[2] * 255, SETTINGS::settings.glow_col[3] * 255), colorTeam = CColor(SETTINGS::settings.teamglow_color[0] * 255, SETTINGS::settings.teamglow_color[1] * 255, SETTINGS::settings.teamglow_color[2] * 255, SETTINGS::settings.teamglow_color[3] * 255), colorlocal = CColor(SETTINGS::settings.glowlocal_col[0] * 255, SETTINGS::settings.glowlocal_col[1] * 255, SETTINGS::settings.glowlocal_col[2] * 255, SETTINGS::settings.glowlocal_col[3] * 255);

			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player) return;
			for (int i = 1; i < 65; i++)
			{
				static SDK::IMaterial* mat = NULL;
				static SDK::IMaterial* mat2 = NULL;
				static SDK::IMaterial* mat3 = NULL;
				static SDK::IMaterial* mat4 = NULL;

				switch (SETTINGS::settings.chamstypeteam) {
				case 0: mat = INTERFACES::MaterialSystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", TEXTURE_GROUP_OTHER); break;
				case 1: mat = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER); break;
				case 2: mat = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER); break;
				case 3: mat = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_OTHER); break;
				case 4: mat = INTERFACES::MaterialSystem->FindMaterial("models/gibs/glass/glass", TEXTURE_GROUP_OTHER); break;
				case 5: mat = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_OTHER); break;
				case 6: mat = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_blue", TEXTURE_GROUP_OTHER); break;
				case 7: mat = INTERFACES::MaterialSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL); break;
				}

				switch (SETTINGS::settings.chamstype) {
				case 0: mat2 = INTERFACES::MaterialSystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", TEXTURE_GROUP_OTHER); break;
				case 1: mat2 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER); break;
				case 2: mat2 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER); break;
				case 3: mat2 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_OTHER); break;
				case 4: mat2 = INTERFACES::MaterialSystem->FindMaterial("models/gibs/glass/glass", TEXTURE_GROUP_OTHER); break;
				case 5: mat2 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_OTHER); break;
				case 6: mat2 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_blue", TEXTURE_GROUP_OTHER); break;
				case 7: mat2 = INTERFACES::MaterialSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL); break;
				}

				switch (SETTINGS::settings.chamstypebacktrack) {
				case 0: mat3 = INTERFACES::MaterialSystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", TEXTURE_GROUP_OTHER); break;
				case 1: mat3 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER); break;
				case 2: mat3 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER); break;
				case 3: mat3 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_OTHER); break;
				case 4: mat3 = INTERFACES::MaterialSystem->FindMaterial("models/gibs/glass/glass", TEXTURE_GROUP_OTHER); break;
				case 5: mat3 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_OTHER); break;
				case 6: mat3 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_blue", TEXTURE_GROUP_OTHER); break;
				case 7: mat3 = INTERFACES::MaterialSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL); break;
				}

				switch (SETTINGS::settings.chamstypeself) {
				case 0: mat4 = INTERFACES::MaterialSystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", TEXTURE_GROUP_OTHER); break;
				case 1: mat4 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER); break;
				case 2: mat4 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER); break;
				case 3: mat4 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_OTHER); break;
				case 4: mat4 = INTERFACES::MaterialSystem->FindMaterial("models/gibs/glass/glass", TEXTURE_GROUP_OTHER); break;
				case 5: mat4 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_OTHER); break;
				case 6: mat4 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_blue", TEXTURE_GROUP_OTHER); break;
				case 7: mat4 = INTERFACES::MaterialSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL); break;
				}

				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);

				if (!entity) continue;
				if (!local_player) continue;

				auto ent = INTERFACES::ClientEntityList->GetClientEntity(i);

				bool is_local_player = entity == local_player;
				bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;
				auto ignorezmaterial = ignorez_metallic;
				auto notignorezmaterial = notignorez_metallic;
				if (is_local_player)
				{

					static float rainbow;
					rainbow += SETTINGS::settings.rainbowself / 10500;
					if (rainbow > 1.f) rainbow = 0.f;

					switch (SETTINGS::settings.localchams)
					{
					case 0: continue; break;
					case 1:
						local_player->SetAbsOrigin(global::FakePosition);
						local_player->DrawModel(0x1, 255);
						local_player->SetAbsOrigin(local_player->GetAbsOrigin());
						continue; break;
					case 2:

						if (SETTINGS::settings.chamstypeselfcolor == 0)
							mat4->ColorModulate(CColor(SETTINGS::settings.localchams_col[0] * 255, SETTINGS::settings.localchams_col[1] * 255, SETTINGS::settings.localchams_col[2] * 255, SETTINGS::settings.localchams_col[3] * 255));
						else if (SETTINGS::settings.chamstypeselfcolor == 1)
							mat4->ColorModulate(CColor::FromHSB(rainbow, 1.f, 1.f));
						INTERFACES::ModelRender->ForcedMaterialOverride(mat4);
						local_player->DrawModel(0x1, 255);
						INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
						continue;  break;
					case 3:
						if (SETTINGS::settings.chamstypeselfcolor == 0)
							mat4->ColorModulate(CColor(SETTINGS::settings.localchams_col[0] * 255, SETTINGS::settings.localchams_col[1] * 255, SETTINGS::settings.localchams_col[2] * 255, SETTINGS::settings.localchams_col[3] * 255));
						else if (SETTINGS::settings.chamstypeselfcolor == 1)
							mat4->ColorModulate(CColor::FromHSB(rainbow, 1.f, 1.f));
						INTERFACES::ModelRender->ForcedMaterialOverride(mat4);
						local_player->SetAbsOrigin(global::FakePosition);
						local_player->DrawModel(0x1, 255);
						local_player->SetAbsOrigin(local_player->GetAbsOrigin());
						INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
						continue; break;
					}
				}

				if (entity->GetHealth() <= 0) continue;
				if (entity->GetIsDormant())	continue;
				if (entity->GetClientClass()->m_ClassID != 38) continue;

				if (is_teammate)
				{

					if (entity && SETTINGS::settings.chamsteam)
					{
						//mat->set_flag(MATERIAL_VAR_IGNOREZ, true);
						//interfaces::model_render->override_material(mat);
						//ent->draw_model(0x1, 255);
						//INTERFACES::model_render->override_material(nullptr);

						static float rainbow;
						rainbow += SETTINGS::settings.rainbowteam / 10500;
						if (rainbow > 1.f) rainbow = 0.f;

						//if (SETTINGS::settings.chamstypeteamcolor == 0)
						mat->Set_Flag(MATERIAL_VAR_IGNOREZ, true);
						if (SETTINGS::settings.chamstypeteamcolor == 0)
							mat->ColorModulate(CColor(SETTINGS::settings.teaminvis_color[0] * 255, SETTINGS::settings.teaminvis_color[1] * 255, SETTINGS::settings.teaminvis_color[2] * 255, SETTINGS::settings.teaminvis_color[3] * 255));
						else if (SETTINGS::settings.chamstypeteamcolor == 1)
							mat->ColorModulate(CColor::FromHSB(rainbow, 1.f, 1.f));
						INTERFACES::ModelRender->ForcedMaterialOverride(mat);
						entity->DrawModel(0x1, 255);
						INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);

						//notignorezmaterial->ColorModulate(SETTINGS::settings.teamvis_color);
						//INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
						//entity->DrawModel(0x1, 255);

						//INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
					}
				}
				if (!is_teammate)
				{
					if (entity && SETTINGS::settings.chams_type)
					{

						static float rainbow;
						rainbow += SETTINGS::settings.rainbowenemy / 10500;
						if (rainbow > 1.f) rainbow = 0.f;

						auto ent = INTERFACES::ClientEntityList->GetClientEntity(i);
						Vector oldOrigin = ent->GetVecOrigin();
						QAngle oldAngs = ent->GetAbsAnglesQang();
						if (entity && SETTINGS::settings.backtrack_chams)
						{
							for (int i = 1; i <= 12; i++)
							{
								//if (SETTINGS::settings.chamstypebacktrackcolor == 0)
								auto record = headPositions[ent->GetIndex()][i];
								ent->SetAbsOriginal(record.origin);
								ent->SetAbsAnglesVec(record.angs);
								Vector temp = ent->GetVecOrigin();
								mat3->Set_Flag(MATERIAL_VAR_IGNOREZ, true);
								mat3->ColorModulate(CColor(SETTINGS::settings.btvis_col[0] * 255, SETTINGS::settings.btvis_col[1] * 255, SETTINGS::settings.btvis_col[2] * 255, SETTINGS::settings.btvis_col[3] * 255));
								INTERFACES::ModelRender->ForcedMaterialOverride(mat3);
								INTERFACES::RenderView->SetBlend(0.35);
								ent->DrawModel(0x1, 255);
								INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
							}
							ent->SetAbsAnglesQang(oldAngs);
							ent->SetAbsOriginal(oldOrigin);

							/*INTERFACES::RenderView->SetBlend(0.9);
							mat3->Set_Flag(MATERIAL_VAR_IGNOREZ, true);
							INTERFACES::RenderView->SetColorModulation(SETTINGS::settings.btvis_col);
							INTERFACES::ModelRender->ForcedMaterialOverride(mat3);
							ent->DrawModel(0x1, 255);
							INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);*/

							static float rainbow;
							rainbow += SETTINGS::settings.rainbowenemy / 10500;
							if (rainbow > 1.f) rainbow = 0.f;

							mat2->Set_Flag(MATERIAL_VAR_IGNOREZ, true);
							if (SETTINGS::settings.chamstypecolor == 0)
								mat2->ColorModulate(CColor(SETTINGS::settings.imodel_col[0] * 255, SETTINGS::settings.imodel_col[1] * 255, SETTINGS::settings.imodel_col[2] * 255, SETTINGS::settings.imodel_col[3] * 255));
							else if (SETTINGS::settings.chamstypecolor == 1)
								mat2->ColorModulate(CColor::FromHSB(rainbow, 1.f, 1.f));
							INTERFACES::ModelRender->ForcedMaterialOverride(mat2);
							entity->DrawModel(0x1, 255);
							INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
						}
						else {
							mat2->Set_Flag(MATERIAL_VAR_IGNOREZ, true);
							if (SETTINGS::settings.chamstypecolor == 0)
								mat2->ColorModulate(CColor(SETTINGS::settings.imodel_col[0] * 255, SETTINGS::settings.imodel_col[1] * 255, SETTINGS::settings.imodel_col[2] * 255, SETTINGS::settings.imodel_col[3] * 255));
							else if (SETTINGS::settings.chamstypecolor == 1)
								mat2->ColorModulate(CColor::FromHSB(rainbow, 1.f, 1.f));
							INTERFACES::ModelRender->ForcedMaterialOverride(mat2);
							entity->DrawModel(0x1, 255);
							INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
						}

						/*INTERFACES::RenderView->SetBlend(1);
						if (SETTINGS::settings.chamstypecolor == 0)
							mat2->ColorModulate(SETTINGS::settings.imodel_col);
						else if (SETTINGS::settings.chamstypecolor == 1)
							mat2->ColorModulate(CColor::FromHSB(rainbow, 1.f, 1.f));
						INTERFACES::ModelRender->ForcedMaterialOverride(mat2);
						ent->DrawModel(0x1, 255);
						INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
						//*/

						/*mat2->Set_Flag(MATERIAL_VAR_IGNOREZ, true);
						if (SETTINGS::settings.chamstypecolor == 0)
							mat2->ColorModulate(SETTINGS::settings.imodel_col);
						else if (SETTINGS::settings.chamstypecolor == 1)
							mat2->ColorModulate(CColor::FromHSB(rainbow, 1.f, 1.f));
						INTERFACES::ModelRender->ForcedMaterialOverride(mat2);
						entity->DrawModel(0x1, 255);
						INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);*/
					}
				}
			}

			for (auto i = 0; i < INTERFACES::GlowObjManager->GetSize(); i++)
			{
				auto &glowObject = INTERFACES::GlowObjManager->m_GlowObjectDefinitions[i];
				auto entity = reinterpret_cast<SDK::CBaseEntity*>(glowObject.m_pEntity);

				if (!entity) continue;
				if (!local_player) continue;

				if (glowObject.IsUnused()) continue;

				bool is_local_player = entity == local_player;
				bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

				if (is_local_player && in_tp && SETTINGS::settings.glowlocal)
				{
					glowObject.m_nGlowStyle = SETTINGS::settings.glowstylelocal;
					glowObject.m_flRed = colorlocal.RGBA[0] / 255.0f;
					glowObject.m_flGreen = colorlocal.RGBA[1] / 255.0f;
					glowObject.m_flBlue = colorlocal.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = colorlocal.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
					continue;
				}
				else if (!SETTINGS::settings.glowlocal && is_local_player)
					continue;

				if (entity->GetHealth() <= 0) continue;
				if (entity->GetIsDormant())	continue;
				if (entity->GetClientClass()->m_ClassID != 38) continue;

				if (is_teammate && SETTINGS::settings.glowteam)
				{
					glowObject.m_nGlowStyle = SETTINGS::settings.glowstyle; //0;
					glowObject.m_flRed = colorTeam.RGBA[0] / 255.0f;
					glowObject.m_flGreen = colorTeam.RGBA[1] / 255.0f;
					glowObject.m_flBlue = colorTeam.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = colorTeam.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
					continue;
				}
				else if (is_teammate && !SETTINGS::settings.glowteam)
					continue;

				if (SETTINGS::settings.glowenable)
				{
					glowObject.m_nGlowStyle = SETTINGS::settings.glowstyle;//0;
					glowObject.m_flRed = color.RGBA[0] / 255.0f;
					glowObject.m_flGreen = color.RGBA[1] / 255.0f;
					glowObject.m_flBlue = color.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = color.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
				}
			}

			if (SETTINGS::settings.smoke_bool)
			{
				std::vector<const char*> vistasmoke_wireframe = { "particle/vistasmokev1/vistasmokev1_smokegrenade" };

				std::vector<const char*> vistasmoke_nodraw =
				{
					"particle/vistasmokev1/vistasmokev1_fire",
					"particle/vistasmokev1/vistasmokev1_emods",
					"particle/vistasmokev1/vistasmokev1_emods_impactdust",
				};

				for (auto mat_s : vistasmoke_wireframe)
				{
					SDK::IMaterial* mat = INTERFACES::MaterialSystem->FindMaterial(mat_s, TEXTURE_GROUP_OTHER);
					mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, true); //wireframe
				}

				for (auto mat_n : vistasmoke_nodraw)
				{
					SDK::IMaterial* mat = INTERFACES::MaterialSystem->FindMaterial(mat_n, TEXTURE_GROUP_OTHER);
					mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, true);
				}

				static auto smokecout = *(DWORD*)(linegoesthrusmoke + 0x8);
				*(int*)(smokecout) = 0;
			}
		}
	}
	void __fastcall HookedOverrideView(void* ecx, void* edx, SDK::CViewSetup* pSetup)
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player) return;

		auto animstate = local_player->GetAnimState();
		if (!animstate) return;

		if (GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.thirdperson_int)) & 1)
			in_tp = !in_tp;

		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			//GrenadePrediction::instance().View(pSetup);
			auto GetCorrectDistance = [&local_player](float ideal_distance) -> float //lambda
			{
				Vector inverse_angles;
				INTERFACES::Engine->GetViewAngles(inverse_angles);

				inverse_angles.x *= -1.f, inverse_angles.y += 180.f;

				Vector direction;
				MATH::AngleVectors(inverse_angles, &direction);

				SDK::CTraceWorldOnly filter;
				SDK::trace_t trace;
				SDK::Ray_t ray;

				ray.Init(local_player->GetVecOrigin() + local_player->GetViewOffset(), (local_player->GetVecOrigin() + local_player->GetViewOffset()) + (direction * (ideal_distance + 5.f)));
				INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

				return ideal_distance * trace.flFraction;
			};

			if (SETTINGS::settings.tp_bool && in_tp)
			{
				if (local_player->GetHealth() <= 0)
					local_player->SetObserverMode(5);

				if (!INTERFACES::Input->m_fCameraInThirdPerson)
				{
					INTERFACES::Input->m_fCameraInThirdPerson = true;
					INTERFACES::Input->m_vecCameraOffset = Vector(global::real_angles.x, global::real_angles.y, GetCorrectDistance(100));

					Vector camForward;
					MATH::AngleVectors(Vector(INTERFACES::Input->m_vecCameraOffset.x, INTERFACES::Input->m_vecCameraOffset.y, 0), &camForward);
				}
			}
			else
			{
				INTERFACES::Input->m_fCameraInThirdPerson = false;
				INTERFACES::Input->m_vecCameraOffset = Vector(global::real_angles.x, global::real_angles.y, 0);
			}

			auto zoomsensration = INTERFACES::cvar->FindVar("zoom_sensitivity_ratio_mouse");
			if (SETTINGS::settings.fixscopesens)
				zoomsensration->SetValue("0");
			else
				zoomsensration->SetValue("1");

			if (SETTINGS::settings.aim_type == 0) {

				if (!local_player->GetIsScoped())
					pSetup->fov = SETTINGS::settings.fov_val;
				else if (local_player->GetIsScoped() && SETTINGS::settings.removescoping)
					pSetup->fov = SETTINGS::settings.fov_val;
				if (!local_player->GetIsScoped() && SETTINGS::settings.tp_bool && in_tp)
					pSetup->fov = SETTINGS::settings.tpfov_val;
			}
			else if (!(SETTINGS::settings.aim_type == 0) && !local_player->GetIsScoped())
				pSetup->fov = 90;
		}
		original_override_view(ecx, pSetup);
	}
	void __fastcall HookedTraceRay(void *thisptr, void*, const SDK::Ray_t &ray, unsigned int fMask, SDK::ITraceFilter *pTraceFilter, SDK::trace_t *pTrace)
	{
		original_trace_ray(thisptr, ray, fMask, pTraceFilter, pTrace);
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			pTrace->surface.flags |= SURF_SKY;
	}
	bool __fastcall HookedGetBool(void* pConVar, void* edx)
	{
		if ((uintptr_t)_ReturnAddress() == CAM_THINK)
			return true;

		return original_get_bool(pConVar);
	}
	float __fastcall GetViewmodelFOV()
	{
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			float player_fov = original_viewmodel_fov();

			if (SETTINGS::settings.Enabled)
				player_fov = SETTINGS::settings.viewfov_val;

			return player_fov;
		}
	}






	void OpenMenu()
	{
		static bool is_down = false;
		static bool is_clicked = false;
		if (GetAsyncKeyState(VK_INSERT))
		{
			is_clicked = false;
			is_down = true;
		}
		else if (!GetAsyncKeyState(VK_INSERT) && is_down)
		{
			is_clicked = true;
			is_down = false;
		}
		else
		{
			is_clicked = false;
			is_down = false;
		}

		if (is_clicked)
		{
			menu_open = !menu_open;

		}
	}

	MDLHandle_t __fastcall HK_FindMDL(void* ecx, void* edx, char* FilePath)
	{

		if (strstr(FilePath, "knife_default_ct.mdl") || strstr(FilePath, "knife_default_t.mdl"))
		{
			sprintf(FilePath, "models/weapons/v_minecraft_pickaxe.mdl");
		}
		return oFindMDL(ecx, FilePath);
	}

	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg) {
		case WM_LBUTTONDOWN:
			PressedKeys[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			PressedKeys[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			PressedKeys[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			PressedKeys[VK_RBUTTON] = false;
			break;
		case WM_MBUTTONDOWN:
			PressedKeys[VK_MBUTTON] = true;
			break;
		case WM_MBUTTONUP:
			PressedKeys[VK_MBUTTON] = false;
			break;
		case WM_XBUTTONDOWN:
		{
			UINT button = GET_XBUTTON_WPARAM(wParam);
			if (button == XBUTTON1)
			{
				PressedKeys[VK_XBUTTON1] = true;
			}
			else if (button == XBUTTON2)
			{
				PressedKeys[VK_XBUTTON2] = true;
			}
			break;
		}
		case WM_XBUTTONUP:
		{
			UINT button = GET_XBUTTON_WPARAM(wParam);
			if (button == XBUTTON1)
			{
				PressedKeys[VK_XBUTTON1] = false;
			}
			else if (button == XBUTTON2)
			{
				PressedKeys[VK_XBUTTON2] = false;
			}
			break;
		}
		case WM_KEYDOWN:
			PressedKeys[wParam] = true;
			break;
		case WM_KEYUP:
			PressedKeys[wParam] = false;
			break;
		default: break;
		}

		OpenMenu();

		if (d3d_init && menu_open && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		return CallWindowProc(INIT::OldWindow, hWnd, uMsg, wParam, lParam);
	}


	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		ImGui_ImplDX9_Init(INIT::Window, pDevice);


		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 15.0f);


		ImGuiStyle &styled = ImGui::GetStyle();
		ImVec4* colors = ImGui::GetStyle().Colors;

		style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); //white
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.85f, 0.85f, 0.85f, 0.85f); //main quarter
		style.Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 0.411f, 0.705f, 1.0f); //main quarter
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.07f, 0.60f); //main bg
		style.Colors[ImGuiCol_Border] = ImVec4(0.14f, 0.16f, 0.19f, 0.60f); //main border
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); //dark
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f); //main bg
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 0.30f, 0.10f, 0.50f); //main colored
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f); //main bg
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.24f, 0.28f, 0.75f); //collapsed
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f); //main bg
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.70f); //main bg
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f); //main bg
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); //main half
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.70f); //main half
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f); //main bg
		style.Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); //main half
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
		style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); //main
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f); //main
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
		style.Colors[ImGuiCol_Header] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); //main
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f); //main
		style.Colors[ImGuiCol_Column] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f); //main border  sc
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(1.00f, 0.30f, 0.10f, 0.50f); //main colored
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f); //main colored
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.90f, 0.90f, 0.90f, 0.75f); //main white
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f); //main white
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f); //main white
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.86f, 0.93f, 0.89f, 0.00f); //dark
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.86f, 0.93f, 0.89f, 0.40f); //close button
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.86f, 0.93f, 0.89f, 0.90f); //close button
		style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.30f, 0.10f, 0.50f); //main colored
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.30f, 0.10f, 0.50f); //main colored
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.07f, 0.70f); //main bg
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.24f, 0.28f, 0.60f); //collapsed

		style.WindowRounding = 0.f;

		/*style.WindowPadding = ImVec2(32, 32);
		style.WindowRounding = 5.0f;
		style.WindowMinSize = ImVec2(32, 32);
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.ChildWindowRounding = 0.0f;
		style.FramePadding = ImVec2(5, 5);
		style.FrameRounding = 4.0f;
		style.ItemSpacing = ImVec2(12, 8);
		style.ItemInnerSpacing = ImVec2(8, 6);
		style.IndentSpacing = 25.0f;
		style.ScrollbarSize = 15.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 3.0f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.DisplayWindowPadding = ImVec2(22, 22);
		style.DisplaySafeAreaPadding = ImVec2(4, 4);
		style.AntiAliasedLines = true;
		style.AntiAliasedShapes = true;
		style.CurveTessellationTol = 1.25f;*/

		pDevice->GetViewport(&Menuxd::viewPort);
		D3DXCreateFont(pDevice, 9, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, ("Verdana"), &Menuxd::fntVerdana9);
		D3DXCreateFont(pDevice, 10, 5, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, ("Verdana"), &Menuxd::fntVerdana10);
		D3DXCreateFont(pDevice, 11, 5, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, ("Verdana"), &Menuxd::fntVerdana11);
		//D3DXCreateFont(pDevice, 12, 5, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, ("Verdana"), &Menuxd::fntVerdana12);
		D3DXCreateFont(pDevice, 11, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, ("csgo_icons"), &Menuxd::fntWeaponIcon);
		font_menu = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(gamer, 34952, 14.f);
		font_menu2 = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(cherryfont, 9800, 48.f);
		coolfont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(sidetabs, 34952, 24.f);
		coolfont2 = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(sidetabs, 34952, 14.f);

		d3d_init = true;

		
	}
	long __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
	{
		static auto ofunc = directz.get_original<EndSceneFn>(42);

		D3DCOLOR rectColor = D3DCOLOR_XRGB(255, 0, 0);
		D3DRECT BarRect = { 1, 1, 1, 1 };
		ImGuiStyle& style = ImGui::GetStyle();
		pDevice->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, rectColor, 0, 0);

		if (!d3d_init)
			GUI_Init(pDevice);

		//ImGui::GetIO().MouseDrawCursor = menu_open;

		static const D3DRENDERSTATETYPE backupStates[] = { D3DRS_COLORWRITEENABLE, D3DRS_ALPHABLENDENABLE, D3DRS_SRCBLEND, D3DRS_DESTBLEND, D3DRS_BLENDOP, D3DRS_FOGENABLE };
		static const int size = sizeof(backupStates) / sizeof(DWORD);

		DWORD oldStates[size] = { 0 };

		for (int i = 0; i < size; i++)
			pDevice->GetRenderState(backupStates[i], &oldStates[i]);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);

		ImGui_ImplDX9_NewFrame();

		g_Menu->Menu(); //THIS IS THE ENTRY POINT FOR THE MENU
		
		ImGui::Render();

		//if (SETTINGS::settings.Visuals.Enabled)
			//D9Visuals::Render(pDevice);

		if (SETTINGS::settings.spread_bool)
			Menuxd::drawfatalpricel(pDevice);

		for (int i = 0; i < size; i++)
			pDevice->SetRenderState(backupStates[i], oldStates[i]);

		return ofunc(pDevice);

	}

	HRESULT __stdcall hooked_present(IDirect3DDevice9* device, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion) {

		static auto ofunc = directz.get_original<hooked_presentFn>(17);

		return ofunc(device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}

	long __stdcall Hooked_EndScene_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto ofunc = directz.get_original<EndSceneResetFn>(16);

		if (!d3d_init)
			return ofunc(pDevice, pPresentationParameters);

		ImGui_ImplDX9_InvalidateDeviceObjects();

		auto hr = ofunc(pDevice, pPresentationParameters);

		ImGui_ImplDX9_CreateDeviceObjects();

		return hr;
	}


	typedef bool(__thiscall *FireEventClientSide)(void*, SDK::IGameEvent*);

	bool __fastcall Hooked_FireEventClientSide(void *ecx, void* edx, SDK::IGameEvent* pEvent) {

		static auto ofunct = fireevent.get_original<FireEventClientSide>(9);

		if (!pEvent)
			return ofunct(ecx, pEvent);

		DamageESP::HandleGameEvent(pEvent);

		/*
		if (g_Options.NewLegitbot.Aimbot.bKillDelay && g_Options.NewLegitbot.Aimbot.Enabled)
		{

		if (strcmp(pEvent->GetName(), "player_death") == 0)
		{
		int attacker = Interfaces.pEngine->GetPlayerForUserID(pEvent->GetInt("attacker"));
		int iLocalPlayer = Interfaces.pEngine->GetLocalPlayer();
		if (attacker == iLocalPlayer)
		{
		G::KillStopDelay = Interfaces.pGlobalVars->curtime + g_Options.NewLegitbot.Aimbot.iKillDelay;
		}
		}
		}
		*/


		return ofunct(ecx, pEvent);
	}

	void InitHooks()
	{
		iclient_hook_manager.Init(INTERFACES::Client);
		original_frame_stage_notify = reinterpret_cast<FrameStageNotifyFn>(iclient_hook_manager.HookFunction<FrameStageNotifyFn>(37, HookedFrameStageNotify));

		panel_hook_manager.Init(INTERFACES::Panel);
		original_paint_traverse = reinterpret_cast<PaintTraverseFn>(panel_hook_manager.HookFunction<PaintTraverseFn>(41, HookedPaintTraverse));

		model_render_hook_manager.Init(INTERFACES::ModelRender);
		original_draw_model_execute = reinterpret_cast<DrawModelExecuteFn>(model_render_hook_manager.HookFunction<DrawModelExecuteFn>(21, HookedDrawModelExecute));

		scene_end_hook_manager.Init(INTERFACES::RenderView);
		original_scene_end = reinterpret_cast<SceneEndFn>(scene_end_hook_manager.HookFunction<SceneEndFn>(9, HookedSceneEnd));

		trace_hook_manager.Init(INTERFACES::Trace);
		original_trace_ray = reinterpret_cast<TraceRayFn>(trace_hook_manager.HookFunction<TraceRayFn>(5, HookedTraceRay));

		override_view_hook_manager.Init(INTERFACES::ClientMode);
		original_override_view = reinterpret_cast<OverrideViewFn>(override_view_hook_manager.HookFunction<OverrideViewFn>(18, HookedOverrideView));
		original_create_move = reinterpret_cast<CreateMoveFn>(override_view_hook_manager.HookFunction<CreateMoveFn>(24, HookedCreateMove));
		original_viewmodel_fov = reinterpret_cast<GetViewmodelFOVFn>(override_view_hook_manager.HookFunction<GetViewmodelFOVFn>(35, GetViewmodelFOV));

		auto sv_cheats = INTERFACES::cvar->FindVar("sv_cheats");
		get_bool_manager = VMT::VMTHookManager(reinterpret_cast<DWORD**>(sv_cheats));
		original_get_bool = reinterpret_cast<SvCheatsGetBoolFn>(get_bool_manager.HookFunction<SvCheatsGetBoolFn>(13, HookedGetBool));

		INTERFACES::cvar->get_convar("viewmodel_offset_x")->callbacks.SetSize(0);
		INTERFACES::cvar->get_convar("viewmodel_offset_y")->callbacks.SetSize(0);
		INTERFACES::cvar->get_convar("viewmodel_offset_z")->callbacks.SetSize(0);

		fireevent.setup(INTERFACES::GameEventManager);
		fireevent.hook_index(9, Hooked_FireEventClientSide);


		if (!(INIT::Window = FindWindowA("Valve001", nullptr)))
			Sleep(100);

		if (INIT::Window)
			INIT::OldWindow = (WNDPROC)SetWindowLongPtr(INIT::Window, GWL_WNDPROC, (LONG_PTR)Hooked_WndProc);

		DWORD DeviceStructureAddress = **(DWORD**)(UTILS::FindSignature("shaderapidx9.dll", "A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C") + 1);

		// fluxi if (devicestructureaddress)

		directz.setup((DWORD**)DeviceStructureAddress);
		directz.hook_index(16, Hooked_EndScene_Reset);
		directz.hook_index(17, hooked_present);
		directz.hook_index(42, Hooked_EndScene);

	}
	void EyeAnglesPitchHook(const SDK::CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast<SDK::CBaseEntity*>(pStruct);
		if (!entity)
			return;

	}
	void EyeAnglesYawHook(const SDK::CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast<SDK::CBaseEntity*>(pStruct);
		if (!entity)
			return;
	}
	void InitNetvarHooks()
	{
		UTILS::netvar_hook_manager.Hook("DT_CSPlayer", "m_angEyeAngles[0]", EyeAnglesPitchHook);
		UTILS::netvar_hook_manager.Hook("DT_CSPlayer", "m_angEyeAngles[1]", EyeAnglesYawHook);
	}
}

const char* key_binds[] = { "none", "mouse1", "mouse2", "mouse3", "mouse4", "mouse5", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12", "control", "alt" };

void MenuTabs::Tab1() {
	
	
}

void MenuTabs::Tab2() {


}

void ImDrawRectRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow)
{
	ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

	CColor colColor(0, 0, 0, 255);

	flRainbow += flSpeed;
	if (flRainbow > 1.f) flRainbow = 0.f;//1 0 

	for (int i = 0; i < width; i = i + 1)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		CColor colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
		windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), colRainbow.GetU32());
	}
}

struct hud_weapons_t {
	std::int32_t* get_weapon_count() {
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};

void KnifeApplyCallbk() {

	static auto clear_hud_weapon_icon_fn =
		reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(
			UTILS::FindSignature("client_panorama.dll", "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C 89 5D FC"));

	auto element = FindHudElement<std::uintptr_t*>("CCSGO_HudWeaponSelection");

	auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xA0);
	if (hud_weapons == nullptr)
		return;

	if (!*hud_weapons->get_weapon_count())
		return;

	for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
		i = clear_hud_weapon_icon_fn(hud_weapons, i);

	static SDK::ConVar* Meme = INTERFACES::cvar->FindVar("cl_fullupdate");
	Meme->nFlags &= ~FCVAR_CHEAT;
	INTERFACES::Engine->ClientCmd_Unrestricted("cl_fullupdate");

}

SDK::CBaseWeapon* xd(SDK::CBaseEntity* xz)
{
	if (!INTERFACES::Engine->IsConnected())
		return nullptr;
	if (!xz->IsAlive())
		return nullptr;

	HANDLE weaponData = *(HANDLE*)((DWORD)xz + OFFSETS::m_hActiveWeapon);
	return (SDK::CBaseWeapon*)INTERFACES::ClientEntityList->GetClientEntityFromHandle(weaponData);
}

short SafeWeaponID()
{
	SDK::CBaseEntity* local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!(local_player))
		return 0;

	SDK::CBaseWeapon* WeaponC = xd(local_player);

	if (!(WeaponC))
		return 0;

	return WeaponC->GetItemDefenitionIndex();
}





template<typename T, int N>
inline size_t array_size(T(&)[N])
{
	return N;
}

#define ARRAY_SIZE(X)   (sizeof(array_size(X)) ? (sizeof(X) / sizeof((X)[0])) : -1)

const char* tabNames[] = { "A", "C", "D", "G", "B", "F" };

void CMenu::Menu() {

	POINT mp;

	GetCursorPos(&mp);
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos.x = mp.x;
	io.MousePos.y = mp.y;

	ImGui::GetIO().MouseDrawCursor = menu_open;

	if (!menuVars.set)
		SetupVariables();

	CheckForResolutionChange();

	if (menu_open) {

		MenuAnimations();
		/*DrawTabsOnTop();
		DrawMenu();*/

		if (tabsOnTop) { //OLD MENU CODE WITH CHANGABLE TABS : TOP TABS LOOK BETTER
			DrawTabsOnTop();
			DrawMenu();
		}
		else {
			DrawTabBackground();
			DrawTabs();
			DrawMenu();
		}
	}else {
		menuVars.alpha = 0;
		tabMenuVars.position = Vector2D(-tabMenuVars.size.x, 0);
	}
}

void CMenu::DrawTabBackground() {

	


		TabStyle();

		ImGui::SetNextWindowPos(ImVec2(tabMenuVars.position.x, tabMenuVars.position.y));
		ImGui::SetNextWindowSize(ImVec2(tabMenuVars.size.x, tabMenuVars.size.y));

		ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

		ImGui::PushFont(coolfont);
		{

		ImGui::Begin("##tab_background", &menu_open, ImVec2(tabMenuVars.size.x, tabMenuVars.size.y), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		{
			TabStyle();
			ImGui::SetWindowPos(ImVec2(tabMenuVars.position.x, tabMenuVars.position.y));
			ImGui::SetWindowSize(ImVec2(tabMenuVars.size.x, tabMenuVars.size.y));

			ImGui::BeginChild("##childdd", ImVec2(tabMenuVars.size.x*0.98, tabMenuVars.size.y));
			{
			}
			ImGui::EndChild();
		}
		ImGui::End();

	}
	ImGui::PopFont();
}

//kewlcolor ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

void CMenu::DrawTabs() {

	TabStyle();

	ImGui::PushFont(font_menu2);
	{

		int buttonHeight = tabMenuVars.size.y * 0.075;
		int childHeight = buttonHeight * array_size(tabNames); // * by number of tabs
		int positionY = (menuVars.screenSize.y - childHeight) / 2;

		ImGui::SetNextWindowPos(ImVec2(tabMenuVars.position.x, positionY));
		ImGui::SetNextWindowSize(ImVec2(tabMenuVars.size.x*0.98, childHeight));


		ImGui::Begin("##tab_buttons", &menu_open, ImVec2(tabMenuVars.size.x*0.98, tabMenuVars.size.y), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
		{
			ImGui::SetWindowPos(ImVec2(tabMenuVars.position.x, positionY));
			ImGui::SetWindowSize(ImVec2(tabMenuVars.size.x*0.98, childHeight));

			ImGui::BeginChild("##tab_child", ImVec2(tabMenuVars.size.x, childHeight));
			{

				for (auto i = 0; i < array_size(tabNames); i++)
				{
					if (currentTab == i) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.09f, 0.12f, 1.00f));
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
						if (ImGui::Button(tabNames[i], ImVec2(tabMenuVars.size.x*0.98, buttonHeight))) {

						}
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						continue;
					}

					if (ImGui::Button(tabNames[i], ImVec2(tabMenuVars.size.x, buttonHeight))) {
						currentTab = i;
					}
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();

	}
	ImGui::PopFont();


}

void CMenu::DrawTabsOnTop() {
		TabStyle();

		int buttonHeight = tabMenuVars.size.y * 0.05;
		int buttonWidth = menuVars.size.x / array_size(tabNames);

		ImGui::SetNextWindowPos(ImVec2(menuVars.position.x, 0));
		ImGui::SetNextWindowSize(ImVec2(menuVars.size.x, buttonHeight));

		ImGui::PushFont(font_menu2);
		{

		ImGui::Begin("##tab_buttons", &menu_open, ImVec2(menuVars.size.x, buttonHeight), menuVars.alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
		{
			ImGui::SetWindowPos(ImVec2(menuVars.position.x, 0));
			ImGui::SetWindowSize(ImVec2(menuVars.size.x, buttonHeight));
			for (auto i = 0; i < array_size(tabNames); i++)
			{
				if (currentTab == i) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
					if (ImGui::Button(tabNames[i], ImVec2(buttonWidth, buttonHeight))) {

					}
					ImGui::PopStyleColor();
					ImGui::SameLine();
					continue;
				}

				if (ImGui::Button(tabNames[i], ImVec2(buttonWidth, buttonHeight))) {
					currentTab = i;
				}
				ImGui::SameLine();
			}
		}
		ImGui::End();
	}
	ImGui::PopFont();

}

void CMenu::DrawMenu() {

		MenuStyle();

		ImGui::SetNextWindowSize(ImVec2(menuVars.size.x, menuVars.size.y));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));

		ImGui::PushFont(coolfont2);
		{

		ImGui::Begin("nnware BETA", &menu_open, ImVec2(menuVars.position.x, menuVars.position.y), menuVars.alpha, /*ImGuiWindowFlags_NoResize |*/ ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_OnlyDragByTitleBar);
		{

			MenuStyle();
			ImGui::SetWindowSize(ImVec2(menuVars.size.x, menuVars.size.y));


			/*if (SETTINGS::settings.SkeetBar)
			{
				static float flRainbow;
				float flSpeed = 0.001;
				int curWidth = 4;
				ImVec2 curPos = ImGui::GetCursorPos();
				ImVec2 curWindowPos = ImGui::GetWindowPos();
				curPos.x += curWindowPos.x;
				curPos.y += curWindowPos.y;
				int size;
				int y;
				INTERFACES::Engine->GetScreenSize(y, size);
				ImDrawRectRainbow(100, 100, 100, 100, flSpeed, flRainbow);//10 5 4
			}*/

			if (menuVars.alpha >= 1.0f) { //only draw menu items when alpha is 1, fixes fade in bug
				if (currentTab == 0)
					RagebotTab();
				else if (currentTab == 1)
					aatab();
				else if (currentTab == 2)
					VisualsTab();
				else if (currentTab == 3)
					MiscTab();
				else if (currentTab == 4)
					SkinsTab();
				else if (currentTab == 5)
					ConfigTab();
				else if (currentTab == 6)
					radioTab();
				else
					currentTab = 0; //incase of bug somewhere will reset tab
			}

			if (ImGui::GetWindowPos().x < tabMenuVars.size.x) {
				ImGui::SetWindowPos(ImVec2(tabMenuVars.size.x, ImGui::GetWindowPos().y));
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();

	}
	ImGui::PopFont();
}

void CMenu::MenuAnimations() {
	// TAB ANIMATION
	if ((tabMenuVars.position.x += tabMenuVars.speed) > 0)
		tabMenuVars.position.x = 0;
	else
		tabMenuVars.position.x += tabMenuVars.speed;

	//MENU ALPHA ANIM
	if (menuVars.alpha < 1.0f) {
		menuVars.alpha += menuVars.alphaSpeed;
	}

	// RAINBOW ANIM FROM LEXIS RAINBOW SCOPE
	rainbow += 0.001;

	if (rainbow > 1.f)
		rainbow = 0.f;

	
}

void CMenu::MenuStyle() {
	ImGuiStyle style = ImGui::GetStyle();

	CColor color = CColor::FromHSB(rainbow, 1.f, 1.f);
	ImVec4 col = ImVec4{ (float)color.RGBA[0]/255, (float)color.RGBA[1]/255, (float)color.RGBA[2]/255, (float)color.RGBA[3]/255};

	style.WindowPadding = ImVec2(15, 15);
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.FramePadding = ImVec2(5, 5);
	style.FrameRounding = 0.0f;
	style.ItemSpacing = ImVec2(12, 8);
	style.ItemInnerSpacing = ImVec2(2, 3);
	style.IndentSpacing = 1.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;

	style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);//
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	/*style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.0f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(1.0f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);*/

	ImGui::GetStyle() = style;
}

void CMenu::TabStyle() {
	ImGuiStyle style = ImGui::GetStyle();

	style.WindowPadding = ImVec2(0, 0);
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.FramePadding = ImVec2(0, 0);
	style.FrameRounding = 0.0f;
	style.ItemSpacing = ImVec2(0, 0);
	style.ItemInnerSpacing = ImVec2(0, 0);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;


	style.Colors[ImGuiCol_Text] = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 0.411f, 0.705f, 1.0f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);

	ImGui::GetStyle() = style;
}

void CMenu::SetupVariables() {
	int sX, sY;
	INTERFACES::Engine->GetScreenSize(sX, sY);

	menuVars.screenSize = Vector2D(sX, sY);
	menuVars.size = Vector2D(sX*menuVars.scale.x, sY*menuVars.scale.y);
	menuVars.position = Vector2D((sX - menuVars.size.x) / 2, (sY - menuVars.size.y) / 2);


	tabMenuVars.size = Vector2D(menuVars.screenSize.x * 0.15, menuVars.screenSize.y);
	tabMenuVars.position = Vector2D(-tabMenuVars.size.x, 0);

	menuVars.set = true;
}

void CMenu::CheckForResolutionChange() {
	int sX, sY;

	INTERFACES::Engine->GetScreenSize(sX, sY);

	if (Vector2D(sX, sY) != menuVars.screenSize)
		SetupVariables();
}

/*MENU TABS*/

void CMenu::RagebotTab() {

	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + ImGui::GetStyle().ItemSpacing.x)) / 2;

	const char* hitboxes[] = { "Head", "Body Aim", "Hitscan" };
	const char* hitbones[] = { "Head", "Neck", "Thorax", "Chest", "Stomach", "Pelvis", "Left arm", "Right arm", "Left hand", "Right hand", "Left thigh", "Right thigh", "Left shin", "Right shin", "Left foot", "Right foot" };

	ImGui::PushFont(font_menu);
	{

		ImGui::BeginChild("#aimbot_setting", ImVec2(childX, 0), true); {

			ImGui::Checkbox("Enable Aimbot", &SETTINGS::settings.aim_bool);
			ImGui::SliderInt("Hit-Chance", &SETTINGS::settings.chance_val, 0, 100);
			ImGui::SliderInt("Minimum Damage", &SETTINGS::settings.damage_val, 1, 100);
			ImGui::Checkbox(("More Aimpoints"), &SETTINGS::settings.multi_bool);
			if (SETTINGS::settings.multi_bool)
			{
				ImGui::SliderFloat("Head Scale", &SETTINGS::settings.point_val, 0, 1);
				ImGui::SliderFloat("Body Scale", &SETTINGS::settings.body_val, 0, 1);
			}
			ImGui::Checkbox(("Auto zeus"), &SETTINGS::settings.autozeus_bool);//
			ImGui::Checkbox(("Smart auto knife"), &SETTINGS::settings.autoknife_bool);//
			ImGui::Checkbox(("Auto Stop"), &SETTINGS::settings.stop_bool);
			ImGui::Checkbox(("Auto Scope"), &SETTINGS::settings.autoscope);
			ImGui::Checkbox(("Lag Compensation"), &SETTINGS::settings.rage_lagcompensation);
			ImGui::Checkbox(("Fakelag Prediction"), &SETTINGS::settings.fake_bool);
			//ImGui::Combo("baim on key", &SETTINGS::settings.baim_key, key_binds, sizeof(key_binds));
		} ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("#aimbot_settings", ImVec2(childX, 0), true); {

			ImGui::Text("Hitboxes");
			ImGui::Combo("##Hitboxes", &SETTINGS::settings.acc_type, hitboxes, ARRAYSIZE(hitboxes));
			/*ImGui::ListBoxHeader("Hitscan"); {
			ImGui::Selectable("head", &SETTINGS::settings.hitscan_head);
			ImGui::Selectable("neck", &SETTINGS::settings.hitscan_neck);
			ImGui::Selectable("chest", &SETTINGS::settings.hitscan_chest);
			ImGui::Selectable("pelvis", &SETTINGS::settings.hitscan_pelvis);
			ImGui::Selectable("arms", &SETTINGS::settings.hitscan_arms);
			ImGui::Selectable("legs", &SETTINGS::settings.hitscan_legs);
			}
			ImGui::ListBoxFooter();*/
			for (int i = 0; i < ARRAYSIZE(hitbones); ++i)
			{
				ImGui::Selectable(hitbones[i], SETTINGS::settings.multiHitboxes[i]);
			}
			//ImGui::Checkbox(("smart body-aim"), &SETTINGS::settings.smartbaim);

		} ImGui::EndChild();

	}
	ImGui::PopFont();

}

void CMenu::aatab()
{
	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + ImGui::GetStyle().ItemSpacing.x)) / 2;

	ImGui::PushFont(font_menu);
	{
		ImGui::BeginChild("#antiaims", ImVec2(childX, 0), true); {

			const char* antiaimmode[] = { "Standing", "Moving", "Jumping" ,"Fakewalking" };
			const char* aa_pitch[] = { "None", "Emotion", "Fake Down", "Fake Up", "Fake Zero" };
			const char* aa_mode[] = { "None", "Backwards", "Manual", "Freestanding" };
			const char* aa_fake[] = { "None", "Backjitter", "Random", "Local View", "Opposite", "Rotational" };

			ImGui::Checkbox(("Enable Hentai-Aim"), &SETTINGS::settings.aa_bool);
			ImGui::Text("Mode");
			ImGui::Combo(("##mode"), &SETTINGS::settings.aa_mode, antiaimmode, ARRAYSIZE(antiaimmode));

			switch (SETTINGS::settings.aa_mode) {

			case 0: {

				ImGui::Text("Pitch");
				ImGui::Combo(("pitch##st"), &SETTINGS::settings.aa_pitch_type, aa_pitch, ARRAYSIZE(aa_pitch));
				ImGui::Text("Real");
				ImGui::Combo(("real##st"), &SETTINGS::settings.aa_real_type, aa_mode, ARRAYSIZE(aa_mode));
				//ImGui::Combo(("fake##st"), &SETTINGS::settings.aa_fake_type, aa_fake, ARRAYSIZE(aa_fake));
				break;
			}

			case 1: {

				ImGui::Text("Pitch");
				ImGui::Combo(("pitch##moving"), &SETTINGS::settings.aa_pitch1_type, aa_pitch, ARRAYSIZE(aa_pitch));
				ImGui::Text("Real");
				ImGui::Combo(("real##moving"), &SETTINGS::settings.aa_real1_type, aa_mode, ARRAYSIZE(aa_mode));
				//ImGui::Combo(("fake##moving"), &SETTINGS::settings.aa_fake1_type, aa_fake, ARRAYSIZE(aa_fake));
				break;
			}

			case 2: {

				ImGui::Text("Pitch");
				ImGui::Combo(("pitch##jumping"), &SETTINGS::settings.aa_pitch2_type, aa_pitch, ARRAYSIZE(aa_pitch));
				ImGui::Text("Real");
				ImGui::Combo(("real##jumping"), &SETTINGS::settings.aa_real2_type, aa_mode, ARRAYSIZE(aa_mode));
				//ImGui::Combo(("fake##jumping"), &SETTINGS::settings.aa_fake2_type, aa_fake, ARRAYSIZE(aa_fake));
				break;
			}

			case 3: {
				ImGui::Text("Pitch");
				ImGui::Combo(("##fakewalking"), &SETTINGS::settings.aa_pitch3_type, aa_pitch, ARRAYSIZE(aa_pitch));
				ImGui::Text("Real");
				ImGui::Combo(("##fakewalking"), &SETTINGS::settings.aa_real3_type, aa_mode, ARRAYSIZE(aa_mode));

			}
			}

			ImGui::Spacing();

			ImGui::Checkbox(("Resolver"), &SETTINGS::settings.resolve_bool);

		} ImGui::EndChild();


		ImGui::SameLine();

		ImGui::BeginChild("#antiaimbuilder", ImVec2(childX, 0), true); {
			//dingma, dingma wha? dingma ma car door
			//									-calvin
			ImGui::Checkbox(("Hentai-Aim Arrows"), &SETTINGS::settings.rifk_arrow);
			ImGui::Checkbox(("Desync Yaw"), &SETTINGS::settings.desync_yaw);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//ImGui::Combo(("Flip Key"), &SETTINGS::settings.flip_int, key_binds, ARRAYSIZE(key_binds));
			switch (SETTINGS::settings.aa_mode) {
				//"Standing", "Moving", "Jumping" ,"Fakewalking"
			case 0: {
				if (SETTINGS::settings.aa_real_type < 3) {
					ImGui::SliderInt("Standing Real Additive", &SETTINGS::settings.aa_realadditive_val, -180, 180);
					ImGui::SliderInt("Standing LBY Delta", &SETTINGS::settings.delta_val, -119, 119);
				}
				else if (SETTINGS::settings.aa_real_type ==  3) {
					ImGui::SliderInt("Standing Freestanding LBY Delta", &SETTINGS::settings.delta_val, -119, 119);
					ImGui::SliderInt("Standing Freestanding Spin Range", &SETTINGS::settings.spinangle, 0.f, 360.f);
					ImGui::SliderInt("Standing Spins Per Tick", &SETTINGS::settings.spinspeed, 0.f, 60.f);
				}
				if (SETTINGS::settings.aa_real_type == 1) {
					ImGui::Checkbox("Standing back Jiiter", &SETTINGS::settings.backjitter);
				}
				if (SETTINGS::settings.aa_real_type == 2)
				{
					ImGui::Text("Manual Flip Key");
					ImGui::Combo("##manual_flip_key", &SETTINGS::settings.flip_int, key_binds, ARRAYSIZE(key_binds));
				}
				break;
			}

			case 1: {
				if (SETTINGS::settings.aa_real1_type < 3) {
					ImGui::SliderInt("Moving Real Additive", &SETTINGS::settings.aa_realadditive1_val, -180, 180);
					ImGui::SliderInt("Moving LBY Delta", &SETTINGS::settings.delta1_val, -119, 119);
				}
				else if (SETTINGS::settings.aa_real1_type == 3) {
					ImGui::SliderInt("Moving Freestanding LBY Delta", &SETTINGS::settings.delta1_val, -119, 119);
					ImGui::SliderInt("Moving Freestanding Spin Range", &SETTINGS::settings.spinangle1, 0.f, 360.f);
					ImGui::SliderInt("Moving Spins Per Tick", &SETTINGS::settings.spinspeed1, 0.f, 60.f);
				}
				if (SETTINGS::settings.aa_real1_type == 1) {
					ImGui::Checkbox("Moving back Jiiter", &SETTINGS::settings.backjitter1);
				}
				if (SETTINGS::settings.aa_real1_type == 2)
				{
					ImGui::Text("Manual Flip Key");
					ImGui::Combo("##manual_flip_key", &SETTINGS::settings.flip_int, key_binds, ARRAYSIZE(key_binds));
				}
				break;
			}

			case 2: {
				if (SETTINGS::settings.aa_real2_type < 3) {
					ImGui::SliderInt("Jumping Real Additive", &SETTINGS::settings.aa_realadditive2_val, -180, 180);
					ImGui::SliderInt("Jumping LBY Delta", &SETTINGS::settings.delta2_val, -119, 119);
				}
				else if (SETTINGS::settings.aa_real2_type == 3) {
					ImGui::SliderInt("Jumping Freestanding LBY Delta", &SETTINGS::settings.delta2_val, -119, 119);
					ImGui::SliderInt("Jumping Freestanding Spin Range", &SETTINGS::settings.spinangle2, 0.f, 360.f);
					ImGui::SliderInt("Jumping Spins Per Tick", &SETTINGS::settings.spinspeed2, 0.f, 60.f);
				}
				if (SETTINGS::settings.aa_real2_type == 1) {
					ImGui::Checkbox("Jumping back Jiiter", &SETTINGS::settings.backjitter2);
				}
				if (SETTINGS::settings.aa_real2_type == 2)
				{
					ImGui::Text("Manual Flip Key");
					ImGui::Combo("##manual_flip_key", &SETTINGS::settings.flip_int, key_binds, ARRAYSIZE(key_binds));
				}
				break;
			}

			case 3: {
				if (SETTINGS::settings.aa_real3_type < 3) {
					ImGui::SliderInt("Fakewalking Real Additive", &SETTINGS::settings.aa_realadditive3_val, -180, 180);
					ImGui::SliderInt("Fakewalking LBY Delta", &SETTINGS::settings.delta3_val, -119, 119);
				}
				else if (SETTINGS::settings.aa_real3_type == 3) {
					ImGui::SliderInt("Fakewalking Freestanding LBY delta", &SETTINGS::settings.delta3_val, -119, 119);
					ImGui::SliderInt("Fakewalking Freestanding Spin Range", &SETTINGS::settings.spinangle3, 0.f, 360.f);
					ImGui::SliderInt("Fakewalking Spins Per Tick", &SETTINGS::settings.spinspeed3, 0.f, 60.f);
				}
				if (SETTINGS::settings.aa_real3_type == 1) {
					ImGui::Checkbox("Fakewalking back Jiiter", &SETTINGS::settings.backjitter3);
				}
				if (SETTINGS::settings.aa_real3_type == 2)
				{
					ImGui::Text("Manual Flip Key");
					ImGui::Combo("##manual_flip_key", &SETTINGS::settings.flip_int, key_binds, ARRAYSIZE(key_binds));
				}
				break;
			}
			}
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Checkbox("Fake Walk", &SETTINGS::settings.slowalk);
			ImGui::Combo("##Fake Walk", &SETTINGS::settings.slowalkkey, key_binds, ARRAYSIZE(key_binds));
			ImGui::SliderFloat("Fake Walk Speed", &SETTINGS::settings.slowwalkspeed, 1, 100);

			/*switch (SETTINGS::settings.aa_mode) {

				case 0: {
					ImGui::SliderInt("Standing Angle °", &SETTINGS::settings.spinangle, 0, 180);
					ImGui::SliderInt("Standing Speed %", &SETTINGS::settings.spinspeed, 0, 100);
					break;
				}
				case 1: {
					ImGui::SliderInt("Moving Angle °", &SETTINGS::settings.spinangle1, 0, 180);
					ImGui::SliderInt("Moving Speed %", &SETTINGS::settings.spinspeed1, 0, 100);
					break;
				}
				case 2: {
					ImGui::SliderInt("Jumping Angle °", &SETTINGS::settings.spinangle2, 0, 180);
					ImGui::SliderInt("Jumping Speed %", &SETTINGS::settings.spinspeed2, 0, 100);
					break;
				}
			}*/
		}
		ImGui::EndChild();
	}
	ImGui::PopFont();

}

void CMenu::VisualsTab() {

	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + (ImGui::GetStyle().ItemSpacing.x * 2))) / 3;


	ImGui::PushFont(font_menu);
	{

	ImGui::BeginChild("#visuals", ImVec2(childX, 0), true); {

		ImGui::Checkbox(("Enable visuals"), &SETTINGS::settings.Enabled);

		const char* arroestyle[] = { "Top", "Middle / Regular", "Bottom", "Stretched middle / Regular" };
		const char* arrowestyle[] = { "Solid", "Flashy" };

		static bool Main = true;
		static bool Colors = false;

		static int page = 0;

		ImGuiStyle& style = ImGui::GetStyle();


		style.ItemSpacing = ImVec2(1, 1);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.f));

		//TGUI_Combobox("team selection", 2, team_select, SETTINGS::settings.espteamselection);
		const char* meme = "page : error";

		switch (page) {

		case 0: meme = "";  break;
		case 1: meme = "";  break;
		default: break;
		}

		ImGui::Text(meme); ImGui::SameLine(); ImGui::Text(("	 ")); ImGui::SameLine();
		if (ImGui::Button(("Enemy"), ImVec2(45, 22))) {

			if (page != 0)
				page--;
		};

		ImGui::SameLine();

		if (ImGui::Button(("Team"), ImVec2(45, 22))) {

			if (page != 1)
				page++;
		};

		ImGui::Text(("	"));

		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();

		style.ItemSpacing = ImVec2(8, 4);

		switch (page) {

		case 0: {
			ImGui::Checkbox(("Draw enemy box"), &SETTINGS::settings.box_bool);
			ImGui::Checkbox(("Draw enemy name"), &SETTINGS::settings.name_bool);
			ImGui::Checkbox(("Draw enemy weapon"), &SETTINGS::settings.weap_bool);
			ImGui::Checkbox(("Draw enemy ammo"), &SETTINGS::settings.weap_ammo);
			ImGui::Checkbox(("Draw enemy flags"), &SETTINGS::settings.info_bool);
			ImGui::Checkbox(("Draw enemy health"), &SETTINGS::settings.health_bool);
			ImGui::Checkbox(("Draw enemy money"), &SETTINGS::settings.money_bool);
			//TGUI_Checkbox("draw enemy ammo"), &SETTINGS::settings.ammo_bool);
			ImGui::Checkbox(("Draw LBY Timer"), &SETTINGS::settings.lby_bar);
			ImGui::Checkbox(("Draw hit damage"), &SETTINGS::settings.dmg_bool);
			ImGui::SliderFloat(("Hit damage font size"), &SETTINGS::settings.fontsizedmg_bool, 0, 100);
			ImGui::Checkbox(("Draw fov arrows"), &SETTINGS::settings.fov_bool);
		}break;
		case 1:
		{
			ImGui::Checkbox(("Draw team box"), &SETTINGS::settings.boxteam);
			ImGui::Checkbox(("Draw team name"), &SETTINGS::settings.nameteam);
			ImGui::Checkbox(("Draw team weapon"), &SETTINGS::settings.weaponteam);
			ImGui::Checkbox(("Draw team flags"), &SETTINGS::settings.flagsteam);
			ImGui::Checkbox(("Draw team health"), &SETTINGS::settings.healthteam);
			ImGui::Checkbox(("Draw team money"), &SETTINGS::settings.moneyteam);
			ImGui::Checkbox(("Draw team ammo"), &SETTINGS::settings.ammoteam);
			ImGui::Checkbox(("Draw fov arrows"), &SETTINGS::settings.arrowteam);
		}break;
		}

		ImGui::Spacing();
		ImGui::Spacing();

		//TGUI_Checkbox("Lag comp hitbox", SETTINGS::settings.visual_hitbox);
		//TGUI_Slider("Lag comp hitbox time", "°", 0, 100, SETTINGS::settings.lagcomptime);
		ImGui::Checkbox(("Sound esp"), &SETTINGS::settings.bSound);
		//ImGui::SliderFloat(("slider1"), &SETTINGS::settings.slider1, 0, 100);
		//ImGui::SliderFloat(("slider2"), &SETTINGS::settings.slider2, 0, 100);
		//ImGui::SliderFloat(("slider3"), &SETTINGS::settings.slider3, 0, 100);
		//ImGui::SliderFloat(("slider4"), &SETTINGS::settings.slider4, 0, 100);
		//ImGui::SliderFloat(("slider5"), &SETTINGS::settings.slider5, 0, 100);
		ImGui::Text("Arrow transparency");
		ImGui::Combo(("##Arrow transparency"), &SETTINGS::settings.arrowtrypes, arrowestyle, 2);
		ImGui::Text("FOV arrow style");
		ImGui::Combo(("##FOV arrow style"), &SETTINGS::settings.arroewstyle, arroestyle, 4);
		//"Top", "Middle / Regular", "Bottom"
		if (SETTINGS::settings.arroewstyle == 0)
		{
			SETTINGS::settings.arrowradius = 100.f;
			SETTINGS::settings.randoslider = 320.f;
		}
		if (SETTINGS::settings.arroewstyle == 1)
		{
			SETTINGS::settings.arrowradius = 250.f;
			SETTINGS::settings.randoslider = 0.f;
		}
		if (SETTINGS::settings.arroewstyle == 2)
		{
			SETTINGS::settings.arrowradius = 100.f;
			SETTINGS::settings.randoslider = -276.f;
		}
		if (SETTINGS::settings.arroewstyle == 3)
		{
			SETTINGS::settings.arrowradius = 200.f;
			SETTINGS::settings.randoslider = 0.f;
		}

	} ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("#visuals2", ImVec2(childX, 0), true); {


		const char* chams_color[] = { "Color", "Rainbow" };
		const char* chams_type[] = { "platinum", "glass", "crystal clear", "gold", "chrome", "plastic", "crystal blue", "flat", "material" };
		const char* glow_styles[] = { "Regular", "Pulsing", "Outline" };

		ImGui::Checkbox(("Enemy chams"), &SETTINGS::settings.chams_type);
		ImGui::Text("Enemy color type");
		ImGui::Combo("##Enemy color type", &SETTINGS::settings.chamstypecolor, chams_color, 2);
		ImGui::SliderFloat(("Enemy Rainbow speed"), &SETTINGS::settings.rainbowenemy, 0, 100);
		ImGui::Text("Enemy model type");
		ImGui::Combo("##Enemy model type", &SETTINGS::settings.chamstype, chams_type, 8);
		ImGui::Checkbox(("Backtrack chams"), &SETTINGS::settings.backtrack_chams);
		ImGui::Text("Backtrack model type");
		ImGui::Combo(("##Backtrack model type"), &SETTINGS::settings.chamstypebacktrack, chams_type, 8);

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Checkbox(("Team chams"), &SETTINGS::settings.chamsteam);
		ImGui::Text("Team chams");
		ImGui::Combo(("Team color type"), &SETTINGS::settings.chamstypeteamcolor, chams_color, 2);
		ImGui::SliderFloat(("Team Rainbow speed"), &SETTINGS::settings.rainbowteam, 0, 100);
		ImGui::Text("Team model type");
		ImGui::Combo(("##Team model type"), &SETTINGS::settings.chamstypeteam, chams_type, 8);

		ImGui::Checkbox(("Enemy Glow Enable"), &SETTINGS::settings.glowenable);
		ImGui::Checkbox(("Team Glow Enable"), &SETTINGS::settings.glowteam);
		ImGui::Text("Glow Style");
		ImGui::Combo(("##glow style"), &SETTINGS::settings.glowstyle, glow_styles, ARRAYSIZE(glow_styles));

		ImGui::Checkbox(("Local Glow"), &SETTINGS::settings.glowlocal);
		ImGui::Text("Local Glow Style");
		ImGui::Combo(("##local glow style"), &SETTINGS::settings.glowstylelocal, glow_styles, ARRAYSIZE(glow_styles));

	} ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("#visuals3", ImVec2(childX, 0), true); {

		const char* crosshair_select[] = { "None", "Static", "Recoil" };
		const char* spreadshit[] = { "Off", "Rainbow", "Another" };
		const char* scope_lines[] = { "Rainbow Lines", "Black Lines" };

		ImGui::Checkbox(("No Smoke"), &SETTINGS::settings.smoke_bool);
		ImGui::Checkbox(("No Flash"), &SETTINGS::settings.NoFlash);

		ImGui::SliderFloat("Modulate", &SETTINGS::settings.daytimevalue, 0, 100);
		ImGui::SliderFloat("Prop Transparency", &SETTINGS::settings.asus_props_value, 0, 100);

		//ImGui::Checkbox("Night Mode", &SETTINGS::settings.night_bool);
		ImGui::Checkbox(("Bullet Tracers"), &SETTINGS::settings.beam_bool);
		ImGui::SliderFloat("Bullet life", &SETTINGS::settings.bulletlife, 0, 30);
		ImGui::SliderFloat("Bullet size", &SETTINGS::settings.bulletsize, 0, 20);
		ImGui::Checkbox(("Bullet Impacts"), &SETTINGS::settings.impacts);

		ImGui::Checkbox(("Thirdperson"), &SETTINGS::settings.tp_bool);
		ImGui::Text("Thirdperson Key");
		ImGui::Combo(("##thirdperson_key"), &SETTINGS::settings.thirdperson_int, key_binds, ARRAYSIZE(key_binds));
		ImGui::SliderFloat(("Thirdperson Transparency"), &SETTINGS::settings.transparency_amnt, 0, 1);
		ImGui::SliderFloat("Thirdperson Fov", &SETTINGS::settings.tpfov_val, 0, 179);
		//ImGui::Text("Crosshair");
		//ImGui::Combo(("##crosshair"), &SETTINGS::settings.xhair_type, crosshair_select, ARRAYSIZE(crosshair_select));
		ImGui::Checkbox(("Spread Circle"), &SETTINGS::settings.spread_bool);

		ImGui::Checkbox(("Remove Scope"), &SETTINGS::settings.scope_bool);
		ImGui::Text("Remove Scope");
		ImGui::Combo(("##Remove Scope"), &SETTINGS::settings.scope_type, scope_lines, ARRAYSIZE(scope_lines));
		ImGui::Checkbox(("Remove Zoom"), &SETTINGS::settings.removescoping);
		ImGui::Checkbox(("Fix Zoom Sensitivity"), &SETTINGS::settings.fixscopesens);
		
		ImGui::Checkbox(("Enable Postprocessing"), &SETTINGS::settings.matpostprocessenable);
		//ImGui::Hotkey(("flashlight key"), &SETTINGS::settings.flashlightkey);
		ImGui::SliderFloat("Render Fov", &SETTINGS::settings.fov_val, 0, 179);
		ImGui::SliderFloat("Viewmodel Fov", &SETTINGS::settings.viewfov_val, 0, 179);

		ImGui::Checkbox(("LBY Indicator"), &SETTINGS::settings.lbyenable);
		ImGui::Checkbox(("Screen Hitmarker"), &SETTINGS::settings.hitmarkerscreen);
		ImGui::Checkbox(("Player Hitmarker"), &SETTINGS::settings.playerhitmarker);

		ImGui::Checkbox(("Force crosshiar"), &SETTINGS::settings.forcehair); ImGui::BeginTooltip;
	} ImGui::EndChild();
	}
	ImGui::PopFont();
}

void CMenu::MiscTab() {
	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + ImGui::GetStyle().ItemSpacing.x)) / 2;

	ImGui::PushFont(font_menu);
	{

	ImGui::BeginChild("##misc", ImVec2(childX, 0), true); {
		const char* hitmarker[] = { "None", "Nnware", "Your mad", "Skeet", "Aimware", "Rifk 7 beep", "BameWare", "Nnware v2", "Bell", "deathnotice.wav", "Anime moan", "Anime moan2", ";)" };

		ImGui::Checkbox(("Auto Bunnyhop"), &SETTINGS::settings.bhop_bool);
		ImGui::Checkbox(("Auto Strafer"), &SETTINGS::settings.strafe_bool);
		ImGui::Checkbox(("Duck in Air"), &SETTINGS::settings.duck_bool);
		ImGui::Checkbox(("No duck delay"), &SETTINGS::settings.nocoruchdelay);
		//ImGui::Spacing();
		ImGui::Checkbox(("Animated Clantag"), &SETTINGS::settings.misc_clantag);
		//ImGui::Spacing();
		ImGui::Combo(("Hit sound"), &SETTINGS::settings.hitsound, hitmarker, ARRAYSIZE(hitmarker));
		ImGui::Combo(("Kill sound"), &SETTINGS::settings.hitsound2, hitmarker, ARRAYSIZE(hitmarker));
		//ImGui::Spacing();
		ImGui::Checkbox(("Full bright"), &SETTINGS::settings.full_bright); ImGui::BeginTooltip;
		//ImGui::Spacing();
		ImGui::Checkbox(("Preserve killfeed"), &SETTINGS::settings.killfeed);
		//ImGui::Spacing();
			
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Checkbox(("Viewmodel changer"), &SETTINGS::settings.Viewmodelchanger);
		ImGui::SliderInt(("Viewmodel x"), &SETTINGS::settings.viewmodel_x, -10, 20);
		ImGui::SliderInt(("Viewmodel y"), &SETTINGS::settings.viewmodel_y, -10, 20);
		ImGui::SliderInt(("Viewmodel z"), &SETTINGS::settings.viewmodel_z, -10, 30);
		//ImGui::InputText("##clantag_text", clantag_name, 64);
		//if (ImGui::Button("force update clantag"))
		//	should_reset = true;
		//	else
		//	should_reset = false;

		//ImGui::Hotkey(("circle strafer"), &SETTINGS::settings.circlestrafekey, key_binds, ARRAYSIZE(key_binds));
	} ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("##misc2", ImVec2(childX, 0), true); {

		const char* fakelag_mode[] = { "Factor", "Adaptive" };

		ImGui::Checkbox(("Menu Tabs on Top"), &tabsOnTop);
		if (ImGui::Button("Disbale CVAR features (anti-untrusted)"))
		{
			SETTINGS::settings.cvarsoff = true;
			SETTINGS::settings.impacts = false;
			SETTINGS::settings.full_bright = false;
			SETTINGS::settings.forcehair = false;
			SETTINGS::settings.matpostprocessenable = false;
			SETTINGS::settings.fixscopesens = false;
		}
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Checkbox(("Enable Fakelag"), &SETTINGS::settings.lag_bool);
		ImGui::Text("Fakelag Type");
		ImGui::Combo(("##Fakelag type"), &SETTINGS::settings.lag_type, fakelag_mode, ARRAYSIZE(fakelag_mode));
		ImGui::SliderInt("Standing Lag", &SETTINGS::settings.stand_lag, 1, 14);
		ImGui::SliderInt("Moving Lag", &SETTINGS::settings.move_lag, 1, 14);
		ImGui::SliderInt("Jumping Lag", &SETTINGS::settings.jump_lag, 1, 14);
		//ImGui::SliderInt("Fakewalk Lag", &SETTINGS::settings.fakewalk_lag, 1, 14);

		//ImGui::Spacing();
		//ImGui::Spacing();


	} ImGui::EndChild();

	}
	ImGui::PopFont();
}

void CMenu::SkinsTab() {

	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + ImGui::GetStyle().ItemSpacing.x)) / 2;


	ImGui::PushFont(font_menu);
	{

		ImGui::BeginChild(("#skinchanger"), ImVec2(childX, 0), true); {

			ImGui::Text("DO NOT CLICK FORCE UPDATE WITHOUT \nENABLING SKIN CHANGER!", CColor(255, 255, 255));

			ImGui::Checkbox("Skin changer", &SETTINGS::settings.skinenabled);
			ImGui::Checkbox("Glove changer", &SETTINGS::settings.glovesenabled);
			ImGui::Checkbox("Model changer", &SETTINGS::settings.CUSTOMMODEL);

			ImGui::Text("Enemy");
			ImGui::Combo(("##Enemy"), &SETTINGS::settings.customct, playermodels, ARRAYSIZE(playermodels));
			ImGui::Text("Teamate");
			ImGui::Combo(("##Teamate"), &SETTINGS::settings.customtt, playermodels, ARRAYSIZE(playermodels));
			ImGui::Text("Ak47 model");
			ImGui::Combo(("##ak47 model"), &SETTINGS::settings.m4a4model, m4a4model, ARRAYSIZE(m4a4model));
			ImGui::Text("M4a1-s model");
			ImGui::Combo(("##M4a1-s model"), &SETTINGS::settings.m4a4smodel, m4a1smodel, ARRAYSIZE(m4a4model));
			//ImGui::Combo(("Local"), &SETTINGS::settings.customlocal, playermodels, ARRAYSIZE(playermodels));
			

			ImGui::PushItemWidth(150.0f);
			ImGui::Text("Knife Model");
			ImGui::Combo(("##knife model"), &SETTINGS::settings.Knife, KnifeModel, ARRAYSIZE(KnifeModel));
			if (SETTINGS::settings.Knife == 14) {
				ImGui::PushItemWidth(150.0f);
			}
			else {
				ImGui::PushItemWidth(150.0f);
				ImGui::Text("Knife Skin");
				ImGui::Combo(("##knife skin"), &SETTINGS::settings.KnifeSkin, knifeskins, ARRAYSIZE(knifeskins));
				ImGui::PushItemWidth(150.0f);
			}
			ImGui::Text("Glove Model");
			ImGui::Combo(("##glove model"), &SETTINGS::settings.gloves, GloveModel, ARRAYSIZE(GloveModel));
			ImGui::PushItemWidth(150.0f);

			switch (SETTINGS::settings.gloves) {

			case 0: {
				const char* glovesskins[] = { "" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &SETTINGS::settings.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 1: {
				const char* glovesskins[] = { "charred", "snakebite", "bronzed", "guerilla" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &SETTINGS::settings.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 2: {
				const char* glovesskins[] = { "hedge maze", "pandoras box", "superconductor", "arid", "omega", "amphibious", "bronze morph" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &SETTINGS::settings.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 3: {
				const char* glovesskins[] = { "lunar weave", "convoy", "crimson weave", "diamondback", "overtake", "racing green", "king snake", "imperial plaid" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &SETTINGS::settings.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 4: {
				const char* glovesskins[] = { "leather", "spruce ddpat", "slaughter", "cobalt skulls", "overprint", "duct tape", "arboreal" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &SETTINGS::settings.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 5: {
				const char* glovesskins[] = { "eclipse", "spearmint", "boom", "cool mint", "turtle", "transport", "polygon", "pow" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &SETTINGS::settings.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 6: {
				const char* glovesskins[] = { "forest ddpat", "crimson kimono", "emerald web", "foundation", "crimson web", "buckshot", "fade", "mogul" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &SETTINGS::settings.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 7: {
				const char* glovesskins[] = { "emerald", "mangrove", "rattler", "case hardened" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &SETTINGS::settings.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			}

			ImGui::Spacing();
			ImGui::Spacing();

			if (ImGui::Button(("Force update")))
				KnifeApplyCallbk();



		} ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild(("#skinchanger_pages"), ImVec2(childX, 0), true, true ? ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar : ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders); {

			static bool Main = true;
			static bool Colors = false;

			static int page = 0;

			ImGuiStyle& style = ImGui::GetStyle();


			style.ItemSpacing = ImVec2(1, 1);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.f));

			const char* meme = "page : error";

			switch (page) {

			case 0: meme = "page : 1";  break;
			case 1: meme = "page : 2";  break;
			case 2: meme = "HvH weapons";  break;
			default: break;
			}

			ImGui::Text(meme); ImGui::SameLine(); ImGui::Text(("                  ")); ImGui::SameLine();
			if (ImGui::Button(("-"), ImVec2(42, 22))) {

				if (page != 0)
					page--;
			};

			ImGui::SameLine();

			if (ImGui::Button(("+"), ImVec2(42, 22))) {

				if (page != 2)
					page++;
			};

			ImGui::Text(("        "));

			ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();

			style.ItemSpacing = ImVec2(8, 4);

			switch (page) {

			case 0: {
				ImGui::Combo(("AK-47"), &SETTINGS::settings.AK47Skin, ak47, ARRAYSIZE(ak47));
				ImGui::Combo(("M4A1-S"), &SETTINGS::settings.M4A1SSkin, m4a1s, ARRAYSIZE(m4a1s));
				ImGui::Combo(("M4A4"), &SETTINGS::settings.M4A4Skin, m4a4, ARRAYSIZE(m4a4));
				ImGui::Combo(("Galil AR"), &SETTINGS::settings.GalilSkin, galil, ARRAYSIZE(galil));
				ImGui::Combo(("AUG"), &SETTINGS::settings.AUGSkin, aug, ARRAYSIZE(aug));
				ImGui::Combo(("FAMAS"), &SETTINGS::settings.FAMASSkin, famas, ARRAYSIZE(famas));
				ImGui::Combo(("SG-553"), &SETTINGS::settings.Sg553Skin, sg553, ARRAYSIZE(sg553));
				ImGui::Combo(("UMP45"), &SETTINGS::settings.UMP45Skin, ump45, ARRAYSIZE(ump45));
				ImGui::Combo(("MAC-10"), &SETTINGS::settings.Mac10Skin, mac10, ARRAYSIZE(mac10));
				ImGui::Combo(("PP-Bizon"), &SETTINGS::settings.BizonSkin, bizon, ARRAYSIZE(bizon));
				ImGui::Combo(("TEC-9"), &SETTINGS::settings.tec9Skin, tec9, ARRAYSIZE(tec9));
				ImGui::Combo(("P2000"), &SETTINGS::settings.P2000Skin, p2000, ARRAYSIZE(p2000));
				ImGui::Combo(("P250"), &SETTINGS::settings.P250Skin, p250, ARRAYSIZE(p250));
				ImGui::Combo(("Dual-Barettas"), &SETTINGS::settings.DualSkin, dual, ARRAYSIZE(dual));
				ImGui::Combo(("CZ75-Auto"), &SETTINGS::settings.Cz75Skin, cz75, ARRAYSIZE(cz75));
				ImGui::Combo(("SCAR20"), &SETTINGS::settings.SCAR20Skin, scar20, ARRAYSIZE(scar20));

			} break;
			case 1:
			{
				ImGui::Combo(("Nova"), &SETTINGS::settings.NovaSkin, nova, ARRAYSIZE(nova));
				ImGui::Combo(("XM1014"), &SETTINGS::settings.XmSkin, xm, ARRAYSIZE(xm));
				ImGui::Combo(("USP-S"), &SETTINGS::settings.USPSkin, usp, ARRAYSIZE(usp));
				ImGui::Combo(("Desert Eagle"), &SETTINGS::settings.DeagleSkin, deagle, ARRAYSIZE(deagle));
				ImGui::Combo(("Five-Seven"), &SETTINGS::settings.FiveSkin, five, ARRAYSIZE(five));
				ImGui::Combo(("AWP"), &SETTINGS::settings.AWPSkin, awp, ARRAYSIZE(awp));
				ImGui::Combo(("SSG08"), &SETTINGS::settings.SSG08Skin, ssg08, ARRAYSIZE(ssg08));
				ImGui::Combo(("G3SG1"), &SETTINGS::settings.G3sg1Skin, g3sg1, ARRAYSIZE(g3sg1));
				ImGui::Combo(("MP9"), &SETTINGS::settings.Mp9Skin, mp9, ARRAYSIZE(mp9));
				ImGui::Combo(("Glock-18"), &SETTINGS::settings.GlockSkin, glock, ARRAYSIZE(glock));
				ImGui::Combo(("Revolver"), &SETTINGS::settings.RevolverSkin, revolver, ARRAYSIZE(revolver));
				ImGui::Combo(("Negev"), &SETTINGS::settings.NegevSkin, negev, ARRAYSIZE(negev));
				ImGui::Combo(("M249"), &SETTINGS::settings.M249Skin, m249, ARRAYSIZE(m249));
				ImGui::Combo(("SAWED-Off"), &SETTINGS::settings.SawedSkin, sawed, ARRAYSIZE(sawed));
				ImGui::Combo(("Mag-7"), &SETTINGS::settings.MagSkin, mag, ARRAYSIZE(mag));
			} break;
			case 2:
			{
				ImGui::Combo(("SCAR20"), &SETTINGS::settings.SCAR20Skin, scar20, ARRAYSIZE(scar20));
				ImGui::Combo(("AWP"), &SETTINGS::settings.AWPSkin, awp, ARRAYSIZE(awp));
				ImGui::Combo(("SSG08"), &SETTINGS::settings.SSG08Skin, ssg08, ARRAYSIZE(ssg08));
				ImGui::Combo(("G3SG1"), &SETTINGS::settings.G3sg1Skin, g3sg1, ARRAYSIZE(g3sg1));
				ImGui::Combo(("MP9"), &SETTINGS::settings.Mp9Skin, mp9, ARRAYSIZE(mp9));
				ImGui::Combo(("Glock-18"), &SETTINGS::settings.GlockSkin, glock, ARRAYSIZE(glock));
				ImGui::Combo(("Revolver"), &SETTINGS::settings.RevolverSkin, revolver, ARRAYSIZE(revolver));
				ImGui::Combo(("Dual-Barettas"), &SETTINGS::settings.DualSkin, dual, ARRAYSIZE(dual));
			} break;
			default:break;
			}
		} ImGui::EndChild();
	}
	ImGui::PopFont();
}

void CMenu::ConfigTab() {

	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + ImGui::GetStyle().ItemSpacing.x)) / 2;


	ImGui::PushFont(font_menu);
	{

		ImGui::BeginChild("qwerq23rew", ImVec2(400, 0), true);
		{
			static int sel;
			std::string config;
			std::vector<std::string> configs = SETTINGS::settings.GetConfigs();
			if (configs.size() > 0) {
				ImGui::ListBoxConfigArray("Configs", &sel, configs);
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();
				ImGui::PushItemWidth(220.f);
				config = configs[SETTINGS::settings.config_sel];
			}
			SETTINGS::settings.config_sel = sel;

			ImGui::InputText("##CFG", ConfigNamexd, 64);

			if (ImGui::Button("Create", ImVec2(263, 0)))
			{
				std::string ConfigFileName = ConfigNamexd;
				if (ConfigFileName.size() < 1)
				{
					ConfigFileName = "settings";
				}
				SETTINGS::settings.CreateConfig(ConfigFileName);
				global::Msg("Configuration created.    \n");
			}
			if (config.size() >= 1) {
				if (ImGui::Button("Delete", ImVec2(263, 0)))
				{
					SETTINGS::settings.Remove(config);
					global::Msg("Configuration removed.    \n");
				}
			}

			ImGui::Spacing();

			if (configs.size() >= 1) {
				if (ImGui::Button("Save", ImVec2(263, 0)))
				{
					SETTINGS::settings.Save(config);
					global::Msg("Configuration saved.    \n");
				}
			}

			if (configs.size() > 0) {
				if (ImGui::Button("Load", ImVec2(263, 0)))
				{

					SETTINGS::settings.Load(config);
					global::Msg("Configuration loaded.    \n");
				}
			}

			//ImGui::SameLine();
			//if (ImGui::Button("Refresh configs", ImVec2(220, 0)))
			//{
			//	SETTINGS::settings.RefreshConfigs();
				//global::Msg("Configuration created.    \n");
			//}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("#colors", ImVec2(childX, 0), true); {
			//ImGuiColorEditFlags_Float
			ImGui::Text(("Players"));
			ImGui::ColorEdit4(("Enemy box color"), SETTINGS::settings.box_col, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Enemy name color"), SETTINGS::settings.name_col, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Enemy weapon color"), SETTINGS::settings.weapon_col, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Enemy fov arrows color"), SETTINGS::settings.fov_col, ImGuiColorEditFlags__OptionsDefault);

			ImGui::ColorEdit4(("Team box color"), SETTINGS::settings.boxteam_col, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Team name color"), SETTINGS::settings.nameteam_col, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Team weapon color"), SETTINGS::settings.weaponteam_col, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Team fov arrows color"), SETTINGS::settings.arrowteam_col, ImGuiColorEditFlags__OptionsDefault);
			//ImGui::ColorEdit4(("ct Visible##box"), SETTINGS::settings.BoxColorPickCTVIS, ImGuiColorEditFlags__theshits);
			//ImGui::ColorEdit4(("ct invisible##box"), SETTINGS::settings.BoxColorPickCTINVIS, ImGuiColorEditFlags__theshits);
			//ImGui::ColorEdit4(("t Visible##box"), SETTINGS::settings.BoxColorPickTVIS, ImGuiColorEditFlags__theshits);
			//ImGui::ColorEdit4(("t invisible##box"), SETTINGS::settings.BoxColorPickTINVIS, ImGuiColorEditFlags__theshits);

			ImGui::Text(("chams"));
			//ImGui::ColorEdit4(("enemy visible##chams"), SETTINGS::settings.vmodel_col, ImGuiColorEditFlags__theshits);
			ImGui::ColorEdit4(("Enemy invisible##chams"), SETTINGS::settings.imodel_col, ImGuiColorEditFlags__OptionsDefault);
			//ImGui::ColorEdit4(("team visible##chams"), SETTINGS::settings.teamvis_color, ImGuiColorEditFlags__theshits);
			ImGui::ColorEdit4(("Team invisible##chams"), SETTINGS::settings.teaminvis_color, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Local##chams"), SETTINGS::settings.localchams_col, ImGuiColorEditFlags__OptionsDefault);

			ImGui::Text(("Glow"));
			ImGui::ColorEdit4(("Enemy##glow"), SETTINGS::settings.glow_col, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Team##glow"), SETTINGS::settings.teamglow_color, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Local##glow"), SETTINGS::settings.glowlocal_col, ImGuiColorEditFlags__OptionsDefault);


			ImGui::Text(("Bullet tracers"));
			ImGui::ColorEdit4(("Local##tracer"), SETTINGS::settings.bulletlocal_col, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Enemy##tracer"), SETTINGS::settings.bulletenemy_col, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Team##tracer"), SETTINGS::settings.bulletteam_col, ImGuiColorEditFlags__OptionsDefault);

			ImGui::Text(("Other"));
			ImGui::ColorEdit4(("Sound esp color##SoundEspColor"), SETTINGS::settings.cSound, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Damage esp##DamageEspColor"), SETTINGS::settings.dmg_ccolor, ImGuiColorEditFlags__OptionsDefault);
			ImGui::ColorEdit4(("Grenade prediction##xd"), SETTINGS::settings.grenadepredline_col);
			//ImGui::ColorEdit4(("spread circle##xd"), SETTINGS::settings.spreadcirclecol, ImGuiColorEditFlags__theshits);
			//ImGui::ColorEdit4(("fake chams##xd"), SETTINGS::settings.fakechamscol); // ImGuiColorEditFlags__DataTypeMask | ImGuiSelectableFlags_Disabled | ImGuiColorEditFlags_RGB ImGuiColorEditFlags_NoInputs
		} ImGui::EndChild();

	}
	ImGui::PopFont();

}

void CMenu::radioTab() {
	/*TabStyle();
	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + ImGui::GetStyle().ItemSpacing.x)) / 2;

	ImGui::BeginChild("#radio", ImVec2(childX, 0), true); {

		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::Spacing();
		//std::string songname;
		//if (ImGui::Button("GetSong")) {
		//	std::string text = "Current track: ";
		//	std::string song = GetHardstyleSong().c_str();
		//	std::string copied = ", copied to clipboard.";

		//	g_Logger.WriteToLog((text + song + copied).c_str());
		//}
		static bool radioInit = false;
		static int govno = 0;
		if (govno != SETTINGS::settings.radio_selected)
			radioInit = false;
		if (!radioInit) {
			BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);
			stream = BASS_StreamCreateURL(stations[SETTINGS::settings.radio_selected].c_str(), 0, 0, NULL, 0);
			radioInit = true;
			SETTINGS::settings.radio_paused = false;
			govno = SETTINGS::settings.radio_selected;
		}

		static float vol = 0.1f;
		BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, vol);

		if (!SETTINGS::settings.radio_paused)
		{
			BASS_ChannelPlay(stream, false);
		}

		ImGui::PushItemWidth(300);

		if (ImGui::Combo(XorStr("##stancs"), &SETTINGS::settings.radio_selected, radio_name, IM_ARRAYSIZE(radio_name)))
		{
			BASS_ChannelStop(stream);
			stream = NULL;
			stream = BASS_StreamCreateURL(stations[SETTINGS::settings.radio_selected].c_str(), 0, 0, NULL, 0);
		}

		//ImGui::PushFont(awesomefont);

		if (ImGui::Button(XorStr("<<"), ImVec2(76, 32)))
		{
			if (SETTINGS::settings.radio_selected == 0)
				SETTINGS::settings.radio_selected = ARRAYSIZE(stations);

			SETTINGS::settings.radio_selected--;
			BASS_ChannelPause(stream);
			stream = NULL;
			stream = BASS_StreamCreateURL(stations[SETTINGS::settings.radio_selected].c_str(), 0, 0, NULL, 0);
		}
		ImGui::SameLine();

		char* nazvanie;

		if (!SETTINGS::settings.radio_paused)
			nazvanie = XorStr("||");
		else
			nazvanie = XorStr(">");

		if (ImGui::Button(nazvanie, ImVec2(132, 32)))
		{
			SETTINGS::settings.radio_paused = !SETTINGS::settings.radio_paused;
			BASS_ChannelPause(stream);
		}

		ImGui::SameLine();
		if (ImGui::Button(">>", ImVec2(76, 32))) {
			SETTINGS::settings.radio_selected++;
			if (SETTINGS::settings.radio_selected > ARRAYSIZE(stations) - 1)
				SETTINGS::settings.radio_selected = 0;

			BASS_ChannelPause(stream);
			stream = NULL;
			stream = BASS_StreamCreateURL(stations[SETTINGS::settings.radio_selected].c_str(), 0, 0, NULL, 0);
		}
		ImGui::Spacing();

		ImGui::SliderFloat(XorStr("##Volume"), &vol, 0.f, 1.f);

	} ImGui::EndChild();*/
}