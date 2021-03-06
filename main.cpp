#include "includes.h"

#include "UTILS\interfaces.h"
#include "HOOKS\hooks.h"
#include "UTILS\offsets.h"
#include "FEATURES\EventListener.h"
#include "SDK\RecvData.h"
#include "SDK\CClientEntityList.h"
#include "SDK\IEngine.h"
#include "FEATURES/recv.h"
#include "FEATURES/NewEventLog.h"
#include <chrono>
#include <thread>
#include <map>
#include "external dependencies/steam api/isteamfriends.h"
#include "SDK/ConVar.h"
#include "SDK/IClient.h"
#include <string>
#include <sstream>

using namespace std::literals::chrono_literals;

bool using_fake_angles[65];
bool full_choke;
bool is_shooting;

bool in_tp;
bool fake_walk;
bool in_slowalk;
bool flipaa;

int resolve_type[65];

int target;
int shots_fired[65];
int shots_hit[65];
int shots_missed[65];
bool didMiss = true;
bool didShot = false;
int backtrack_missed[65];

float tick_to_back[65];
float lby_to_back[65];
bool backtrack_tick[65];

float lby_delta;
float update_time[65];
float walking_time[65];

float local_update;

int hitmarker_time;
int random_number;

bool menu_hide;

int oldest_tick[65];
float compensate[65][12];
Vector backtrack_hitbox[65][20][12];
float backtrack_simtime[65][12];
bool fake = false;

HMODULE get_module_handle(wchar_t* name) {

	static std::map<wchar_t*, HMODULE> modules = { };

	if (modules.count(name) == 0)
		modules[name] = GetModuleHandleW(name);

	return modules[name];
}

ISteamFriends* SteamFriends() {

	static ISteamFriends* result;

	if (!result) {

		auto steam_api = get_module_handle(L"steam_api.dll");
		HSteamUser hSteamUser = ((HSteamUser(__cdecl*)(void))GetProcAddress(steam_api, "SteamAPI_GetHSteamUser"))();
		HSteamPipe hSteamPipe = ((HSteamPipe(__cdecl*)(void))GetProcAddress(steam_api, "SteamAPI_GetHSteamPipe"))();
		ISteamClient* g_SteamClient = ((ISteamClient*(__cdecl*)(void))GetProcAddress(steam_api, "SteamClient"))();
		result = g_SteamClient->GetISteamFriends(hSteamUser, hSteamPipe, "SteamFriends015");
	} return result;
}

// CRASH LIST

/*
0x4E414542 - Don't know yet

Exception thrown at 0x5A8B925B (vstdlib.dll) in csgo.exe: 0xC0000005: Access violation reading location 0xCC00443A.

Exception thrown at 0x2E55842B (client_panorama.dll) in csgo.exe: 0xC5: Access violation reading location 0x24.

*/

/*class netvar_manager {

public:
	std::string DumpTable(SDK::RecvTable * table, int depth);
	void DumpOffsets(std::string fileName);
};*/

/*std::string netvar_manager::DumpTable(SDK::RecvTable* table, int depth) {

	std::string pre("");
	std::stringstream ss;

	for (int i = 0; i < depth; i++)
		pre.append("\t");

	ss << pre << table->m_pNetTableName << "\n";

	for (int i = 0; i < table->m_nProps; i++) {

		SDK::RecvProp* prop = &table->m_pProps[i];
		if (!prop)
			continue;

		std::string varName(prop->m_pVarName);

		if (!(varName.find("baseclass") || varName.find("0") || varName.find("1") || varName.find("2")))
			continue;

		ss << pre << "\t" << varName << " [0x" << std::hex << prop->m_Offset << "]\n";

		if (prop->m_pDataTable)
			ss << DumpTable(prop->m_pDataTable, depth + 1);
	}

	ss << std::endl;

	return ss.str();
}*/

/*void netvar_manager::DumpOffsets(std::string fileName) {

	std::ofstream output(fileName);
	output.clear();
	std::stringstream ss;

	for (SDK::ClientClass* pClass = INTERFACES::Client->GetAllClasses(); pClass != NULL; pClass = pClass->m_pNext) {

		SDK::RecvTable* table = pClass->m_pRecvTable;
		ss << this->DumpTable(table, false);
	}

	output << "[OOFSETs DUMPER] \n\n Last checked: " << __TIME__ << "\n\n";
	output << ss.str();
	output.close();

	UTILS::ConsolePrint("[OOFSETs] Dumped offsets to: (%s)", fileName.c_str());
}*/

