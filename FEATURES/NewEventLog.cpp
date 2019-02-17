
#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ConVar.h"
#include "../SDK/ISurface.h"
#include "../UTILS/render.h"
#include "NewEventLog.h"
#include "../FEATURES/Resolver.h"
#include "../FEATURES/Visuals.h"
#include "../cheats/visuals/other_esp.h"
#include <playsoundapi.h>
#include <sstream> 
#include <string>
#include "../xdxd.h"
#include "../SDK/IViewRenderBeams.h"

#pragma comment(lib, "Winmm.lib")
bool hurtcalled = false;
typedef void(__cdecl* MsgFn)(const char* msg, va_list);
void ConMsg(const char* msg, ...) {

	if (msg == nullptr)
		return; //If no string was passed, or it was null then don't do anything

	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg"); //This gets the address of export "Msg" in the dll "tier0.dll". The static keyword means it's only called once and then isn't called again (but the variable is still there)
	char buffer[989];
	va_list list; //Normal varargs stuff http://stackoverflow.com/questions/10482960/varargs-to-printf-all-arguments
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	fn(buffer, list); //Calls the function, we got the address above.

	if (!INTERFACES::cvar->FindVar("developer")->GetInt())
		INTERFACES::cvar->FindVar("developer")->SetValue(true);

	if (! INTERFACES::cvar->FindVar( "con_filter_enable" )->GetInt( ) )
		INTERFACES::cvar->FindVar( "con_filter_enable" )->SetValue( 2 );
}

/* sound esp */
soundesp g_soundesp;

/* sound logs */
std::vector<sound_info> sound_logs;

enum
{
	TE_BEAMPOINTS = 0x00,		// beam effect between two points
	TE_SPRITE = 0x01,	// additive sprite, plays 1 cycle
	TE_BEAMDISK = 0x02,	// disk that expands to max radius over lifetime
	TE_BEAMCYLINDER = 0x03,		// cylinder that expands to max radius over lifetime
	TE_BEAMFOLLOW = 0x04,		// create a line of decaying beam segments until entity stops moving
	TE_BEAMRING = 0x05,		// connect a beam ring to two entities
	TE_BEAMSPLINE = 0x06,
	TE_BEAMRINGPOINT = 0x07,
	TE_BEAMLASER = 0x08,		// Fades according to viewpoint
	TE_BEAMTESLA = 0x09,
};
enum
{
	FBEAM_STARTENTITY = 0x00000001,
	FBEAM_ENDENTITY = 0x00000002,
	FBEAM_FADEIN = 0x00000004,
	FBEAM_FADEOUT = 0x00000008,
	FBEAM_SINENOISE = 0x00000010,
	FBEAM_SOLID = 0x00000020,
	FBEAM_SHADEIN = 0x00000040,
	FBEAM_SHADEOUT = 0x00000080,
	FBEAM_ONLYNOISEONCE = 0x00000100,		// Only calculate our noise once
	FBEAM_NOTILE = 0x00000200,
	FBEAM_USE_HITBOXES = 0x00000400,		// Attachment indices represent hitbox indices instead when this is set.
	FBEAM_STARTVISIBLE = 0x00000800,		// Has this client actually seen this beam's start entity yet?
	FBEAM_ENDVISIBLE = 0x00001000,		    // Has this client actually seen this beam's end entity yet?
	FBEAM_ISACTIVE = 0x00002000,
	FBEAM_FOREVER = 0x00004000,
	FBEAM_HALOBEAM = 0x00008000,		    // When drawing a beam with a halo, don't ignore the segments and endwidth
	FBEAM_REVERSED = 0x00010000,
	NUM_BEAM_FLAGS = 17	                    // KEEP THIS UPDATED!
};

