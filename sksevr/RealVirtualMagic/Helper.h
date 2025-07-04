#pragma once


#include "RealVirtualMagic.h"

#define NINODE_CHILDREN(ninode) ((NiTArray <NiAVObject *> *) ((char*)(&((ninode)->m_children))))

namespace RealVirtualMagic
{
	extern SpellItem* magickaSpell;
	extern SpellItem* magickaRateSpell;
	extern SpellItem* healthSpell;
	extern SpellItem* healRateSpell;
	extern SpellItem* shieldSpell;

	extern BGSPerk* shieldPerk100;
	extern BGSPerk* shieldPerk75;
	extern BGSPerk* shieldPerk50;
	extern BGSPerk* shieldPerk25;

	extern SpellItem* alterationPowerSpell;
	extern SpellItem* conjurationPowerSpell;
	extern SpellItem* destructionPowerSpell;
	extern SpellItem* illusionPowerSpell;
	extern SpellItem* restorationPowerSpell;
	extern SpellItem* shoutRecoverySpell;

	extern EffectSetting* magickaDecEffect;
	extern EffectSetting* magickaIncEffect;
	extern EffectSetting* magickaRateDecEffect;
	extern EffectSetting* magickaRateIncEffect;
	extern EffectSetting* HealthDecEffect;
	extern EffectSetting* HealthIncEffect;
	extern EffectSetting* HealRateDecEffect;
	extern EffectSetting* HealRateIncEffect;
	extern EffectSetting* shieldEffect;

	extern EffectSetting* alterationPowerIncEffect;
	extern EffectSetting* alterationPowerDecEffect;
	extern EffectSetting* conjurationPowerIncEffect;
	extern EffectSetting* conjurationPowerDecEffect;
	extern EffectSetting* destructionPowerIncEffect;
	extern EffectSetting* destructionPowerDecEffect;
	extern EffectSetting* illusionPowerIncEffect;
	extern EffectSetting* illusionPowerDecEffect;
	extern EffectSetting* restorationPowerIncEffect;
	extern EffectSetting* restorationPowerDecEffect;

	extern EffectSetting* shoutRecoveryIncEffect;
	extern EffectSetting* shoutRecoveryDecEffect;

	extern TESEffectShader* shieldEffectShader;

	extern BGSPerk* chosenShieldPerk;
	
	

	typedef void(*_DamageActorValue)(VMClassRegistry* VMinternal, UInt32 stackId, Actor* thisActor, BSFixedString const& dmgValueName, float dmg);
	extern RelocAddr<_DamageActorValue> DamageActorValue; 
	typedef void(*_RestoreActorValue)(VMClassRegistry* VMinternal, UInt32 stackId, Actor* thisActor, BSFixedString const& dmgValueName, float amount);
	extern RelocAddr<_RestoreActorValue> RestoreActorValue;
	typedef bool(*_IsInCombatNative)(Actor* actor);


	typedef bool(*_HasSpell)(VMClassRegistry* registry, UInt64 stackID, Actor* actor, TESForm* akSpell);
	extern RelocAddr <_HasSpell> HasSpell;

	typedef bool(*_AddSpell)(VMClassRegistry* registry, UInt64 stackID, Actor* actor, TESForm* akSpell, bool abVerbose);
	extern RelocAddr <_AddSpell> AddSpell;

	typedef bool(*_RemoveSpell)(VMClassRegistry* registry, UInt64 stackID, Actor* actor, TESForm* akSpell);
	extern RelocAddr <_RemoveSpell> RemoveSpell;

	typedef bool(*_DoCombatSpellApply)(VMClassRegistry* registry, UInt32 stackId, Actor* akActor, SpellItem* spell, TESObjectREFR* akTarget);
	extern RelocAddr<_DoCombatSpellApply> DoCombatSpellApply;

	bool ActorInCombat(Actor* actor);

	float GetCurrentMagicka();
	float GetBaseMaxMagicka();
	float GetMaxMagicka();
	void SetMaxMagicka(float amount);
	void ChangeCurrentMagicka(float amount);
	void ChangeCurrentMagickaFunc(float amount);

	float GetCurrentHealth();
	float GetMaxHealth();
	void SetMaxHealth(float amount);
	void ChangeCurrentHealth(float amount);
	void ChangeCurrentHealthFunc(float amount);

	float GetMagickaRate();
	float GetMagickaRateMult();
	void SetMagickaRate(float amount);

	float GetDestruction();
	void SetAllMagickPower(float amount);

	void SetAllDamageResist(float amount);

