std::string stations[] = {
	"http://dutchcore.fm:8100/high",
	"http://uk5.internet-radio.com:8270/",
	"http://streams.bigfm.de/bigfm-deutschland-128-mp3",
	"https://streams.bigfm.de/bigfm-deutschrap-128-mp3",
	"http://air2.radiorecord.ru:805/rr_320",
	"http://air.radiorecord.ru:805/dub_320",
	"http://air.radiorecord.ru:805/teo_320",
	"http://www.radiorecord.ru/player-mini/?st=hbass",
	"http://www.radiorecord.ru/player-mini/?st=rapclassics"
};


const char* radio_name[] = {
	"DutchCore",
	"HardStyle",
	"BigFm",
	"BigFm deutsch rap",
	"Radio Record",
	"Record Dubstep",
	"Record Hardstyle",
	"Hard-bass",
	"Hip-Hop"
};

/*
#include "../MENU/bass.h"
#include "../MENU/xor.h"
#include "../MENU/radionstuff.h"
#pragma comment(lib, "bass.lib")
#pragma comment (lib, "d3d9.lib")

const char* tabNames[] = { "Ragebot", "Visuals", "Misc", "Skins", "Configs", "Radio" };

void CMenu::radioTab() {
	TabStyle();
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

	} ImGui::EndChild();
}
*/