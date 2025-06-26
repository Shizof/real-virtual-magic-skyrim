#include "Helper.h"

#include "skse64/PapyrusFormList.cpp"
#include <skse64/PapyrusActor.cpp>

namespace RealVirtualMagic
{
	RelocAddr <_HasSpell> HasSpell(0x0984420);
	RelocAddr <_AddSpell> AddSpell(0x0984330);
	RelocAddr <_RemoveSpell> RemoveSpell(0x09841A0);
	RelocAddr<_DoCombatSpellApply> DoCombatSpellApply(0x00992240);

	RelocAddr<_DamageActorValue> DamageActorValue(0x09848B0);
	RelocAddr<_RestoreActorValue> RestoreActorValue(0x0986480);
	UInt32 strangeRunesModIndex = 9999;
	std::string strangeRunesName = "StrangeRunes.esp";
	RelocPtr<AIProcessManager*> g_AIProcessManager(0x1F831B0);
	
	RelocAddr<_PlayEffectShader> PlayEffectShader(0x2AE290);

	std::string pluginName = "RealVirtualMagic.esp";

	UInt32 magickaSpellFormId = 0x0814;
	UInt32 magickaRateSpellFormId = 0x0815;
	UInt32 healthSpellFormId = 0x0816;
	UInt32 healRateSpellFormId = 0x0817;
	UInt32 shieldSpellFormId = 0x0818;

	UInt32 shieldPerk100FormId = 0x0D85;
	UInt32 shieldPerk75FormId = 0x0D86;
	UInt32 shieldPerk50FormId = 0x0D87;
	UInt32 shieldPerk25FormId = 0x0D88;

	SpellItem* magickaSpell;
	SpellItem* magickaRateSpell;
	SpellItem* healthSpell;
	SpellItem* healRateSpell;
	SpellItem* shieldSpell;

	BGSPerk* shieldPerk100;
	BGSPerk* shieldPerk75;
	BGSPerk* shieldPerk50;
	BGSPerk* shieldPerk25;


	UInt32 alterationPowerFormId = 0x0819;
	UInt32 conjurationPowerFormId = 0x081A;
	UInt32 destructionPowerFormId = 0x081B;
	UInt32 illusionPowerFormId = 0x081C;
	UInt32 restorationPowerFormId = 0x081D;

	UInt32 shoutRecoveryFormId = 0x0D83;


	SpellItem* alterationPowerSpell;
	SpellItem* conjurationPowerSpell;
	SpellItem* destructionPowerSpell;
	SpellItem* illusionPowerSpell;
	SpellItem* restorationPowerSpell;

	SpellItem* shoutRecoverySpell;


	UInt32 magickaDecEffectFormId = 0x0802;
	UInt32 magickaIncEffectFormId = 0x0803;
	UInt32 magickaRateDecEffectFormId = 0x0801;
	UInt32 magickaRateIncEffectFormId = 0x0804;
	UInt32 HealthDecEffectFormId = 0x0805;
	UInt32 HealthIncEffectFormId = 0x0806;
	UInt32 HealRateDecEffectFormId = 0x0807;
	UInt32 HealRateIncEffectFormId = 0x0808;
	UInt32 shieldEffectFormId = 0x0809;

	EffectSetting* magickaDecEffect;
	EffectSetting* magickaIncEffect;
	EffectSetting* magickaRateDecEffect;
	EffectSetting* magickaRateIncEffect;
	EffectSetting* HealthDecEffect;
	EffectSetting* HealthIncEffect;
	EffectSetting* HealRateDecEffect;
	EffectSetting* HealRateIncEffect;
	EffectSetting* shieldEffect;


	UInt32 alterationPowerIncEffectFormId = 0x0080A;
	UInt32 alterationPowerDecEffectFormId = 0x0080B;
	UInt32 conjurationPowerIncEffectFormId = 0x0080C;
	UInt32 conjurationPowerDecEffectFormId = 0x0080D;
	UInt32 destructionPowerIncEffectFormId = 0x0080E;
	UInt32 destructionPowerDecEffectFormId = 0x0080F;
	UInt32 illusionPowerIncEffectFormId = 0x00810;
	UInt32 illusionPowerDecEffectFormId = 0x00811;
	UInt32 restorationPowerIncEffectFormId = 0x00812;
	UInt32 restorationPowerDecEffectFormId = 0x00813;