	bool DoesPlayerHaveEffect(EffectSetting* effect);
	bool DoesPlayerHaveEffects(EffectSetting* effect1, EffectSetting* effect2);
	void ChangeSpellEffects(SpellItem* spell, EffectSetting* incEffect, EffectSetting* decEffect, float newMagnitude);
	bool CheckIfPlayerHaveEffectsAndGetCurrentMagnitude(EffectSetting* decEffect, EffectSetting* incEffect, float& currentMag);
	void UpdatePlayerSpellEffects(SpellItem* spell, EffectSetting* incEffect, EffectSetting* decEffect, float newMagnitude, float minChangeAmount);
	bool DoesPlayerHavePerk(BGSPerk* perk);
	BGSPerk* WhichShieldPerkToUse();

	void LoadValues();

	void logChildren(NiAVObject* bone, int depth, int maxDepth, const char* filter);

	extern UInt32 strangeRunesModIndex;

	struct BSEffectShaderData
	{
		inline UInt32 IncRef() { return InterlockedIncrement(&refCount); }
		inline UInt32 DecRef() { return InterlockedDecrement(&refCount); }

		UInt32 refCount; // 00

		// more...
	};

	struct OwnedController
	{
		void* vtbl; // 00
		UInt32 targetHandle; // 08
		UInt32 unk0C;
		TESEffectShader* shader; // 10
		BGSArtObject* artObject; // 18
		UInt32 aimAtHandle; // 20
		UInt32 unk24;
		NiPointer<NiAVObject> attachRoot; // 28
	};
	STATIC_ASSERT(sizeof(OwnedController) == 0x30);

	struct BSTempEffect : NiObject
	{
		float		   lifetime;	 // 10
		UInt32		   pad14;		 // 14
		TESObjectCELL* cell;		 // 18
		float		   age;			 // 20
		bool		   initialized;	 // 24
		UInt8		   pad25;		 // 25
		UInt16		   pad26;		 // 26
		UInt32		   effectID;	 // 28
		UInt32		   pad2C;		 // 2C
	};
	STATIC_ASSERT(sizeof(BSTempEffect) == 0x30);

	struct ReferenceEffect : BSTempEffect
	{
		OwnedController* controller;		// 30
		UInt32				target;			// 38
		UInt32				aimAtTarget;	// 3C
		bool				finished;		// 40
		bool				ownController;	// 41
		UInt16				pad42;			// 42
		UInt32				pad44;			// 44
	};
	STATIC_ASSERT(offsetof(ReferenceEffect, controller) == 0x30);
	STATIC_ASSERT(offsetof(ReferenceEffect, finished) == 0x40);
	STATIC_ASSERT(sizeof(ReferenceEffect) == 0x48);

	struct ShaderReferenceEffect : ReferenceEffect
	{
		UInt8 unkArraysAndSoundHandle[0xCC - 0x48]; // 48
		std::uint32_t		  pad0CC;			 // 0CC
		void* unk0D0;			 // 0D0 - smart ptr
		void* unk0D8;			 // 0D8 - smart ptr
		void* unk0E0;			 // 0E0 - smart ptr
		void* unk0E8;			 // 0E8 - smart ptr
		void* unk0F0;			 // 0F0 - smart ptr
		NiPointer<NiAVObject> lastRootNode;		 // 0F8
		TESBoundObject* wornObject;		 // 100
		TESEffectShader* effectData;		 // 108
		BSEffectShaderData* effectShaderData;	 // 110
		void* unk118;			 // 118 - smart ptr
		std::uint32_t		  unk120;			 // 120
		std::uint32_t		  unk124;			 // 124
		std::uint32_t		  unk128;			 // 128
		std::uint32_t		  unk12C;			 // 12C
		std::uint32_t		  flags;			 // 130
		std::uint32_t		  pushCount;		 // 134
	};
	STATIC_ASSERT(offsetof(ShaderReferenceEffect, pushCount) == 0x134);
	STATIC_ASSERT(sizeof(ShaderReferenceEffect) == 0x138);

	struct ModelReferenceEffect : ReferenceEffect
	{
		UInt8 otherBaseClassesAndHitEffectArtData[0xB0 - 0x48];  // 48
		std::uint64_t			unkB0;			   // B0
		BGSArtObject* artObject;		   // B8
		std::uint64_t			unkC0;			   // C0
		NiPointer<NiAVObject>	artObject3D;	   // C8
		std::uint64_t			unkD0;			   // D0
	};
	STATIC_ASSERT(offsetof(ModelReferenceEffect, artObject3D) == 0xC8);

	class AIProcessManager
	{
	public:
		static AIProcessManager* GetSingleton();