void soundesp::draw_circle(CColor color, Vector position)
{
	SDK::BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMRINGPOINT;
	beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
	beamInfo.m_nModelIndex = INTERFACES::ModelInfo->GetModelIndex("sprites/purplelaser1.vmt");
	beamInfo.m_pszHaloName = "sprites/purplelaser1.vmt";
	beamInfo.m_nHaloIndex = INTERFACES::ModelInfo->GetModelIndex("sprites/purplelaser1.vmt");
	beamInfo.m_flHaloScale = 5;
	beamInfo.m_flLife = 2.50f;
	beamInfo.m_flWidth = 12.f;
	beamInfo.m_flFadeLength = 1.0f;
	beamInfo.m_flAmplitude = 0.f;
	beamInfo.m_flRed = SETTINGS::settings.cSound[0] * 255;
	beamInfo.m_flGreen = SETTINGS::settings.cSound[1] * 255;
	beamInfo.m_flBlue = SETTINGS::settings.cSound[2] * 255;
	beamInfo.m_flBrightness = 255;
	beamInfo.m_flSpeed = 1.f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 1;
	beamInfo.m_nSegments = 1;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = 0;
	beamInfo.m_vecCenter = position + Vector(0, 0, 5);
	beamInfo.m_flStartRadius = 1;
	beamInfo.m_flEndRadius = 350;

	auto beam = INTERFACES::ViewRenderBeams->CreateBeamRingPoint(beamInfo);

	if (beam) INTERFACES::ViewRenderBeams->DrawBeam(beam);
}

void soundesp::draw()
{
	/* if footstep esp is enabled */
	if (!SETTINGS::settings.bSound)
		return;

	/* if we are connected */
	if (!INTERFACES::Engine->IsInGame() || !INTERFACES::Engine->IsConnected())
		return;

	/*CUtlVector<SndInfo_t> sndList;
	sndList.RemoveAll();
	IEngineSound->GetActiveSounds(sndList);

	for (int i = 0; i < sndList.Count(); i++)
	{
		SndInfo_t sndInfo = sndList.Element(i);

		if (sndInfo.m_nSoundSource)
		{
			if (sndInfo.m_nChannel == 4)
			{
				if (sndInfo.m_bUpdatePositions)
				{
					C_BaseEntity* target = (g_pEntityList->GetClientEntity(sndList[i].m_nSoundSource));

					if (target && target->GetTeam() != g::pLocalEntity->GetTeam())
					{
						if (!sndInfo.m_pOrigin)
							continue;

						g_soundesp.draw_circle(SETTINGS::settingscSound, *sndInfo.m_pOrigin);
					}
				}
			}
		}
	}*/

	for (unsigned int i = 0; i < sound_logs.size(); i++)
	{
		/* draw the beam */
		g_soundesp.draw_circle(CColor(SETTINGS::settings.cSound[0] * 255, SETTINGS::settings.cSound[1] * 255, SETTINGS::settings.cSound[2] * 255), sound_logs[i].position);

		sound_logs.erase(sound_logs.begin() + i);
	}
}

namespace FEATURES
{
	namespace MISC
	{
		InGameLogger in_game_logger;
		void ColorLine::Draw(int x, int y, unsigned int font)
		{
			for (int i = 0; i < texts.size(); i++)
			{
				RENDER::DrawF(x, y, font, false, false, colors[i], texts[i]);
				x += RENDER::GetTextSize(font, texts[i]).x;
			}
		}

		void InGameLogger::Do()
		{
			if (log_queue.size() > max_lines_at_once)
				log_queue.erase(log_queue.begin() + max_lines_at_once, log_queue.end());

			for (int i = 0; i < log_queue.size(); i++)
			{
				auto log = log_queue[i];
				float time_delta = fabs(UTILS::GetCurtime() - log.time);

				int height = ideal_height + (16 * i);

				/// erase dead logs
				if (time_delta > text_time)
				{
					log_queue.erase(log_queue.begin() + i);
					break;
				}
				if (time_delta > text_time - slide_out_speed)
					height = height + (((slide_out_speed - (text_time - time_delta)) / slide_out_speed) * slide_out_distance);

				/// fade out
				if (time_delta > text_time - text_fade_out_time)
					log.color_line.ChangeAlpha(255 - (((time_delta - (text_time - text_fade_out_time)) / text_fade_out_time) * 255.f));
				/// fade in
				if (time_delta < text_fade_in_time)
					log.color_line.ChangeAlpha((time_delta / text_fade_in_time) * 255.f);

				int width = ideal_width;

				/// slide from left
				if (time_delta < text_fade_in_time)
					width = (time_delta / text_fade_in_time) * static_cast<float>(slide_in_distance) + (ideal_width - slide_in_distance);
				/// slider from right
				if (time_delta > text_time - text_fade_out_time)
					width = ideal_width + (((time_delta - (text_time - text_fade_out_time)) / text_fade_out_time) * static_cast<float>(slide_out_distance));

				
				log.color_line.Draw(width, height, FONTS::in_game_logging_font);
				
			}
		}
	}
}