	UInt32 shoutRecoveryIncEffectFormId = 0x00D81;
	UInt32 shoutRecoveryDecEffectFormId = 0x00D82;

	EffectSetting* alterationPowerIncEffect;
	EffectSetting* alterationPowerDecEffect;
	EffectSetting* conjurationPowerIncEffect;
	EffectSetting* conjurationPowerDecEffect;
	EffectSetting* destructionPowerIncEffect;
	EffectSetting* destructionPowerDecEffect;
	EffectSetting* illusionPowerIncEffect;
	EffectSetting* illusionPowerDecEffect;
	EffectSetting* restorationPowerIncEffect;
	EffectSetting* restorationPowerDecEffect;

	EffectSetting* shoutRecoveryIncEffect;
	EffectSetting* shoutRecoveryDecEffect;


	UInt32 shieldEffectShaderFormId = 0x00081E;
	TESEffectShader* shieldEffectShader;


	BGSPerk* chosenShieldPerk;

	void FillSpellWithFormId(UInt32 baseFormId, SpellItem** targetSpell)
	{
		const UInt32 fullFormId = GetFullFormIdFromEspAndFormId(pluginName.c_str(), GetBaseFormID(baseFormId));
		if (fullFormId > 0)
		{
			TESForm* form = LookupFormByID(fullFormId);
			if (form)
			{
				*targetSpell = DYNAMIC_CAST(form, TESForm, SpellItem);
				if (targetSpell)
					LOG_ERR("Spell found. formid: %x", fullFormId);
				else
					LOG_ERR("Spell null. formid: %x", fullFormId);

			}
			else
			{
				LOG_ERR("Spell not found. formid: %x", fullFormId);
			}
		}
	}


	void FillPerkWithFormId(UInt32 baseFormId, BGSPerk** targetPerk)
	{
		const UInt32 fullFormId = GetFullFormIdFromEspAndFormId(pluginName.c_str(), GetBaseFormID(baseFormId));
		if (fullFormId > 0)
		{
			TESForm* form = LookupFormByID(fullFormId);
			if (form)
			{
				*targetPerk = DYNAMIC_CAST(form, TESForm, BGSPerk);
				if (targetPerk)
					LOG_ERR("Perk found. formid: %x", fullFormId);
				else
					LOG_ERR("Perk null. formid: %x", fullFormId);

			}
			else
			{
				LOG_ERR("Perk not found. formid: %x", fullFormId);
			}
		}
	}

	void FillMagicEffectWithFormId(UInt32 baseFormId, EffectSetting** targetEffect)
	{
		const UInt32 fullFormId = GetFullFormIdFromEspAndFormId(pluginName.c_str(), GetBaseFormID(baseFormId));
		if (fullFormId > 0)
		{
			TESForm* form = LookupFormByID(fullFormId);
			if (form)
			{
				*targetEffect = DYNAMIC_CAST(form, TESForm, EffectSetting);
			}
			else
			{
				LOG_ERR("Magicka Spell not found. formid: %x", fullFormId);
			}
		}
	}

	void FillEffectShaderWithFormId(UInt32 baseFormId, TESEffectShader** effectShader)
	{
		const UInt32 fullFormId = GetFullFormIdFromEspAndFormId(pluginName.c_str(), GetBaseFormID(baseFormId));
		if (fullFormId > 0)
		{
			TESForm* form = LookupFormByID(fullFormId);
			if (form)
			{
				*effectShader = DYNAMIC_CAST(form, TESForm, TESEffectShader);
			}
			else
			{
				LOG_ERR("Effect shader not found. formid: %x", fullFormId);
			}
		}
	}