void patchLogs() {

	//if (!INTERFACES::Engine->Con_BoneIsVisible()) //seems to cause crashes, so commented it. Use it if you want
		//INTERFACES::Engine->ExecuteClientCmd("toggleconsole");

	//INTERFACES::Engine->ExecuteClientCmd("clear");

	/*std::string playerName = SteamFriends()->GetPersonaName();

	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "\n[Wanheda] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Welcome back, ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(20, 255, 20), playerName.c_str()); /// Welcome back, lexi
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "\n");

	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "This cheat was reworked in C++ by ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "lexi ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "& ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "SkymeR \n");

	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Menu made by ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "IcePixelx ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "if you want one make sure to ask for a ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "paid ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "request with ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "@IcePixelx#4931 ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "on our Discord. \n\n");

	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Discord Link:");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), " https://discord.gg/b6nnV48 \n\n");

	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "If you are running into trouble or crashes please tag us with ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(33, 202, 218), "@SUPPORT ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "on Discord to get our attention. \n");

	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Use the ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "INSERT ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "key to open the menu! (NOTE: make sure to be tabbed in the game when you inject to prevent crashes (use windowed fullscreen when you inject)) \n\n");

	INTERFACES::cvar->ConsoleColorPrintf(CColor(215, 210, 120), "Changelog: \n\n");

	INTERFACES::cvar->ConsoleColorPrintf(CColor(170, 55, 70), " + [RAGEBOT] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Added fakelag prediction \n");

	INTERFACES::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Bug fixes and improvements \n");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Added animated clantag \n");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Remade animfix \n");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Remade fakewalk \n");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed standing fakelag breaking when using adaptive fakelag mode \n");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Standing fakelag now disables on lby break \n");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed crashes when using rainbow spread crosshair \n");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed auto-stop \n");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed some crashes on injection \n");

	INTERFACES::cvar->ConsoleColorPrintf(CColor(220, 145, 30), " ~ [ANTI-AIM] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Completely remade antiaims and freestanding \n");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(220, 145, 30), " ~ [ANTI-AIM] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed LBY breaker \n");

	INTERFACES::cvar->ConsoleColorPrintf(CColor(90, 245, 90), " + [VISUALS] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Remade thirdperson \n");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(90, 245, 90), " + [VISUALS] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed antiaim arrows \n");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(90, 245, 90), " + [VISUALS] ");
	INTERFACES::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Made LBY indicator infallible \n");*/
}

void Start() {

	INTERFACES::InitInterfaces();
	OFFSETS::InitOffsets();
	NetvarHook();
	UTILS::INPUT::input_handler.Init();
	FONTS::InitFonts();
	HOOKS::InitHooks();
	HOOKS::InitNetvarHooks();
	FEATURES::MISC::InitializeEventListeners();
	//patchLogs();

	//netvar_manager offsets;

	//offsets.DumpOffsets("offsets.txt");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {

	switch (dwReason)
	{

	case DLL_PROCESS_ATTACH:
	{
		CreateDirectoryA(("C:\\nnwareRECODEbeta"), NULL);
		CreateDirectoryA(("C:\\nnwareRECODEbeta\\Logs"), NULL);
		CreateDirectoryA(("C:\\nnwareRECODEbeta\\Configs"), NULL);
		CreateDirectoryA(("C:\\nnwareRECODEbeta\\Resources"), NULL);
		CreateDirectoryA(("C:\\nnwareRECODEbeta\\Resources\\Fonts"), NULL);

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Start, NULL, NULL, NULL);
	}
	break;
	case DLL_PROCESS_DETACH:
	{
		LOG("DETACHING");
		RemoveFontResource("C:\\nnwareRECODEbeta\\Resources\\Fonts\\againts.ttf");
		RemoveFontResource("C:\\nnwareRECODEbeta\\Resources\\Fonts\\Vermin_Vibes.ttf");
	}
	break;
	}

	return true;
}