		// members
		bool						enableDetection;				// 001
		bool						enableDetectionStats;			// 002
		UInt8						pad003;							// 003
		UInt32						trackedDetectionHandle;			// 004
		bool						enableHighProcessing;			// 008
		bool						enableLowProcessing;			// 009
		bool						enableMiddleHighProcessing;		// 00A
		bool						enableMiddleLowProcessing;		// 00B
		UInt16						unk00C;							// 00C
		UInt8						unk00E;							// 00E
		UInt8						pad00F;							// 00F
		SInt32						numActorsInHighProcess;			// 010
		float						unk014;							// 014
		UInt32						unk018;							// 018
		float						removeExcessComplexDeadTime;	// 01C
		HANDLE						semaphore;						// 020
		UInt32						unk028;							// 028
		UInt32						pad02C;							// 02C
		tArray<UInt32>				highProcesses;					// 030
		tArray<UInt32>				lowProcesses;					// 048
		tArray<UInt32>				middleLowProcesses;				// 060
		tArray<UInt32>				middleHighProcesses;			// 078
		tArray<UInt32>* highProcessesPtr;				// 090
		tArray<UInt32>* lowProcessesPtr;				// 098
		tArray<UInt32>* middleLowProcessesPtr;			// 0A0
		tArray<UInt32>* middleHighProcessesPtr;			// 0A8
		UInt64						unk0B0;							// 0B0
		UInt64						unk0B8;							// 0B8
		UInt64						unk0C0;							// 0C0
		UInt64						unk0C8;							// 0C8
		UInt64						unk0D0;							// 0D0
		UInt64						unk0D8;							// 0D8
		UInt64						unk0E0;							// 0E0
		tArray<BSTempEffect*>		tempEffects;					// 0E8
		SimpleLock					tempEffectsLock;				// 100
		tArray<ReferenceEffect*>	referenceEffects;				// 108
		SimpleLock					referenceEffectsLock;			// 120
		tArray<void*>				unk128;							// 128
		UInt64						unk140;							// 140
		UInt64						unk148;							// 148
		UInt64						unk150;							// 150
		tArray<UInt32>				unk158;							// 158
		UInt32						unk170;							// 170
		UInt32						pad174;							// 174
		UInt64						unk178;							// 178
		tArray<void*>				unk180;							// 180
		SimpleLock					unk198;							// 198
		tArray<UInt32>				unk1A0;							// 1A0
		tArray<void*>				unk1B8;							// 1B8
		float						unk1D0;							// 1D0
		float						unk1D4;							// 1D4
		UInt64						unk1D8;							// 1D8
		UInt32						unk1E0;							// 1E0
		bool						enableAIProcessing;				// 1E4
		bool						enableMovementProcessing;		// 1E5
		bool						enableAnimationProcessing;		// 1E6
		UInt8						unk1E7;							// 1E7
		UInt64						unk1E8;							// 1E8


		MEMBER_FN_PREFIX(AIProcessManager);
		DEFINE_MEMBER_FN(StopArtObject, void, 0x007048E0, TESObjectREFR*, BGSArtObject*);

	};
	STATIC_ASSERT(sizeof(AIProcessManager) == 0x1F0);

	extern RelocPtr<AIProcessManager*> g_AIProcessManager;


	class taskRemoveAndAddSpell : public TaskDelegate
	{
	public:
		virtual void Run();
		virtual void Dispose();

		taskRemoveAndAddSpell(SpellItem* akSpell);
		SpellItem* m_akSpell;
	};

	class taskAddSpell : public TaskDelegate
	{
	public:
		virtual void Run();
		virtual void Dispose();

		taskAddSpell(SpellItem* akSpell);
		SpellItem* m_akSpell;
	};

	class taskRemoveSpell : public TaskDelegate
	{
	public:
		virtual void Run();
		virtual void Dispose();

		taskRemoveSpell(SpellItem* akSpell);
		SpellItem* m_akSpell;
	};



	class taskAddPerk : public TaskDelegate
	{
	public:
		virtual void Run();
		virtual void Dispose();

		taskAddPerk(BGSPerk* akPerk);
		BGSPerk* m_akPerk;
	};

	class taskRemovePerk : public TaskDelegate
	{
	public:
		virtual void Run();
		virtual void Dispose();

		taskRemovePerk(BGSPerk* akPerk);
		BGSPerk* m_akPerk;
	};

	class taskRemoveAllShieldPerks : public TaskDelegate
	{
	public:
		virtual void Run();
		virtual void Dispose();

		taskRemoveAllShieldPerks();
	};

	class taskDoCombatSpellApply : public TaskDelegate
	{
	public:
		virtual void Run();
		virtual void Dispose();

		taskDoCombatSpellApply(SpellItem* spell, Actor* actor, UInt32 targetRefHandle);
		UInt32 m_targetRefHandle;
		SpellItem* m_spell;
		Actor* m_actor;
	};



	typedef ShaderReferenceEffect* (*_PlayEffectShader)(TESObjectREFR* ref, TESEffectShader* effectShader, float duration, TESObjectREFR* facingRef, UInt8 a5, UInt8 a6, __int64 a7, bool a8);
	extern RelocAddr <_PlayEffectShader> PlayEffectShader;
}
