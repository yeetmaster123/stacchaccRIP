#include "night_mode.h"

void cnight_mode::do_nightmode() {

	static bool nightmode_performed = false, nightmode_lastsetting;

	if (!INTERFACES::Engine->IsConnected() || !INTERFACES::Engine->IsInGame()) {

		if (nightmode_performed)
			nightmode_performed = false;
		return;
	}

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (!local_player) {

		if (nightmode_performed)
			nightmode_performed = false;
		return;
	}

	if (nightmode_lastsetting != SETTINGS::settings.night_bool) {

		nightmode_lastsetting = SETTINGS::settings.night_bool;
		nightmode_performed = false;
	}

	if (!nightmode_performed) {

		static SDK::ConVar* r_DrawSpecificStaticProp = INTERFACES::cvar->FindVar("r_DrawSpecificStaticProp");
		r_DrawSpecificStaticProp->nFlags &= ~FCVAR_CHEAT;

		static SDK::ConVar* sv_skyname = INTERFACES::cvar->FindVar("sv_skyname");
		sv_skyname->nFlags &= ~FCVAR_CHEAT;

		for (SDK::MaterialHandle_t i = INTERFACES::MaterialSystem->FirstMaterial(); i != INTERFACES::MaterialSystem->InvalidMaterial(); i = INTERFACES::MaterialSystem->NextMaterial(i)) {

			SDK::IMaterial *pMaterial = INTERFACES::MaterialSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "World")) {

				if (SETTINGS::settings.night_bool)
					pMaterial->ColorModulate(0.08, 0.08, 0.05);
				else
					pMaterial->ColorModulate(1, 1, 1);

				if (SETTINGS::settings.night_bool) {

					sv_skyname->SetValue("sky_csgo_night02");
					pMaterial->SetMaterialVarFlag(SDK::MATERIAL_VAR_TRANSLUCENT, false);
					pMaterial->ColorModulate(0.05, 0.05, 0.05);
				} else {

					sv_skyname->SetValue("vertigoblue_hdr");
					pMaterial->ColorModulate(1.00, 1.00, 1.00);
				}
			} else if (strstr(pMaterial->GetTextureGroupName(), "StaticProp")) {

				if (SETTINGS::settings.night_bool)
					pMaterial->ColorModulate(0.28, 0.28, 0.28);
				else
					pMaterial->ColorModulate(1, 1, 1);
			}
		} nightmode_performed = true;
	}
}

cnight_mode* nightmode = new cnight_mode();