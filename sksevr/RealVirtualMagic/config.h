#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

#include <thread>
#include <atomic>

#include "skse64\NiNodes.h"
#include "skse64\NiTypes.h"
#include "skse64/GameObjects.h"
#include "skse64/NiExtraData.h"
#include "skse64/GameRTTI.h"
#include "skse64/InternalTasks.h"
#include "skse64/PluginAPI.h"
#include "skse64/PapyrusActor.h"
#include "skse64/PapyrusGame.h"
#include "skse64_common/SafeWrite.h"
#include "skse64/GameExtraData.h"
#include <skse64/PapyrusWornObject.h>
#include "skse64/GameReferences.h"
#include "skse64/NiGeometry.h"
#include <skse64/PapyrusConstructibleObject.h>
#include "skse64/PapyrusPotion.h"
#include "skse64\GameMenus.h"
#include "skse64/GameEvents.h"
#include "skse64/GameInput.h"
#include "skse64/GameVR.h"
#include "skse64/PapyrusNativeFunctions.h"
#include "skse64\PapyrusEvents.h"
#include "skse64\PapyrusSpell.h"
#include "skse64\PapyrusForm.h"
#include "skse64/PluginAPI.h"	
#include "skse64\GameSettings.h"
//#include "Utility.hpp"

#include "SkyrimVRESLAPI.h"


extern SKSETaskInterface* g_task;

namespace RealVirtualMagic 
{
	class IniSetting
	{
	public:
		double value;
		double defaultVal;
		double minVal;
		double maxVal;
		double interval;
		const char* settingName;

		IniSetting(double defaultVal = 0, 
			double minVal = 0, 
			double maxVal = 0, 
			double interval = 0, 
			const char* skyrimSettingName = "") :
			value(defaultVal), defaultVal(defaultVal), minVal(minVal), maxVal(maxVal), interval(interval), settingName(settingName) { }

		int intValue() { return static_cast<int>(value + 0.5); }
		bool boolValue() { return (value != 0); }
	};

	//function declarations
	void loadConfig();
		
	void saveConfig();

	extern unsigned short hapticFrequency;
	extern int hapticStrength;

	//Other parameters
	extern bool leftHandedMode;

	extern double latestBrainPower;

	extern std::atomic<bool> configSavingMode;

	struct CaseInsensitiveCompare 
	{
		bool operator()(const std::string& a, const std::string& b) const 
		{
			return _stricmp(a.c_str(), b.c_str()) < 0;
		}
	};

	extern SkyrimVRESLPluginAPI::ISkyrimVRESLInterface001* skyrimVRESLInterface;

	extern std::shared_ptr<IniSetting> logging;
	extern std::shared_ptr<IniSetting> useDebugger;
	extern std::shared_ptr<IniSetting> useBCI;
	extern std::shared_ptr<IniSetting> minMagickaRate;
	extern std::shared_ptr<IniSetting> maxMagickaRate;
	extern std::shared_ptr<IniSetting> minSpellpower;
	extern std::shared_ptr<IniSetting> maxSpellpower;
	extern std::shared_ptr<IniSetting> unstableMagicThreshold;
	extern std::shared_ptr<IniSetting> unstableMagicDamage;
	extern std::shared_ptr<IniSetting> shieldActivationFocus;
	extern std::shared_ptr<IniSetting> modifyMagicka;
	extern std::shared_ptr<IniSetting> modifyMagickaRate;
	extern std::shared_ptr<IniSetting> modifyMagicPower;
	extern std::shared_ptr<IniSetting> damageHealth;
	extern std::shared_ptr<IniSetting> activateShield;
	extern std::shared_ptr<IniSetting> modifyShoutRecovery;
	extern std::shared_ptr<IniSetting> minShoutRecovery;
	extern std::shared_ptr<IniSetting> maxShoutRecovery;
	extern std::shared_ptr<IniSetting> outOfCombatActive;
	extern std::shared_ptr<IniSetting> minimumUpdatePercent;

	extern std::map<std::string, std::shared_ptr<IniSetting>, CaseInsensitiveCompare> iniSettingsMap;

	float GetBrainPower(StaticFunctionTag* base);
	void SetValue(StaticFunctionTag* base, BSFixedString settingName, UInt32 intValue, float floatValue);
	float GetValue(StaticFunctionTag* base, BSFixedString settingName);
	float GetSettingDefault(StaticFunctionTag* base, BSFixedString name);
	float GetSettingMin(StaticFunctionTag* base, BSFixedString name);
	float GetSettingMax(StaticFunctionTag* base, BSFixedString name);
	float GetSettingInterval(StaticFunctionTag* base, BSFixedString name);



	void Log(const int msgLogLevel, const char* fmt, ...);
	enum eLogLevels
	{
		LOGLEVEL_ERR = 0,
		LOGLEVEL_WARN,
		LOGLEVEL_INFO,
	};

#define LOG(fmt, ...) Log(LOGLEVEL_WARN, fmt, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) Log(LOGLEVEL_ERR, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) Log(LOGLEVEL_INFO, fmt, ##__VA_ARGS__)

}