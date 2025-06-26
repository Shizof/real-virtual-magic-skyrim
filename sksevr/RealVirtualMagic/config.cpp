#include "config.h"

namespace RealVirtualMagic 
{		
	unsigned short hapticFrequency = 15;
	int hapticStrength = 1;

	double latestBrainPower = 0.0;

	//Other parameters
	bool leftHandedMode = false;

	SkyrimVRESLPluginAPI::ISkyrimVRESLInterface001* skyrimVRESLInterface;

	std::atomic<bool> configSavingMode;

	//Config parameters
	std::shared_ptr<IniSetting> logging;
	std::shared_ptr<IniSetting> useDebugger;
	std::shared_ptr<IniSetting> useBCI;
	std::shared_ptr<IniSetting> minMagickaRate;
	std::shared_ptr<IniSetting> maxMagickaRate;
	std::shared_ptr<IniSetting> minSpellpower;
	std::shared_ptr<IniSetting> maxSpellpower;
	std::shared_ptr<IniSetting> unstableMagicThreshold;
	std::shared_ptr<IniSetting> unstableMagicDamage;
	std::shared_ptr<IniSetting> shieldActivationFocus;
	std::shared_ptr<IniSetting> modifyMagicka;
	std::shared_ptr<IniSetting> modifyMagickaRate;
	std::shared_ptr<IniSetting> modifyMagicPower;
	std::shared_ptr<IniSetting> damageHealth;
	std::shared_ptr<IniSetting> activateShield;
	std::shared_ptr<IniSetting> shieldDuration;
	std::shared_ptr<IniSetting> shieldPercentage;
	std::shared_ptr<IniSetting> shieldColor;
	std::shared_ptr<IniSetting> shieldParticleAmount;
	std::shared_ptr<IniSetting> outOfCombatActive;
	std::shared_ptr<IniSetting> modifyShoutRecovery;
	std::shared_ptr<IniSetting> minShoutRecovery;
	std::shared_ptr<IniSetting> maxShoutRecovery;
	std::shared_ptr<IniSetting> minimumUpdatePercent;

	std::map<std::string, std::shared_ptr<IniSetting>, CaseInsensitiveCompare> iniSettingsMap
	{
		{"logging", std::make_shared<IniSetting>(0, 0, 1, 1,"logging")},
		{"useDebugger", std::make_shared<IniSetting>(0, 0, 1, 1, "useDebugger")},
		{"useBCI", std::make_shared<IniSetting>(1, 0, 1, 1, "useBCI")},
		{"minMagickaRate", std::make_shared<IniSetting>(-10, -100, 100, 1, "minMagickaRate")},
		{"maxMagickaRate", std::make_shared<IniSetting>(10, -100, 100, 1, "maxMagickaRate")},
		{"minSpellpower", std::make_shared<IniSetting>(-100, -100, 100, 1, "minSpellpower")},
		{"maxSpellpower", std::make_shared<IniSetting>(100, -100, 100, 1, "maxSpellpower")},
		{"unstableMagicThreshold", std::make_shared<IniSetting>(0.2, 0.00, 1.00, 0.01, "unstableMagicThreshold")},
		{"unstableMagicDamage", std::make_shared<IniSetting>(10, 0, 100, 1, "unstableMagicDamage")},
		{"shieldActivationFocus", std::make_shared<IniSetting>(0.60, 0.0, 1.0, 0.01, "shieldActivationFocus")},
		{"modifyMagicka", std::make_shared<IniSetting>(1, 0, 1, 1, "modifyMagicka")},
		{"modifyMagickaRate", std::make_shared<IniSetting>(1, 0, 1, 1, "modifyMagickaRate")},
		{"modifyMagicPower", std::make_shared<IniSetting>(1, 0, 1, 1, "modifyMagicPower")},
		{"damageHealth", std::make_shared<IniSetting>(1, 0, 1, 1, "damageHealth")},
		{"activateShield", std::make_shared<IniSetting>(1, 0, 1, 1, "activateShield")},
		{"shieldDuration", std::make_shared<IniSetting>(5.0, 0, 1000.0, 0.1, "shieldDuration")},
		{"shieldPercentage", std::make_shared<IniSetting>(100, 0, 100, 25, "shieldPercentage")},
		{"shieldColor", std::make_shared<IniSetting>(0, 0, 16777215, 1, "shieldColor")},
		{"shieldParticleAmount", std::make_shared<IniSetting>(5, 0, 100, 1, "shieldParticleAmount")},
		{"outOfCombatActive", std::make_shared<IniSetting>(1, 0, 1, 1, "outOfCombatActive")},
		{"modifyShoutRecovery", std::make_shared<IniSetting>(1, 0, 1, 1, "modifyShoutRecovery")},
		{"minShoutRecovery", std::make_shared<IniSetting>(-50, -100, 100, 1, "minShoutRecovery")},
		{"maxShoutRecovery", std::make_shared<IniSetting>(50, -100, 100, 1, "maxShoutRecovery")},
		{"minimumUpdatePercent", std::make_shared<IniSetting>(0.05, 0.01, 0.2, 0.01, "minimumUpdatePercent")}
	};