bool did_hit = false;

namespace FEATURES
{
	namespace MISC
	{

		CREATE_EVENT_LISTENER(ItemPurchaseListener, "item_purchase");

		CREATE_EVENT_LISTENER(PlayerHurtListener, "player_hurt");

		CREATE_EVENT_LISTENER(BulletImpactListener, "bullet_impact");

		CREATE_EVENT_LISTENER(WeaponFireListener, "weapon_fire");

		CREATE_EVENT_LISTENER(RoundStartListener, "round_start");

		CREATE_EVENT_LISTENER(RoundPreStartListener, "round_prestart");

		CREATE_EVENT_LISTENER(PlayerDeathListener, "player_death");

		CREATE_EVENT_LISTENER(RoundEndListener, "round_end");

		CREATE_EVENT_LISTENER(Player_footstep_event, "player_footstep");

		CREATE_EVENT_LISTENER(Player_hurt_event, "player_hurt");

		ItemPurchaseListener item_purchase_listener;
		PlayerHurtListener player_hurt_listener;
		BulletImpactListener bullet_impact_listener;
		WeaponFireListener weapon_fire_listener;
		RoundStartListener round_start_listener;
		RoundPreStartListener round_prestart_listener;
		PlayerDeathListener player_death_listener;
		RoundEndListener round_end_listener;
		Player_footstep_event player_footstep_listener;
		Player_hurt_event player_hurt_listener2;

		void InitializeEventListeners()
		{

			INTERFACES::GameEventManager->AddListener(&item_purchase_listener, ("item_purchase"), false);
			INTERFACES::GameEventManager->AddListener(&player_hurt_listener, ("player_hurt"), false);
			INTERFACES::GameEventManager->AddListener(&bullet_impact_listener, ("bullet_impact"), false);
			INTERFACES::GameEventManager->AddListener(&weapon_fire_listener, ("weapon_fire"), false);
			INTERFACES::GameEventManager->AddListener(&round_start_listener, ("round_start"), false);
			INTERFACES::GameEventManager->AddListener(&player_death_listener, ("player_death"), false);
			INTERFACES::GameEventManager->AddListener(&round_prestart_listener, ("round_prestart"), false);
			INTERFACES::GameEventManager->AddListener(&round_end_listener, ("round_end"), false);
			INTERFACES::GameEventManager->AddListener(&player_footstep_listener, "player_footstep", false);
			INTERFACES::GameEventManager->AddListener(&player_hurt_listener2, "player_hurt", false);

		}

		void RemoveEventListeners()
		{
			INTERFACES::GameEventManager->RemoveListener(&item_purchase_listener);
			INTERFACES::GameEventManager->RemoveListener(&player_hurt_listener);
			INTERFACES::GameEventManager->RemoveListener(&bullet_impact_listener);
			INTERFACES::GameEventManager->RemoveListener(&weapon_fire_listener);
			INTERFACES::GameEventManager->RemoveListener(&round_start_listener);
			INTERFACES::GameEventManager->RemoveListener(&round_start_listener);
			INTERFACES::GameEventManager->RemoveListener(&round_end_listener);
			INTERFACES::GameEventManager->RemoveListener(&player_footstep_listener);
			INTERFACES::GameEventManager->RemoveListener(&player_hurt_listener2);
		}

		void ItemPurchaseListener::FireGameEvent(SDK::IGameEvent* game_event)
		{
			if (!game_event)
				return;

			SDK::CBaseEntity* local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			SDK::CBaseEntity* entity = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid")));
			if (!entity)
				return;

			if (entity->GetTeam() == local_player->GetTeam())
				return;

			SDK::player_info_t player_info;
			if (!INTERFACES::Engine->GetPlayerInfo(entity->GetIndex(), &player_info))
				return;

			MISC::InGameLogger::Log log;

			std::string name = player_info.szName, weapon_name = game_event->GetString("weapon");
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);
			std::transform(weapon_name.begin(), weapon_name.end(), weapon_name.begin(), ::toupper);