	void LoadValues()
	{
		DataHandler* dataHandler = DataHandler::GetSingleton();

		if (dataHandler)
		{
			const ModInfo* strangeRunesModInfo = dataHandler->LookupLoadedModByName(strangeRunesName.c_str());

			if (strangeRunesModInfo)
			{
				if (IsValidModIndex(strangeRunesModInfo->modIndex))
				{
					strangeRunesModIndex = strangeRunesModInfo->modIndex;
					LOG_ERR("Strange Runes esp found. Mod Index: %x", strangeRunesModIndex);
				}
				else
				{
					LOG_ERR("Strange runes mod not found!");
				}
			}

			FillSpellWithFormId(magickaSpellFormId, &magickaSpell);
			FillSpellWithFormId(magickaRateSpellFormId, &magickaRateSpell);
			FillSpellWithFormId(healthSpellFormId, &healthSpell);
			FillSpellWithFormId(healRateSpellFormId, &healRateSpell);
			FillSpellWithFormId(shieldSpellFormId, &shieldSpell);

			FillPerkWithFormId(shieldPerk100FormId, &shieldPerk100);
			FillPerkWithFormId(shieldPerk75FormId, &shieldPerk75);
			FillPerkWithFormId(shieldPerk50FormId, &shieldPerk50);
			FillPerkWithFormId(shieldPerk25FormId, &shieldPerk25);

			FillSpellWithFormId(alterationPowerFormId, &alterationPowerSpell);
			FillSpellWithFormId(conjurationPowerFormId, &conjurationPowerSpell);
			FillSpellWithFormId(destructionPowerFormId, &destructionPowerSpell);
			FillSpellWithFormId(illusionPowerFormId, &illusionPowerSpell);
			FillSpellWithFormId(restorationPowerFormId, &restorationPowerSpell);
			FillSpellWithFormId(shoutRecoveryFormId, &shoutRecoverySpell);

			FillMagicEffectWithFormId(magickaDecEffectFormId, &magickaDecEffect);
			FillMagicEffectWithFormId(magickaIncEffectFormId,& magickaIncEffect);
			FillMagicEffectWithFormId(magickaRateDecEffectFormId, &magickaRateDecEffect);
			FillMagicEffectWithFormId(magickaRateIncEffectFormId, &magickaRateIncEffect);
			FillMagicEffectWithFormId(HealthDecEffectFormId, &HealthDecEffect);
			FillMagicEffectWithFormId(HealthIncEffectFormId, &HealthIncEffect);
			FillMagicEffectWithFormId(HealRateDecEffectFormId, &HealRateDecEffect);
			FillMagicEffectWithFormId(HealRateIncEffectFormId, &HealRateIncEffect);
			FillMagicEffectWithFormId(shieldEffectFormId, &shieldEffect);

			FillMagicEffectWithFormId(alterationPowerIncEffectFormId, &alterationPowerIncEffect);
			FillMagicEffectWithFormId(alterationPowerDecEffectFormId, &alterationPowerDecEffect);
			FillMagicEffectWithFormId(conjurationPowerIncEffectFormId, &conjurationPowerIncEffect);
			FillMagicEffectWithFormId(conjurationPowerDecEffectFormId, &conjurationPowerDecEffect);
			FillMagicEffectWithFormId(destructionPowerIncEffectFormId, &destructionPowerIncEffect);
			FillMagicEffectWithFormId(destructionPowerDecEffectFormId, &destructionPowerDecEffect);
			FillMagicEffectWithFormId(illusionPowerIncEffectFormId, &illusionPowerIncEffect);
			FillMagicEffectWithFormId(illusionPowerDecEffectFormId, &illusionPowerDecEffect);
			FillMagicEffectWithFormId(restorationPowerIncEffectFormId, &restorationPowerIncEffect);
			FillMagicEffectWithFormId(restorationPowerDecEffectFormId, &restorationPowerDecEffect);
			FillMagicEffectWithFormId(shoutRecoveryIncEffectFormId, &shoutRecoveryIncEffect);
			FillMagicEffectWithFormId(shoutRecoveryDecEffectFormId, &shoutRecoveryDecEffect);

			FillEffectShaderWithFormId(shieldEffectShaderFormId, &shieldEffectShader);
		}
	}


	bool ActorInCombat(Actor* actor)
	{
		UInt64* vtbl = *((UInt64**)actor);
		const bool combat = ((_IsInCombatNative)(vtbl[0xE5]))(actor);
		return combat;
	}
	
	float GetCurrentMagicka()
	{
		return (*g_thePlayer)->actorValueOwner.GetCurrent(25);
	}

