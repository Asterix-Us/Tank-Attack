// Copyright Vasile Bogdan, 2023. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TankAttackEditorTarget : TargetRules
{
	public TankAttackEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("TankAttack");
	}
}
