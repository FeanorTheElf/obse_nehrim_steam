#include "obse/PluginAPI.h"
#include "obse/CommandTable.h"
#include "Achievements.h"

#if OBLIVION
#include "obse/GameAPI.h"
#include "obse/GameMenus.h"
#else
#include "obse_editor/EditorAPI.h"
#endif

#include "obse/ParamInfos.h"
#include "obse/Script.h"
#include "obse/GameObjects.h"
#include <string>

IDebugLog		gLog("obse_nehrim_steam.log");

PluginHandle				g_pluginHandle = kPluginHandle_Invalid;
OBSEScriptInterface         * g_scriptInterface = NULL;
OBSEArrayVarInterface		* g_arrayIntfc = NULL;
OBSEScriptInterface			* g_scriptIntfc = NULL;
OBSEMessagingInterface      * g_msg = NULL;

// Because of some strange reason, a definition of this function is not provided anywhere else, so give it here so that we do not get unresolved symbol
void ShowRuntimeError(Script* script, const char* fmt, ...)
{
	char errorHeader[0x400];
	sprintf_s(errorHeader, 0x400, "Error in script %08x", script ? script->refID : 0);

	va_list args;
	va_start(args, fmt);

	char	errorMsg[0x400];
	vsprintf_s(errorMsg, 0x400, fmt, args);

	Console_Print(errorHeader);
	_MESSAGE(errorHeader);
	Console_Print(errorMsg);
	_MESSAGE(errorMsg);

	va_end(args);
}

#if OBLIVION

bool Cmd_UnlockAchievement_Execute(COMMAND_ARGS)
{
	char achievementCode[512] = { 0 };
	if (ExtractArgs(EXTRACT_ARGS, &achievementCode))
		Achievements::setAchievementUnlocked(achievementCode);

	return true;
}

#endif

DEFINE_COMMAND_PLUGIN(UnlockAchievement,
	"Unlocks the steam achievement with the given achievement code.",
	0,
	1,
	kParams_OneString)


DWORD WINAPI SteamInitAndEventLoopThread(LPVOID lpParam)
{
	while (true) {
		InterfaceManager * intm = InterfaceManager::GetSingleton();
		if (intm && intm->activeMenu) {
			if (intm->activeMenu->id == 1044) {
				Achievements::startSteam();
				break;
			}
		}
		Sleep(100);
	}
	_MESSAGE("Starting Steam event loop");
	while (true) {
		Achievements::runSteamCallbacks();
		Sleep(100);
	}
	return 0;
}

void MessageHandler(OBSEMessagingInterface::Message* msg)
{
	if (msg->type == OBSEMessagingInterface::kMessage_PostLoad) {
		DWORD   dwthreadId;
		HANDLE  hthread = CreateThread(NULL, 0, SteamInitAndEventLoopThread, 0, 0, &dwthreadId);
	}
}

extern "C" {

bool OBSEPlugin_Query(const OBSEInterface * obse, PluginInfo * info)
{
	_MESSAGE("query");

	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "nehrim_steam";
	info->version = 1;

	if(!obse->isEditor)
	{
		if(obse->obseVersion < OBSE_VERSION_INTEGER)
		{
			_ERROR("OBSE version too old (got %08X expected at least %08X)", obse->obseVersion, OBSE_VERSION_INTEGER);
			return false;
		}

#if OBLIVION
		if(obse->oblivionVersion != OBLIVION_VERSION)
		{
			_ERROR("incorrect Oblivion version (got %08X need %08X)", obse->oblivionVersion, OBLIVION_VERSION);
			return false;
		}
#endif

		g_arrayIntfc = (OBSEArrayVarInterface*)obse->QueryInterface(kInterface_ArrayVar);
		if (!g_arrayIntfc)
		{
			_ERROR("Array interface not found");
			return false;
		}

		g_scriptIntfc = (OBSEScriptInterface*)obse->QueryInterface(kInterface_Script);		
	}

	return true;
}

bool OBSEPlugin_Load(const OBSEInterface * obse)
{
	_MESSAGE("load");

	g_pluginHandle = obse->GetPluginHandle();

	/***************************************************************************
	 *	
	 *	READ THIS!
	 *	
	 *	Before releasing your plugin, you need to request an opcode range from
	 *	the OBSE team and set it in your first SetOpcodeBase call. If you do not
	 *	do this, your plugin will create major compatibility issues with other
	 *	plugins, and may not load in future versions of OBSE. See
	 *	obse_readme.txt for more information.
	 *	
	 **************************************************************************/

	obse->SetOpcodeBase(0x2000);
	obse->RegisterCommand(&kCommandInfo_UnlockAchievement);

	if(!obse->isEditor)
	{
		OBSEStringVarInterface* g_Str = (OBSEStringVarInterface*)obse->QueryInterface(kInterface_StringVar);
		g_Str->Register(g_Str);

		g_scriptInterface = (OBSEScriptInterface*)obse->QueryInterface(kInterface_Script);

		OBSEMessagingInterface* msgIntfc = (OBSEMessagingInterface*)obse->QueryInterface(kInterface_Messaging);
		msgIntfc->RegisterListener(g_pluginHandle, "OBSE", MessageHandler);
		g_msg = msgIntfc;
	}

	return true;
}

};