			log.color_line.PushBack(name, CColor(255, 255, 255,255));
			log.color_line.PushBack(" bought ", CColor(255, 255, 255, 255));
			log.color_line.PushBack(weapon_name, CColor(255, 255, 255, 255));

			//INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
			//INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "%s bought %s \n", name.c_str(), weapon_name.c_str());

			//INTERFACES::cvar->FindVar("con_filter_text")->SetValueChar("bought");
			//ConMsg("%s bought %s", name.c_str(), weapon_name.c_str());

			MISC::in_game_logger.AddLog(log);
		};

		void RoundPreStartListener::FireGameEvent(SDK::IGameEvent* event) {

			if (!event)
				return;

			auto local_player = static_cast<SDK::CBaseEntity*>(INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer())); if (!local_player) return;

			global::DisableAA = true;
		}

		/*auto is_enemy_team_dead() -> bool {

			auto local_player = static_cast<SDK::CBaseEntity*>(INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer()));

			
		}*/

		void RoundEndListener::FireGameEvent(SDK::IGameEvent* event) {

			if (!event)
				return;

			auto local_player = static_cast<SDK::CBaseEntity*>(INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer())); if (!local_player) return;

			/*if (is_enemy_team_dead)
				global::DisableAA = true;
			else
				global::DisableAA = false;*/

			global::DisableAA = false;

			global::m_round_changed = true;

			//global::killcount = 0;
		}

		void Player_hurt_event::FireGameEvent(SDK::IGameEvent* game_event)
		{
			/* if we are connected */
			if (!INTERFACES::Engine->IsInGame() || !INTERFACES::Engine->IsConnected())
				return;

			/* check if we have the sound esp enabled */
			if (!SETTINGS::settings.bSound)
				return;

			/* return if not event */
			if (!game_event)
				return;

			/* local player */
			auto localplayer = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

			/* get the attacker */
			auto attacker = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("attacker")));

			/* get the victim */
			auto victim = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid")));


			/* clock */
			static int timer;

			timer += 1;

			if (timer > 2)
				timer = 0;

			/* check if we are attacker */
			if (attacker == localplayer)
			{
				if (timer < 1)
					sound_logs.push_back(sound_info(victim->GetAbsOrigin(), INTERFACES::Globals->curtime, game_event->GetInt("userid")));
			}
		}

		void Player_footstep_event::FireGameEvent(SDK::IGameEvent* game_event)
		{
			/* check if we are in game */
			if (!INTERFACES::Engine->IsInGame() || !INTERFACES::Engine->IsConnected())
				return;

			/* check if we have the sound esp enabled */
			if (!SETTINGS::settings.bSound)
				return;

			/* return if not event */
			if (!game_event)
				return;

			/* local player */
			auto localplayer = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

			/* footstep */
			if (strstr(game_event->GetName(), "player_footstep"))
			{
				/* get the walker */
				auto walker = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid")));

				/* return if walker is nullptr */
				if (walker == nullptr)
					return;

				/* return if walker is dormant */
				if (walker->GetIsDormant())
					return;

				/* clock */
				static int timer;

				timer += 1;

				if (timer > 1)
					timer = 0;

				if (walker->GetTeam() != localplayer->GetTeam())
				{
					if (walker && timer < 1)
					{
						sound_logs.push_back(sound_info(walker->GetAbsOrigin(), INTERFACES::Globals->curtime, game_event->GetInt("userid")));
					}
				}
			}
		}

		void PlayerHurtListener::FireGameEvent(SDK::IGameEvent* game_event)
		{
			if (!game_event)
				return;
			
			SDK::CBaseEntity* local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;


			if (!strcmp(game_event->GetName(), "player_hurt")) {
			
				global::missed_shots[local_player->GetIndex()]++;
			}


			SDK::CBaseEntity* attacker = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("attacker")));
			if (!attacker)
				return;

			if (attacker != local_player)
				return;

			SDK::player_info_t player_info;
			INTERFACES::Engine->GetPlayerInfo(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid")), &player_info);

			int hitgroup = game_event->GetInt("hitgroup");

			MISC::InGameLogger::Log log;

			std::string name = player_info.szName;
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);

			log.color_line.PushBack("-", CColor(255, 255, 255, 255));
			log.color_line.PushBack(std::to_string(game_event->GetInt("dmg_health")), CColor(255, 255, 255, 255));
			log.color_line.PushBack(" in ", CColor(255, 255, 255, 255));
			log.color_line.PushBack(UTILS::GetHitgroupName(hitgroup), CColor(255, 255, 255, 255));
			log.color_line.PushBack(" to ", CColor(255, 255, 255, 255));
			log.color_line.PushBack(name, CColor(255, 255, 255, 255));

			glow g; g.playerMatrix[128];

			INTERFACES::cvar->FindVar("con_filter_text")->SetValueChar("hurt");
			ConMsg("hurt %s for %i in the %s \n", name.c_str(), game_event->GetInt("dmg_health"), UTILS::GetHitgroupName(hitgroup));

			visuals->set_hitmarker_time(INTERFACES::Globals->curtime);

			//cother_esp other_esp;
			//other_esp.hurt(game_event);
			//other_esp.log_tracer(game_event);

			switch (SETTINGS::settings.hitsound)
			{
			case 0:  break;
			case 1:  PlaySoundA(pew, NULL, SND_ASYNC | SND_MEMORY); break;
			case 2:  PlaySoundA(yourmad, NULL, SND_ASYNC | SND_MEMORY); break;
			case 3:  INTERFACES::Surface->IPlaySound("buttons\\arena_switch_press_02.wav"); break;
			case 4:  PlaySoundA(aimware, NULL, SND_ASYNC | SND_MEMORY); break;
			case 5:  PlaySoundA(rifk7beep, NULL, SND_ASYNC | SND_MEMORY); break;
			case 6:  PlaySoundA(bameWare, NULL, SND_ASYNC | SND_MEMORY); break;
			case 7:  PlaySoundA(odynpapa, NULL, SND_ASYNC | SND_MEMORY); break;
			case 8:  INTERFACES::Surface->IPlaySound("training\\timer_bell.wav"); break;
			case 9:  INTERFACES::Surface->IPlaySound("ui\\deathnotice.wav"); break;
			case 10: PlaySoundA(moan1, NULL, SND_ASYNC | SND_MEMORY); break;
			case 11: PlaySoundA(moan2, NULL, SND_ASYNC | SND_MEMORY); break;
			case 12: PlaySoundA(beepboop, NULL, SND_ASYNC | SND_MEMORY); break;
			//case 13: PlaySoundA(cod, NULL, SND_ASYNC | SND_MEMORY); break;
			}

			did_hit = true;

			//INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
			//INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "-%i in %s to %s \n", game_event->GetInt("dmg_health"), UTILS::GetHitgroupName(hitgroup), name.c_str());

			shots_hit[attacker->GetIndex()]++;
			MISC::in_game_logger.AddLog(log);

			SDK::CBaseEntity* hurt = (SDK::CBaseEntity*) INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid")));

			if (strcmp(game_event->GetName(), "player_hurt") == 0) {

				if (hurt != local_player && attacker == local_player) {
					CVisuals::damage_indicator_t DmgIndicator;
					DmgIndicator.dmg = game_event->GetInt("dmg_health");
					DmgIndicator.player = hurt;
					DmgIndicator.earse_time = local_player->GetTickBase() * INTERFACES::Globals->interval_per_tick + 3.f;
					DmgIndicator.initializes = false;

					visuals->dmg_indicator.push_back(DmgIndicator);
				}
			}
			if (attacker == local_player)
			{
				hurtcalled = true;
			}

		};

		void BulletImpactListener::FireGameEvent(SDK::IGameEvent* game_event)
		{

			if (!game_event)
				return;

			if (!(INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame()))
				return;

			int iUser = INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid"));
			auto entity = reinterpret_cast<SDK::CBaseEntity*>(INTERFACES::ClientEntityList->GetClientEntity(iUser));
			if (!entity) return;

			if (entity->GetIsDormant()) return;

			float x, y, z;
			x = game_event->GetFloat("x");
			y = game_event->GetFloat("y");
			z = game_event->GetFloat("z");

			UTILS::BulletImpact_t impact(entity, Vector(x, y, z), INTERFACES::Globals->curtime, iUser == INTERFACES::Engine->GetLocalPlayer() ? GREEN : RED);

			visuals->Impacts.push_back(impact);

		}

		//global::killcount

		void PlayerDeathListener::FireGameEvent(SDK::IGameEvent* event) {



			if (!event)
				return;

			SDK::CBaseEntity* local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			SDK::CBaseEntity* attacker = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(event->GetInt("attacker")));
			if (!attacker)
				return;

			if (attacker != local_player)
				return;

			//global::killcount++;

			switch (SETTINGS::settings.hitsound2)
			{
			case 0:  break;
			case 1:  PlaySoundA(pew, NULL, SND_ASYNC | SND_MEMORY); break;
			case 2:  PlaySoundA(yourmad, NULL, SND_ASYNC | SND_MEMORY); break;
			case 3:  INTERFACES::Surface->IPlaySound("buttons\\arena_switch_press_02.wav"); break;
			case 4:  PlaySoundA(aimware, NULL, SND_ASYNC | SND_MEMORY); break;
			case 5:  PlaySoundA(rifk7beep, NULL, SND_ASYNC | SND_MEMORY); break;
			case 6:  PlaySoundA(bameWare, NULL, SND_ASYNC | SND_MEMORY); break;
			case 7:  PlaySoundA(odynpapa, NULL, SND_ASYNC | SND_MEMORY); break;
			case 8:  INTERFACES::Surface->IPlaySound("training\\timer_bell.wav"); break;
			case 9:  INTERFACES::Surface->IPlaySound("ui\\deathnotice.wav"); break;
			case 10: PlaySoundA(moan1, NULL, SND_ASYNC | SND_MEMORY); break;
			case 11: PlaySoundA(moan2, NULL, SND_ASYNC | SND_MEMORY); break;
			case 12: PlaySoundA(beepboop, NULL, SND_ASYNC | SND_MEMORY); break;
			}

			//fugma, fugma what? fugma ass
			//							-calvin

			/*if (global::killcount = 1) //again im just a dumb paster :( also tried using killout == 1. still didn't work :--D | 8===D
			{
				PlaySoundA(firstblood, NULL, SND_ASYNC | SND_MEMORY);
			}
			if (global::killcount = 2)
			{
				PlaySoundA(doublekill, NULL, SND_ASYNC | SND_MEMORY);
			}
			if (global::killcount = 3)
			{
				PlaySoundA(triplekill, NULL, SND_ASYNC | SND_MEMORY);
			}
			if (global::killcount = 4)
			{
				PlaySoundA(multikill, NULL, SND_ASYNC | SND_MEMORY);
			}*/
		
				//ultrakill,killing spree, dominating, wikidsick 

			SDK::player_info_t player_info;
			INTERFACES::Engine->GetPlayerInfo(INTERFACES::Engine->GetPlayerForUserID(event->GetInt("userid")), &player_info);

			std::string name = player_info.szName;
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);

			//INTERFACES::cvar->FindVar("con_filter_text")->SetValueChar("killed");
			//ConMsg("killed %s \n", name.c_str());
		}

		void WeaponFireListener::FireGameEvent(SDK::IGameEvent* game_event)
		{

			//MISC::InGameLogger::Log log;

			if (!game_event)
				return;

			bool missed;

			if (!did_hit)
				missed = true;


			if (missed) {
				ConMsg("missed shot due to spread \n");
				missed = false;
			}
			did_hit = false;

			//MISC::in_game_logger.AddLog(log);
		}

		void RoundStartListener::FireGameEvent(SDK::IGameEvent* game_event)
		{

			


			if (!game_event)
				return;

			if (!(INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame()))
				return;

		
			
			SDK::CBaseEntity* local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			global::DisableAA = false;

			//global::killcount = 0; //just to duble confirm that kills = 0
		}
	}
}