	float GetBaseMaxMagicka()
	{
		float totalMod = 0.0f;
		float permanent = (*g_thePlayer)->magickaModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kPermanent];
		totalMod += (*g_thePlayer)->magickaModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kTemporary];
		
		const float currentMagicka = GetCurrentMagicka();

		const float actorValuePercentage = CALL_MEMBER_FN((*g_thePlayer), GetActorValuePercentage)(25);
		
		const float maxMagicka = ceilf(currentMagicka / actorValuePercentage);
		//LOG_ERR("BaseMaxMagicka (%g/%g): %g = %g + %g - %g", currentMagicka, actorValuePercentage, maxMagicka + totalMod - permanent, maxMagicka, totalMod, permanent);
		return (maxMagicka + totalMod - floor(permanent));
	}

	float GetMaxMagicka()
	{
		float totalMod = 0.0f;
		//totalMod += (*g_thePlayer)->magickaModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kPermanent]; //already included in max magicka
		totalMod += (*g_thePlayer)->magickaModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kTemporary];
		//LOG_ERR("Magicka Modifiers: Permanent: %g - Temp: %g - Damage: %g", (*g_thePlayer)->magickaModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kPermanent], (*g_thePlayer)->magickaModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kTemporary], (*g_thePlayer)->magickaModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kDamage]);
		const float currentMagicka = GetCurrentMagicka();
		const float maxMagicka = ceilf(currentMagicka / CALL_MEMBER_FN((*g_thePlayer), GetActorValuePercentage)(25));
		//LOG_ERR("Current Magicka: %g MaxMagicka: %g - CalculatedMax: %g", currentMagicka, maxMagicka, maxMagicka + totalMod);

		return (maxMagicka + totalMod);
	}

	void SetMaxMagicka(float new_current_magicka)
	{
		(*g_thePlayer)->actorValueOwner.SetCurrent(25, new_current_magicka);
	}

	void ChangeCurrentMagicka(float amount)
	{
		LOG("Changing Magicka by %g", amount);
		//(*g_thePlayer)->actorValueOwner.RestoreActorValue((amount < 0) ? 2 : 4, 26, abs(amount));

		std::thread t3(ChangeCurrentMagickaFunc, amount);
		t3.detach();
		//ChangeCurrentMagickaFunc(amount);
	}

	void ChangeCurrentMagickaFunc(float amount)
	{
		if (amount < 0)
		{
			DamageActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Magicka", abs(amount));
			Sleep(1500);
			const float currentMagicka = GetCurrentMagicka();
			if (currentMagicka < 0)
			{
				RestoreActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Magicka", 10 - currentMagicka);
			}
		}
		else
		{
			//Sleep(500);
			RestoreActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Magicka", abs(amount));
		}
	}

	float GetCurrentHealth()
	{
		return (*g_thePlayer)->actorValueOwner.GetCurrent(24);
	}

	float GetMaxHealth()
	{
		float totalMod = 0.0f;
		//totalMod += (*g_thePlayer)->healthModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kPermanent]; //already included in max health
		totalMod += (*g_thePlayer)->healthModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kTemporary];

		//LOG_ERR("Health Modifiers: Permanent: %g - Temp: %g - Damage: %g", (*g_thePlayer)->healthModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kPermanent], (*g_thePlayer)->healthModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kTemporary], (*g_thePlayer)->healthModifiers.modifiers[Actor::ACTOR_VALUE_MODIFIER::kDamage]);
		const float currentHealth = GetCurrentHealth();
		const float maxHealth = ceilf(currentHealth / CALL_MEMBER_FN((*g_thePlayer), GetActorValuePercentage)(24));
		//LOG_ERR("Current Health: %g MaxHealth: %g - CalculatedMax: %g", currentHealth, maxHealth, maxHealth + totalMod);

		return (maxHealth + totalMod);
	}

	void SetMaxHealth(float new_current_magicka)
	{
		(*g_thePlayer)->actorValueOwner.SetCurrent(24, new_current_magicka);
	}

	void ChangeCurrentHealth(float amount)
	{
		LOG("Changing Health by %g", amount);
		//(*g_thePlayer)->actorValueOwner.RestoreActorValue((amount < 0) ? 2 : 4, 26, abs(amount));

		//std::thread t3(ChangeCurrentMagickaFunc, amount);
		//t3.detach();
		ChangeCurrentHealthFunc(amount);
	}

	void ChangeCurrentHealthFunc(float amount)
	{
		if (amount < 0)
		{
			DamageActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Health", abs(amount));
			Sleep(1500);
			const float currentHealth = GetCurrentHealth();
			if (currentHealth < 0)
			{
				RestoreActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Health", 10 - currentHealth);
			}
		}
		else
		{
			//Sleep(500);
			RestoreActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Health", abs(amount));
		}
	}

	float GetMagickaRate()
	{
		return (*g_thePlayer)->actorValueOwner.GetCurrent(28);
	}
	
	float GetMagickaRateMult()
	{
		return (*g_thePlayer)->actorValueOwner.GetCurrent(156);
	}

	void SetMagickaRate(float new_current_magicka_rate)
	{
		(*g_thePlayer)->actorValueOwner.SetCurrent(28, new_current_magicka_rate);
	}

	// ------------------- destruction
	float GetDestruction()
	{
		return (*g_thePlayer)->actorValueOwner.GetCurrent(149);
	}

	void SetAllMagickPower(float new_power)
	{
		(*g_thePlayer)->actorValueOwner.SetCurrent(147, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(148, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(149, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(150, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(151, new_power);
		/*
			kAlterationPowerModifier = 147,
			kConjurationPowerModifier = 148,
			kDestructionPowerModifier = 149,
			kIllusionPowerModifier = 150,
			kRestorationPowerModifier = 151,
		*/
	}


	void SetAllDamageResist(float new_power)
	{
		(*g_thePlayer)->actorValueOwner.SetCurrent(39, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(40, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(41, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(42, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(43, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(44, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(45, new_power);
		/*
			kDamageResist = 39,
			kPoisonResist = 40,
			kResistFire = 41,
			kResistShock = 42,
			kResistFrost = 43,
			kResistMagic = 44,
			kResistDisease = 45,
		*/
	}


	taskRemoveAndAddSpell::taskRemoveAndAddSpell(SpellItem* akSpell)
	{
		m_akSpell = akSpell;
	}

	void taskRemoveAndAddSpell::Run()
	{
		RemoveSpell((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), m_akSpell);
		AddSpell((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), m_akSpell, false);
	}

	void taskRemoveAndAddSpell::Dispose()
	{
		delete this;
	}

	taskAddSpell::taskAddSpell(SpellItem* akSpell)
	{
		m_akSpell = akSpell;
	}

	void taskAddSpell::Run()
	{
		AddSpell((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), m_akSpell, false);
	}

	void taskAddSpell::Dispose()
	{
		delete this;
	}

	taskRemoveSpell::taskRemoveSpell(SpellItem* akSpell)
	{
		m_akSpell = akSpell;
	}

	void taskRemoveSpell::Run()
	{
		RemoveSpell((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), m_akSpell);
	}

	void taskRemoveSpell::Dispose()
	{
		delete this;
	}

	taskRemoveAllShieldPerks::taskRemoveAllShieldPerks()
	{
	}

	void taskRemoveAllShieldPerks::Run()
	{
		if (DoesPlayerHavePerk(shieldPerk100))
			(*g_thePlayer)->RemovePerk(shieldPerk100);
		if (DoesPlayerHavePerk(shieldPerk75))
			(*g_thePlayer)->RemovePerk(shieldPerk75);
		if (DoesPlayerHavePerk(shieldPerk50))
			(*g_thePlayer)->RemovePerk(shieldPerk50);
		if (DoesPlayerHavePerk(shieldPerk25))
			(*g_thePlayer)->RemovePerk(shieldPerk25);
	}

	void taskRemoveAllShieldPerks::Dispose()
	{
		delete this;
	}



	taskAddPerk::taskAddPerk(BGSPerk* akPerk)
	{
		m_akPerk = akPerk;
	}

	void taskAddPerk::Run()
	{
		(*g_thePlayer)->AddPerk(m_akPerk, 1);
	}

	void taskAddPerk::Dispose()
	{
		delete this;
	}

	taskRemovePerk::taskRemovePerk(BGSPerk* akPerk)
	{
		m_akPerk = akPerk;
	}

	void taskRemovePerk::Run()
	{
		(*g_thePlayer)->RemovePerk(m_akPerk);
	}

	void taskRemovePerk::Dispose()
	{
		delete this;
	}

	taskDoCombatSpellApply::taskDoCombatSpellApply(SpellItem* spell, Actor* actor, UInt32 targetRefHandle)
	{
		m_spell = spell;
		m_targetRefHandle = targetRefHandle;
		m_actor = actor;
	}

	void taskDoCombatSpellApply::Run()
	{
		NiPointer<TESObjectREFR> targetRefr;
		LookupREFRByHandle(m_targetRefHandle, targetRefr);

		if (targetRefr && targetRefr->formID != 0)
		{
			DoCombatSpellApply((*g_skyrimVM)->GetClassRegistry(), 0, m_actor, m_spell, targetRefr);
		}
	}

	void taskDoCombatSpellApply::Dispose()
	{
		delete this;
	}
	

	bool DoesPlayerHaveEffect(EffectSetting* effect)
	{
		bool foundEffect = false;
		MagicTarget* magicTarget = &((*g_thePlayer)->magicTarget);
		magicTarget->VisitActiveEffects([&foundEffect, &effect](ActiveEffect* activeEffect) -> BSContainer::ForEachResult {
			EffectSetting* setting = (activeEffect && activeEffect->effect) ? activeEffect->effect->mgef : nullptr;
			if (setting != nullptr && setting == effect)
			{
				foundEffect = true;
				return BSContainer::ForEachResult::kAbort;
			}

			return BSContainer::ForEachResult::kContinue;
			});

		if (foundEffect)
			return true;
		else
			return false;
	}

	bool DoesPlayerHaveEffects(EffectSetting* effect1, EffectSetting* effect2)
	{
		bool foundEffect = false;
		MagicTarget* magicTarget = &((*g_thePlayer)->magicTarget);
		magicTarget->VisitActiveEffects([&foundEffect, &effect1, &effect2](ActiveEffect* activeEffect) -> BSContainer::ForEachResult {
			EffectSetting* setting = (activeEffect && activeEffect->effect) ? activeEffect->effect->mgef : nullptr;
			if (setting != nullptr && (setting == effect1 || setting == effect2))
			{
				foundEffect = true;
				return BSContainer::ForEachResult::kAbort;
			}

			return BSContainer::ForEachResult::kContinue;
			});

		if (foundEffect)
			return true;
		else
			return false;
	}

	bool CheckIfPlayerHaveEffectsAndGetCurrentMagnitude(EffectSetting* decEffect, EffectSetting* incEffect, float &currentMag)
	{
		bool foundEffect = false;

		MagicTarget* magicTarget = &((*g_thePlayer)->magicTarget);
		magicTarget->VisitActiveEffects([&foundEffect, &decEffect, &incEffect, &currentMag](ActiveEffect* activeEffect) -> BSContainer::ForEachResult {
			EffectSetting* setting = (activeEffect && activeEffect->effect) ? activeEffect->effect->mgef : nullptr;
			if (setting != nullptr && (setting == decEffect || setting == incEffect))
			{
				currentMag = ((setting == decEffect) ? -1 : 1) * activeEffect->effect->magnitude;
				//LOG("Player already has this effect: %s. Current magnitude is %g", activeEffect->effect->mgef->fullName.name.data, currentMag);
				
				foundEffect = true;
				return BSContainer::ForEachResult::kAbort;
			}

			return BSContainer::ForEachResult::kContinue;
			});

		if (foundEffect)
			return true;
		else
			return false;
	}

	void ChangeSpellEffects(SpellItem * spell, EffectSetting* incEffect, EffectSetting* decEffect, float newMagnitude)
	{
		if (spell && spell->effectItemList.count > 0)
		{
			MagicItem::EffectItem* effectItem = nullptr;;
			if (spell->effectItemList.GetNthItem(0, effectItem))
			{
				if (effectItem)
				{
					if (newMagnitude > 0)
						effectItem->mgef = incEffect;
					else
						effectItem->mgef = decEffect;

					effectItem->magnitude = abs(newMagnitude);
				}
			}
		}
	}

	bool DoesPlayerHavePerk(BGSPerk* perk)
	{
		return CALL_MEMBER_FN((*g_thePlayer), HasPerk)(perk);
	}

	bool DoesPlayerHaveAnyShieldPerk(BGSPerk* perk)
	{
		return CALL_MEMBER_FN((*g_thePlayer), HasPerk)(perk);
	}

	BGSPerk* WhichShieldPerkToUse()
	{
		float percentage = shieldPercentage.get()->value;

		if (percentage == 0) 
		{
			return nullptr;
		}

		if (percentage >= 87.5f) 
		{  
			return shieldPerk100;
		}
		else if (percentage >= 62.5f) 
		{  
			return shieldPerk75;
		}
		else if (percentage >= 37.5f) 
		{  
			return shieldPerk50;
		}
		else 
		{
			return shieldPerk25;
		}
	}

	void AddOrRemoveSpells(SpellItem* spell, bool addOnly)
	{
		if (!addOnly)
		{
			g_task->AddTask(new taskRemoveSpell(spell));
			Sleep(500);
			g_task->AddTask(new taskAddSpell(spell));
		}
		else
			g_task->AddTask(new taskAddSpell(spell));
	}

	void UpdatePlayerSpellEffects(SpellItem* spell, EffectSetting* incEffect, EffectSetting* decEffect, float newMagnitude, float minChangeAmount)
	{
		if (spell && decEffect && incEffect)
		{
			float curMagnitude = 0.0f;
			const bool playerHaveEffects = CheckIfPlayerHaveEffectsAndGetCurrentMagnitude(decEffect, incEffect, curMagnitude);

			//Only change if a significant change occurs or if the player doesn't have the spell.
			if (!playerHaveEffects || (abs(newMagnitude - curMagnitude) >= minChangeAmount))
			{
				ChangeSpellEffects(spell, incEffect, decEffect, newMagnitude);

				std::thread t2(AddOrRemoveSpells, spell, !playerHaveEffects);
				t2.detach();
			}
		}
	}

	// Fetches information about a node
	std::string getNodeDesc(NiAVObject* node)
	{
		// Use periods to indicate the depth in the scene graph
		if (!node) {
			return "Missing node";
		}

		// Include the node's type followed by its name
		std::string text;
		if (node->GetRTTI() && node->GetRTTI()->name)
			text = text + node->GetRTTI()->name + " ";
		else
			text = text + "UnknownType ";
		if (node->m_name)
			text = text + node->m_name;
		else
			text = text + "Unnamed";

		// Include any extra data
		if (node->m_extraDataLen > 0)
			text = text + " {";
		for (int i = 0; i < node->m_extraDataLen; ++i) {
			// Fetch extra data, writing "NULL" for missing elements
			NiExtraData* extraData = node->m_extraData[i];
			if (i > 0)
				text = text + ", ";
			if (!extraData) {
				text = text + "Missing Data";
				continue;
			}
			else if (!extraData->m_pcName) {
				text = text + "(null)=";
			}
			else {
				text = text + extraData->m_pcName + "=";
			}

			// Cast the extra data to all supported types
			NiStringExtraData* stringData = DYNAMIC_CAST(extraData, NiExtraData, NiStringExtraData);
			NiStringsExtraData* stringsData = DYNAMIC_CAST(extraData, NiExtraData, NiStringsExtraData);
			NiBooleanExtraData* boolData = DYNAMIC_CAST(extraData, NiExtraData, NiBooleanExtraData);
			NiIntegerExtraData* intData = DYNAMIC_CAST(extraData, NiExtraData, NiIntegerExtraData);
			NiIntegersExtraData* intsData = DYNAMIC_CAST(extraData, NiExtraData, NiIntegersExtraData);
			NiFloatExtraData* floatData = DYNAMIC_CAST(extraData, NiExtraData, NiFloatExtraData);
			NiFloatsExtraData* floatsData = DYNAMIC_CAST(extraData, NiExtraData, NiFloatsExtraData);
			NiVectorExtraData* vectorData = DYNAMIC_CAST(extraData, NiExtraData, NiVectorExtraData);

			// Include NiStringExtraData
			if (stringData) {
				text = text + "\"" + stringData->m_pString + "\"";

				// Include NiStringsExtraData as an array
			}
			else if (stringsData) {
				text = text + "[";
				if (!stringsData->m_data) {
					text = text + "(null string data)";
				}
				else {
					for (int j = 0; j < stringsData->m_size; ++j) {
						if (j > 0)
							text = text + ", ";
						text = text + "\"" + stringsData->m_data[j] + "\"";
					}
				}
				text = text + "]";

				// Include NiBooleanExtraData
			}
			else if (boolData) {
				text = text + (boolData->m_data ? "True" : "False");

				// Include NiIntegerExtraData
			}
			else if (intData) {
				text = text + std::to_string(intData->m_data);

				// Include NiIntegersExtraData as an array
			}
			else if (intsData) {
				text = text + "[";
				if (!intsData->m_data) {
					text = text + "(null int data)";
				}
				else {
					for (int j = 0; j < intsData->m_size; ++j) {
						if (j > 0)
							text = text + ", ";
						text = text + std::to_string(intsData->m_data[j]);
					}
				}
				text = text + "]";

				// Include NiFloatExtraData
			}
			else if (floatData) {
				text = text + std::to_string(floatData->m_data);

				// Include NiFloatsExtraData as an array
			}
			else if (floatsData) {
				text = text + "[";
				if (!floatsData->m_data) {
					text = text + "(null float data)";
				}
				else {
					for (int j = 0; j < floatsData->m_size; ++j) {
						if (j > 0)
							text = text + ", ";
						text = text + std::to_string(floatsData->m_data[j]);
					}
				}
				text = text + "]";

				// Include NiVectorExtraData
			}
			else if (vectorData) {
				text = text + "<" + std::to_string(vectorData->m_vector[0]) +
					std::to_string(vectorData->m_vector[1]) +
					std::to_string(vectorData->m_vector[2]) +
					std::to_string(vectorData->m_vector[3]) + +">";

				// Include BSBehaviorGraphExtraData
			}
			else {
				text = text + extraData->GetRTTI()->name;
			}

		}
		if (node->m_extraDataLen > 0)
			text = text + "}";
		return text;
	}

	// Writes information about a node to the log file
	void logNode(int depth, NiAVObject* node)
	{
		auto text = std::string(depth, '.') + getNodeDesc(node);
		_MESSAGE("%d: %s", depth, text.c_str());
	}

	// Lists all parents of a bone to the log file
	void logParents(NiAVObject* bone)
	{
		NiNode* node = bone ? bone->GetAsNiNode() : 0;
		int depth = 1;
		while (node) {
			//auto blanks = std::string(depth, '.');
			//log("%sNode name = %s, RTTI = %s\n", blanks.c_str(), node->m_name, node->GetRTTI()->name);
			logNode(depth, node);
			node = node->m_parent;
			++depth;
		}
	}

	// Lists all children of a bone to the log file, filtering by RTTI type name
	void logChildren(NiAVObject* bone, int depth, int maxDepth, const char* filter)
	{
		if (!bone)
			return;

		if (filter != 0 && filter[0])
		{
			if (strcmp(bone->GetRTTI()->name, filter) != 0)
			{
				return;
			}
		}
		//auto blanks = std::string(depth, '.');
		//log("%sNode name = %s, RTTI = %s\n", blanks.c_str(), bone->m_name, bone->GetRTTI()->name);
		logNode(depth, bone);
		NiNode* node = bone ? bone->GetAsNiNode() : 0;
		if (!node)
			return;

		auto children = NINODE_CHILDREN(node);
		if (children)
		{
			//log("Children has %d %d %d %d\n", children->m_arrayBufLen, children->m_emptyRunStart, children->m_size, children->m_growSize);
			for (int i = 0; i < children->m_emptyRunStart; i++)
			{
				if (depth < maxDepth || maxDepth < 0)
				{
					logChildren(children->m_data[i], depth + 1, maxDepth, filter);
				}
			}
		}
	}

}