	void loadConfig()
	{
		logging = iniSettingsMap["logging"];
		useDebugger = iniSettingsMap["useDebugger"];
		useBCI = iniSettingsMap["useBCI"];
		minMagickaRate = iniSettingsMap["minMagickaRate"];
		maxMagickaRate = iniSettingsMap["maxMagickaRate"];
		minSpellpower = iniSettingsMap["minSpellpower"];
		maxSpellpower = iniSettingsMap["maxSpellpower"];
		unstableMagicThreshold = iniSettingsMap["unstableMagicThreshold"];
		unstableMagicDamage = iniSettingsMap["unstableMagicDamage"];
		shieldActivationFocus = iniSettingsMap["shieldActivationFocus"];
		modifyMagicka = iniSettingsMap["modifyMagicka"];
		modifyMagickaRate = iniSettingsMap["modifyMagickaRate"];
		modifyMagicPower = iniSettingsMap["modifyMagicPower"];
		damageHealth = iniSettingsMap["damageHealth"];
		activateShield = iniSettingsMap["activateShield"];
		shieldDuration = iniSettingsMap["shieldDuration"];
		shieldPercentage = iniSettingsMap["shieldPercentage"];
		shieldColor = iniSettingsMap["shieldColor"];
		shieldParticleAmount = iniSettingsMap["shieldParticleAmount"];
		modifyShoutRecovery = iniSettingsMap["modifyShoutRecovery"];
		minShoutRecovery = iniSettingsMap["minShoutRecovery"];
		maxShoutRecovery = iniSettingsMap["maxShoutRecovery"];
		outOfCombatActive = iniSettingsMap["outOfCombatActive"];
		minimumUpdatePercent = iniSettingsMap["minimumUpdatePercent"];

		std::string	runtimeDirectory = GetRuntimeDirectory();

		if (!runtimeDirectory.empty())
		{
			std::string filepath = runtimeDirectory + "Data\\SKSE\\Plugins\\RealVirtualMagic.ini";

			std::ifstream file(filepath);

			if (!file.is_open())
			{
				transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
				file.open(filepath);
			}

			_MESSAGE("Loading config file.");
			if (file.is_open())
			{
				std::string line;
				std::string currentSetting;
				while (std::getline(file, line))
				{
					//trim(line);
					skipComments(line);
					trim(line);
					if (line.length() > 0)
					{
						if (line.substr(0, 1) == "[")
						{
							//newsetting
							currentSetting = line;
						}
						else
						{							
							std::string variableName;
							std::string variableValueStr = GetConfigSettingsStringValue(line, variableName);
							if (variableName == "logging")
							{
								logging.get()->value = std::stoi(variableValueStr);
							}
							else if (variableName == "minMagickaRate")
							{
								minMagickaRate.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "maxMagickaRate")
							{
								maxMagickaRate.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "minSpellpower")
							{
								minSpellpower.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "maxSpellpower")
							{
								maxSpellpower.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "unstableMagicDamage")
							{
								unstableMagicDamage.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "unstableMagicThreshold")
							{
								unstableMagicThreshold.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "shieldActivationFocus")
							{
								shieldActivationFocus.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "shieldDuration")
							{
								shieldDuration.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "shieldPercentage")
							{
								shieldPercentage.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "shieldColor")
							{
								shieldColor.get()->value = HexStringToColor(variableValueStr);
							}
							else if (variableName == "shieldParticleAmount")
							{
								shieldParticleAmount.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "minimumUpdatePercent")
							{
								minimumUpdatePercent.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "modifyMagicka")
							{
								modifyMagicka.get()->value = std::stoi(variableValueStr);
							}
							else if (variableName == "modifyMagickaRate")
							{
								modifyMagickaRate.get()->value = std::stoi(variableValueStr);
							}
							else if (variableName == "modifyMagicPower")
							{
								modifyMagicPower.get()->value = std::stoi(variableValueStr);
							}
							else if (variableName == "damageHealth")
							{
								damageHealth.get()->value = std::stoi(variableValueStr);
							}
							else if (variableName == "activateShield")
							{
								activateShield.get()->value = std::stoi(variableValueStr);
							}
							else if (variableName == "outOfCombatActive")
							{
								outOfCombatActive.get()->value = std::stoi(variableValueStr);
							}
							else if (variableName == "useDebugger")
							{
								useDebugger.get()->value = std::stoi(variableValueStr);
							}
							else if (variableName == "useBCI")
							{
								useBCI.get()->value = std::stoi(variableValueStr);
							}
							else if (variableName == "modifyShoutRecovery")
							{
								modifyShoutRecovery.get()->value = std::stoi(variableValueStr);
							}
							else if (variableName == "minShoutRecovery")
							{
								minShoutRecovery.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else if (variableName == "maxShoutRecovery")
							{
								maxShoutRecovery.get()->value = strtof(variableValueStr.c_str(), 0);
							}
							else
							{
								_MESSAGE("Unknown ini key: %s", variableName.c_str());
							}
						}
					}
				}
			}

			_MESSAGE("Config file is loaded successfully.");
			return;
		}

		_MESSAGE("Config file is not loaded.");
		return;
	}

	void saveConfig()
	{
		std::string	runtimeDirectory = GetRuntimeDirectory();

		if (!runtimeDirectory.empty())
		{
			std::string filepath = runtimeDirectory + "Data\\SKSE\\Plugins\\RealVirtualMagic.ini";

			std::ofstream output(filepath, std::fstream::out);
			if (!output.is_open()) {
				LOG_ERR("Failure while saving config file.");
				return;
			}

			output << std::fixed << std::setprecision(2);
			output << "#######################################################################################################\n";
			output << "# This is the config file for the Real Virtual Magic Mod. You can also use MCM to change the settings.\n";
			output << "# \n";
			output << "# # ->This is the comment character.\n";
			output << "# \n";
			output << "#######################################################################################################\n";
			output << "\n";
			output << "[General]\n";
			output << "logging = " << logging.get()->intValue() << "\t\t\t\t# Change to 1 to enable logging in the SKSE log. Only do this if something isn't right. It would hurt performance if it's left on\n";
			output << "useDebugger = " << useDebugger.get()->intValue() << " \t\t\t# DO NOT USE unless you have a debugger ready that can hook into the mod. It will freeze the start of Skyrim otherwise\n";
			output << "useBCI = " << useBCI.get()->intValue() << " \t\t\t\t# If 0 the BCI will not be applied to change any ingame values, but events will be streamed to LSL. 1 is regular use.\n";
			output << "\n";
			output << "[Magic]\n";
			output << "outOfCombatActive = " << outOfCombatActive.get()->intValue() << " \t\t\t\t# Should effects be active when out of combat\n";
			output << "\n";
			output << "modifyMagickaRate = " << modifyMagickaRate.get()->intValue() << " \t\t\t\t# Should magicka rate be modified according to focus\n";
			output << "minMagickaRate = " << minMagickaRate.get()->value << "\t\t\t\t# magicka regeneration rate in % of max magicka when brain power is zero\n";
			output << "maxMagickaRate = " << maxMagickaRate.get()->value << "\t\t\t\t# magicka regeneration rate in % of max magicka when brain power is full\n";
			output << "modifyMagicka = " << modifyMagicka.get()->intValue() << " \t\t\t\t# Should it reduce magicka if magicka regeneration rate is negative\n";
			output << "\n";
			output << "modifyMagicPower = " << modifyMagicPower.get()->intValue() << " \t\t\t\t# Should magic power be modified according to focus\n";
			output << "minSpellpower = " << minSpellpower.get()->value << "\t\t\t\t# malus in % value of regular spellpower when brain power is zero\n";
			output << "maxSpellpower = " << maxSpellpower.get()->value << "\t\t\t\t# bonus in % value of regular spellpower when brain power is full\n";
			output << "\n";
			output << "damageHealth = " << damageHealth.get()->intValue() << " \t\t\t\t# Should unstable brain power damage player health\n";
			output << "unstableMagicThreshold = " << unstableMagicThreshold.get()->value << "\t\t\t# brain power below this threshold will lead to unstable magic which damages the player when starting a cast\n";
			output << "unstableMagicDamage = " << unstableMagicDamage.get()->value << "\t\t\t# health damage in % of max health\n";
			output << "\n";
			output << "activateShield = " << activateShield.get()->intValue() << " \t\t\t\t# Should activate shield when focus is high\n";
			output << "shieldActivationFocus = " << shieldActivationFocus.get()->value << "\t\t\t# This is the minimum focus amount for shield activation\n";
			output << "shieldDuration = " << shieldDuration.get()->value << "\t\t\t# This is the duration the shield will stay effective even if the focus drops.\n";
			output << "shieldPercentage = " << shieldPercentage.get()->value << "\t\t\t# This is the percentage amount the shield will protect the player (Allowed values: 100,75,50,25,0)\n";
			output << "shieldColor = " << ColorToHexString(shieldColor.get()->intValue()) << "\t\t\t# This is the color of the shield effect on player\n";
			output << "\n";
			output << "modifyShoutRecovery = " << modifyShoutRecovery.get()->intValue() << " \t\t\t\t# Should shout recovery be modified according to focus\n";
			output << "minShoutRecovery = " << minShoutRecovery.get()->value << "\t\t\t\t# malus in % value of regular shout recovery multiplier when brain power is zero\n";
			output << "maxShoutRecovery = " << maxShoutRecovery.get()->value << "\t\t\t\t# bonus in % value of regular shout recovery multiplier when brain power is full\n";
			output << "\n";
			output << "minimumUpdatePercent = " << minimumUpdatePercent.get()->value << "\t\t\t# This is the minimum percentage of change to reapply effects. Between 0-1.\n";

			output.close();
		}
	}

	float GetBrainPower(StaticFunctionTag* base)
	{
		return latestBrainPower;
	}

	void DisableConfigSavingModeThread()
	{
		Sleep(4000);
		configSavingMode.store(false);
	}

	void SetValue(StaticFunctionTag* base, BSFixedString settingName, UInt32 intValue, float floatValue)
	{
		bool disableNeeded = false;
		if (settingName.data)
		{
			auto setting = iniSettingsMap[settingName.data];
			if (setting != nullptr && setting.get() != nullptr)
			{
				configSavingMode.store(true);
				disableNeeded = true;
				setting.get()->value = floatValue;
				//LOG_ERR("Saving value for %s: %g", settingName.data, floatValue);
			}
		}

		saveConfig();

		if (disableNeeded)
		{
			AfterGameOpens(true);
			std::thread t1(DisableConfigSavingModeThread);
			t1.detach();
		}
	}

	float GetValue(StaticFunctionTag* base, BSFixedString settingName)
	{
		float value = 0.0f;
		if (settingName.data)
		{
			auto setting = iniSettingsMap[settingName.data];
			if (setting != nullptr && setting.get() != nullptr)
			{
				value = setting.get()->value;
			}
		}
		//LOG_ERR("Returning value for %s: %g", settingName.data, value);
		return value;
	}

	float GetSettingDefault(StaticFunctionTag* base, BSFixedString name)
	{
		float value = 0.0f;
		if (name.data)
		{
			auto setting = iniSettingsMap[name.data];
			if (setting != nullptr && setting.get() != nullptr)
				value = setting.get()->defaultVal;
		}
		//LOG_ERR("Returning default value for %s: %g", name.data, value);
		return value;
	}
	float GetSettingMin(StaticFunctionTag* base, BSFixedString name)
	{
		float value = 0.0f;
		if (name.data)
		{
			auto setting = iniSettingsMap[name.data];
			if (setting != nullptr && setting.get() != nullptr)
				value = setting.get()->minVal;
		}
		//LOG_ERR("Returning minimum value for %s: %g", name.data, value);
		return value;
	}
	float GetSettingMax(StaticFunctionTag* base, BSFixedString name)
	{
		float value = 0.0f;
		if (name.data)
		{
			auto setting = iniSettingsMap[name.data];
			if (setting != nullptr && setting.get() != nullptr)
				value = setting.get()->maxVal;
		}
		//LOG_ERR("Returning maximum value for %s: %g", name.data, value);
		return value;
	}
	float GetSettingInterval(StaticFunctionTag* base, BSFixedString name)
	{
		float value = 0.0f;
		if (name.data)
		{
			auto setting = iniSettingsMap[name.data];
			if (setting != nullptr && setting.get() != nullptr)
				value = setting.get()->interval;
		}
		//LOG_ERR("Returning interval value for %s: %g", name.data, value);
		return value;
	}

	void Log(const int msgLogLevel, const char* fmt, ...)
	{
		if (logging.get() && msgLogLevel > logging.get()->intValue())
		{
			return;
		}

		va_list args;
		char logBuffer[4096];

		va_start(args, fmt);
		vsprintf_s(logBuffer, sizeof(logBuffer), fmt, args);
		va_end(args);

		_MESSAGE(logBuffer);
	}
}