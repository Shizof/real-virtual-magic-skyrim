#include "Engine.h"


namespace RealVirtualMagic
{
	bool runOnce = false; 

	bool isInCombat = false;

	std::atomic<bool> LeftHandSpellCastingOn;
	std::atomic<bool> RightHandSpellCastingOn;
	std::atomic<bool> DualSpellCastingOn;
	std::atomic<bool> DualAimedStarted;
	std::atomic<bool> LeftAimedStarted;
	std::atomic<bool> RightAimedStarted;

	// Animation graph hook for the player that allows specific animations to be blocked from playing
	typedef uint64_t(*IAnimationGraphManagerHolder_NotifyAnimationGraph_VFunc)(uintptr_t* iagmh, BSFixedString* animName);
	IAnimationGraphManagerHolder_NotifyAnimationGraph_VFunc g_originalNotifyAnimationGraph = nullptr; // Normally a JMP to 0x00501530
	static RelocPtr<IAnimationGraphManagerHolder_NotifyAnimationGraph_VFunc> IAnimationGraphManagerHolder_NotifyAnimationGraph_vtbl(0x016E2BF8);
	
	uint64_t Hooked_IAnimationGraphManagerHolder_NotifyAnimationGraph(uintptr_t* iAnimationGraphManagerHolder, BSFixedString* animationName)
	{
		//_MESSAGE("Called %s", animationName->data);

		if (strcmp(animationName->data, "MRh_Equipped_Event") == 0)
		{
			RightHandSpellCastingOn.store(false);
			DualSpellCastingOn.store(false);
		}
		else if (strcmp(animationName->data, "MLh_Equipped_Event") == 0)
		{
			LeftHandSpellCastingOn.store(false);
			DualSpellCastingOn.store(false);
		}
		else if (strcmp(animationName->data, "DualMagic_SpellAimedConcentrationStart") == 0 || strcmp(animationName->data, "DualMagic_SpellSelfConcentrationStart") == 0 || strcmp(animationName->data, "RitualSpellAimConcentrationStart") == 0)
		{
			DualSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "MRh_SpellAimedConcentrationStart") == 0 || strcmp(animationName->data, "MRh_SpellSelfConcentrationStart") == 0 || strcmp(animationName->data, "MRh_SpellTelekinesisStart") == 0)
		{
			RightHandSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "MLh_SpellAimedConcentrationStart") == 0 || strcmp(animationName->data, "MLh_SpellSelfConcentrationStart") == 0 || strcmp(animationName->data, "MLh_SpellTelekinesisStart") == 0)
		{
			LeftHandSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "MLh_SpellAimedStart") == 0 || strcmp(animationName->data, "MLh_SpellSelfStart") == 0)
		{
			LeftAimedStarted.store(true);
			LeftHandSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "MRh_SpellAimedStart") == 0 || strcmp(animationName->data, "MRh_SpellSelfStart") == 0)
		{
			RightAimedStarted.store(true);
			RightHandSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "DualMagic_SpellAimedStart") == 0 || strcmp(animationName->data, "DualMagic_SpellSelfStart") == 0 || strcmp(animationName->data, "RitualSpellStart") == 0)
		{
			DualAimedStarted.store(true);
			DualSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "RitualSpellOut") == 0)
		{
			if (DualAimedStarted.load() || DualSpellCastingOn.load())
			{
				DualAimedStarted.store(false);
				DualSpellCastingOn.store(false);
				EndCast();
			}
		}
		else if (strcmp(animationName->data, "MRh_SpellRelease_Event") == 0)
		{
			if (RightAimedStarted.load())
			{
				RightAimedStarted.store(false);
				RightHandSpellCastingOn.store(false);
				EndCast();
			}
			if (DualAimedStarted.load())
			{
				DualAimedStarted.store(false);
				DualSpellCastingOn.store(false);
				EndCast();
			}
		}
		else if (strcmp(animationName->data, "MLH_SpellRelease_event") == 0)
		{
			if (LeftAimedStarted.load())
			{
				LeftAimedStarted.store(false);
				LeftHandSpellCastingOn.store(false);
				EndCast();
			}
			if (DualAimedStarted.load())
			{
				DualAimedStarted.store(false);
				DualSpellCastingOn.store(false);
				EndCast();
			}
		}
		// return 0 to block the animation from playing
		// Otherwise return...
		return   g_originalNotifyAnimationGraph(iAnimationGraphManagerHolder, animationName);
	}

	void GameLoad()
	{
		if (useDebugger.get()->boolValue())
		{
			WaitForDebugger(true);
		}

		if (runOnce == false)
		{
			runOnce = true;

			//This stuff runs only once after game loaded
			LoadValues();
		}
	}

	void HandHapticFeedbackEffect(bool left)
	{
		if (*g_openVR)
		{
			for (int i = 0; i < hapticStrength; i++)
			{
				if (leftHandedMode == true)
				{
					left = !left;
				}
				LOG_INFO("Triggering haptic pulse for %g on %s...", (float)hapticFrequency, left ? "left hand" : "right hand");
				(*g_openVR)->TriggerHapticPulse(left ? BSVRInterface::kControllerHand_Left : BSVRInterface::kControllerHand_Right, (float)hapticFrequency / 3999.0f);
			}
		}
		else
		{
			LOG("g_openVR is false...");
		}
	}

	void LeftHandedModeChange()
	{
		const int value = vlibGetSetting("bLeftHandedMode:VRInput");
		if ((value==1) != leftHandedMode)
		{
			leftHandedMode = value;
			LOG_ERR("Left Handed Mode is %s.", leftHandedMode ? "ON" : "OFF");
		}
	}

	void CheckMagickCasting()
	{
		while (IsCasting())
		{
			Sleep(22); // 1 frame with reprojection in most HMDs
		}
		EndCast();

	}

	bool IsCasting()
	{
		return (DualSpellCastingOn.load() || LeftHandSpellCastingOn.load() || RightHandSpellCastingOn.load());
	}

	void GameLogic()
	{
		// set up the LSL stream in the beginning
		CreateSystem();

		// game logic runs forever here
		while (true)
		{
			if (ActorInCombat(*g_thePlayer) && !isInCombat)
			{
				WriteEventMarker("combat:start");
				isInCombat = true;
			}
			else if (!ActorInCombat(*g_thePlayer) && isInCombat)
			{
				WriteEventMarker("combat:stop");
				isInCombat = false;
			}

			// we need the option to run the mod to provide events, but not apply the actual brain power
			if (useBCI.get()->boolValue()) {
				
				// this will be true only when the LSL stream is there
				if (IXRInitialized)
				{
					latestBrainPower = GetFocusValue();

					LOG("------------------");
					LOG("latest focus value: %g", latestBrainPower);
					if (latestBrainPower > 1.0 || latestBrainPower < 0.0)
					{
						LOG_ERR("Error, getting a weird focus value: %g. Clamping.", latestBrainPower);
						latestBrainPower = clamp(latestBrainPower, 0.0, 1.0);
					}

					ApplyFocusValue(latestBrainPower);

					Sleep(50); // we expect new values every roughly 50ms
				}
				else
				{
					LOG("IXR not initialized. Attempting to create LSL system.");
					LOG("Setting focus to -999.0 to disable all effects");
					ApplyFocusValue(-999.0);
					// set up the LSL stream
					CreateSystem();
				}
			}
			else
			{
				//LOG_ERR("Current magicka: %g", GetCurrentMagicka());
				Sleep(1000);
			}			
		}
	}

	void StartFunction()
	{		
		LeftHandedModeChange();

		//You can create a thread here like this and run all your game logic in that
		std::thread t1(GameLogic);
		t1.detach();

	}

	void WaitForDebugger(bool should_break = false) {
		while (!IsDebuggerPresent())
			Sleep(100);
		if (should_break)
			DebugBreak();
	}

	void HookAnimations()
	{
		g_originalNotifyAnimationGraph = *IAnimationGraphManagerHolder_NotifyAnimationGraph_vtbl;
		SafeWrite64(IAnimationGraphManagerHolder_NotifyAnimationGraph_vtbl.GetUIntPtr(), uintptr_t(Hooked_IAnimationGraphManagerHolder_NotifyAnimationGraph));
	}

	

	bool RegisterFuncs(VMClassRegistry* registry)
	{
		registry->RegisterFunction(new NativeFunction0<StaticFunctionTag, float>("GetBrainPower", "RealVirtualMagic_PluginScript", GetBrainPower, registry));

		registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, void, BSFixedString, UInt32, float>("SetValue", "RealVirtualMagic_PluginScript", SetValue, registry));

		registry->RegisterFunction(new NativeFunction1<StaticFunctionTag, float, BSFixedString>("GetValue", "RealVirtualMagic_PluginScript", GetValue, registry));

		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, float, BSFixedString>("GetSettingDefault", "RealVirtualMagic_PluginScript", GetSettingDefault, registry));
		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, float, BSFixedString>("GetSettingMin", "RealVirtualMagic_PluginScript", GetSettingMin, registry));
		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, float, BSFixedString>("GetSettingMax", "RealVirtualMagic_PluginScript", GetSettingMax, registry));
		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, float, BSFixedString>("GetSettingInterval", "RealVirtualMagic_PluginScript", GetSettingInterval, registry));

		LOG("RealVirtualMagic registerFunction");
		return true;
	}

	void StartCast()
	{
		WriteEventMarker("magicCast:start");

		if (useBCI.get()->boolValue() && IXRInitialized && damageHealth.get()->boolValue() && latestBrainPower < unstableMagicThreshold.get()->value)
		{
			float healthdamage = GetMaxHealth() * unstableMagicDamage.get()->value / 100;

			if (damageHealth.get()->boolValue())
			{
				if (healthSpell)
				{
					ChangeSpellEffects(healthSpell, HealthDecEffect, HealthDecEffect, healthdamage);
					if (healthSpell->effectItemList.count > 0)
					{
						MagicItem::EffectItem* effectItem = nullptr;;
						if (healthSpell->effectItemList.GetNthItem(0, effectItem))
						{
							if (effectItem)
							{
								effectItem->magnitude = healthdamage;
							}
						}
					}

					const auto playerRefr = DYNAMIC_CAST((*g_thePlayer), Actor, TESObjectREFR);
					if (playerRefr)
					{
						LOG("Applying health damage: %g", healthdamage);
						g_task->AddTask(new taskDoCombatSpellApply(healthSpell, (Actor*)(*g_thePlayer), GetOrCreateRefrHandle(playerRefr)));
					}
				}
			}
		}
	}

	void EndCast()
	{
		WriteEventMarker("magicCast:stop");
	}

	void ApplyFocusValue(double newFocus)
	{
		if ((*g_thePlayer) != nullptr && (*g_thePlayer)->loadedState != nullptr && !isGameStoppedNoDialogue()) //Player is alive and Menu is not open
		{
			LOG("applying focus value of %g", newFocus);
			const bool disableAllEffects = (outOfCombatActive.get()->boolValue() == false && isInCombat == false) || newFocus < -900.0 || configSavingMode.load();
			LOG("disableAllEffects: %d", disableAllEffects);
			if (modifyMagicPower.get()->boolValue() && !disableAllEffects)
			{
				const float newMagicPowerMagnitude = minSpellpower.get()->value + newFocus * (maxSpellpower.get()->value - minSpellpower.get()->value);

				UpdatePlayerSpellEffects(alterationPowerSpell, alterationPowerIncEffect, alterationPowerDecEffect, newMagicPowerMagnitude, (maxSpellpower.get()->value - minSpellpower.get()->value) * minimumUpdatePercent.get()->value);
				UpdatePlayerSpellEffects(conjurationPowerSpell, conjurationPowerIncEffect, conjurationPowerDecEffect, newMagicPowerMagnitude, (maxSpellpower.get()->value - minSpellpower.get()->value) * minimumUpdatePercent.get()->value);
				UpdatePlayerSpellEffects(destructionPowerSpell, destructionPowerIncEffect, destructionPowerDecEffect, newMagicPowerMagnitude, (maxSpellpower.get()->value - minSpellpower.get()->value) * minimumUpdatePercent.get()->value);
				UpdatePlayerSpellEffects(illusionPowerSpell, illusionPowerIncEffect, illusionPowerDecEffect, newMagicPowerMagnitude, (maxSpellpower.get()->value - minSpellpower.get()->value) * minimumUpdatePercent.get()->value);
				UpdatePlayerSpellEffects(restorationPowerSpell, restorationPowerIncEffect, restorationPowerDecEffect, newMagicPowerMagnitude, (maxSpellpower.get()->value - minSpellpower.get()->value) * minimumUpdatePercent.get()->value);
			}
			else
			{
				if (DoesPlayerHaveEffects(alterationPowerDecEffect, alterationPowerIncEffect))
				{
					g_task->AddTask(new taskRemoveSpell(alterationPowerSpell));
				}

				if (DoesPlayerHaveEffects(conjurationPowerDecEffect, conjurationPowerIncEffect))
				{
					g_task->AddTask(new taskRemoveSpell(conjurationPowerSpell));
				}

				if (DoesPlayerHaveEffects(destructionPowerDecEffect, destructionPowerIncEffect))
				{
					g_task->AddTask(new taskRemoveSpell(destructionPowerSpell));
				}

				if (DoesPlayerHaveEffects(illusionPowerDecEffect, illusionPowerIncEffect))
				{
					g_task->AddTask(new taskRemoveSpell(illusionPowerSpell));
				}

				if (DoesPlayerHaveEffects(restorationPowerDecEffect, restorationPowerIncEffect))
				{
					g_task->AddTask(new taskRemoveSpell(restorationPowerSpell));
				}
			}

			if (modifyMagickaRate.get()->boolValue() && !disableAllEffects)
			{
				float newMagickaRate = minMagickaRate.get()->value + newFocus * (maxMagickaRate.get()->value - minMagickaRate.get()->value);

				UpdatePlayerSpellEffects(magickaRateSpell, magickaRateIncEffect, magickaRateDecEffect, newMagickaRate, (maxMagickaRate.get()->value - minMagickaRate.get()->value) * minimumUpdatePercent.get()->value);

				if (newMagickaRate < 0)
				{
					if (modifyMagicka.get()->boolValue())
					{
						if (magickaSpell)
						{
							const float curMagicka = GetCurrentMagicka();
							const float maxmagicka = GetMaxMagicka();
							const float damagePercent = abs(newMagickaRate)/ 100.0f; 
							const float magickadamage = maxmagicka * damagePercent;

							//LOG_ERR("Target magicka: %g Current magicka: %g", maxmagicka - magickadamage, curMagicka);
							if (curMagicka > maxmagicka - magickadamage)
							{
								ChangeSpellEffects(magickaSpell, magickaDecEffect, magickaDecEffect, magickadamage / 20.0f);

								const auto playerRefr = DYNAMIC_CAST((*g_thePlayer), Actor, TESObjectREFR);
								if (playerRefr)
								{
									LOG("Applying magicka damage: %g", magickadamage / 20.0f);
									g_task->AddTask(new taskDoCombatSpellApply(magickaSpell, (Actor*)(*g_thePlayer), GetOrCreateRefrHandle(playerRefr)));
								}
							}
							//else
								//LOG_ERR("No need to apply magicka damage.");
						}
					}
				}
			}
			else
			{
				//if (HasSpell((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), magickaRateSpell))
				if (DoesPlayerHaveEffects(magickaRateIncEffect, magickaRateDecEffect))
				{
					g_task->AddTask(new taskRemoveSpell(magickaRateSpell));
				}
			}

			if (modifyShoutRecovery.get()->boolValue() && !disableAllEffects)
			{
				const float newShoutRecoveryMagnitude = minShoutRecovery.get()->value + newFocus * (maxShoutRecovery.get()->value - minShoutRecovery.get()->value);

				UpdatePlayerSpellEffects(shoutRecoverySpell, shoutRecoveryIncEffect, shoutRecoveryDecEffect, newShoutRecoveryMagnitude, (maxShoutRecovery.get()->value - minShoutRecovery.get()->value) * minimumUpdatePercent.get()->value);
			}
			else
			{
				if (DoesPlayerHaveEffects(shoutRecoveryDecEffect, shoutRecoveryIncEffect))
				{
					g_task->AddTask(new taskRemoveSpell(shoutRecoverySpell));
				}
			}

			if (activateShield.get()->boolValue() && newFocus >= shieldActivationFocus.get()->value && !disableAllEffects)
			{
				//if (!HasSpell((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), shieldSpell))
				if(!DoesPlayerHaveEffect(shieldEffect))
				{
					g_task->AddTask(new taskAddSpell(shieldSpell));
				}
			}
			else
			{
				//if (HasSpell((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), shieldSpell))
				if (DoesPlayerHaveEffect(shieldEffect))
				{
					g_task->AddTask(new taskRemoveSpell(shieldSpell));
				}
			}

			// scale runes
			//LOG("before scaling");
			if (strangeRunesModIndex != 9999)
			{
				//LOG("in scaling");

				BSFixedString runeNodeName("RotNode");
				BSFixedString subRuneNodeName("Rune");
				BSFixedString leftHandNodeName("NPC L Hand [LHnd]");
				BSFixedString rightHandNodeName("NPC R Hand [RHnd]");

				float newScale = 0.5f + newFocus;
				//LOG("new scale: %g", newScale);

				NiNode* rootNodeTP = (*g_thePlayer)->GetNiRootNode(0);

				NiNode* rootNodeFP = (*g_thePlayer)->GetNiRootNode(2);

				NiNode* mostInterestingRoot = (rootNodeFP != nullptr) ? rootNodeFP : rootNodeTP;

				if (mostInterestingRoot)
				{
					//LOG("In mostInterestingRoot");
					NiAVObject* lefthand_node = mostInterestingRoot->GetObjectByName(&leftHandNodeName.data);
					NiAVObject* righthand_node = mostInterestingRoot->GetObjectByName(&rightHandNodeName.data);
					if (lefthand_node != nullptr)
					{
						//LOG("In left hand");
						NiAVObject* leftRuneNode = lefthand_node->GetObjectByName(&runeNodeName.data);
						if (leftRuneNode != nullptr)
						{
							//LOG("in left RotNode");
							NiAVObject* leftRuneNodeSubRune = leftRuneNode->GetObjectByName(&subRuneNodeName.data);
							if (leftRuneNodeSubRune != nullptr)
							{
								LOG("Scaling left rune %g", newScale);
								leftRuneNodeSubRune->m_localTransform.scale = newScale;
							}
						}
					}
					if (righthand_node != nullptr)
					{
						//LOG("In right hand");
						NiAVObject* rightRuneNode = righthand_node->GetObjectByName(&runeNodeName.data);
						if (rightRuneNode != nullptr)
						{
							//LOG("in right RotNode");
							NiAVObject* rightRuneNodeSubRune = rightRuneNode->GetObjectByName(&subRuneNodeName.data);
							if (rightRuneNodeSubRune != nullptr)
							{
								LOG("Scaling right rune %g", newScale);
								rightRuneNodeSubRune->m_localTransform.scale = newScale;
							}
						}
					}
				}
			}

			// wait for a bit to make sure the new settings are applied before logging
			// Sleep(50);

			LOG("new destruction: %g", GetDestruction());
			LOG("new magicka rate: %g", GetMagickaRate());
		}
	}
}
