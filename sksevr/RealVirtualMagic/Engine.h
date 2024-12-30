#pragma once
#include "Helper.h"

namespace RealVirtualMagic 
{
	extern bool IXRInitialized;
	
	void GameLoad();
	void StartFunction();
	void WaitForDebugger(bool should_break);
	void LeftHandedModeChange();
	void CheckMagickCasting();
	bool IsCasting();
	void HookAnimations();
	void StartCast();
	void EndCast();
	void ApplyFocusValue(double newFocus);

	bool RegisterFuncs(VMClassRegistry* registry);

}