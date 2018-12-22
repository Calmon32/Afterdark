using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class FinalProjServerTarget : TargetRules   // Change this line as shown previously
{
       public FinalProjServerTarget(TargetInfo Target) : base(Target)  // Change this line as shown previously
       {
        Type = TargetType.Server;
        ExtraModuleNames.Add("FinalProj");    // Change this line as shown previously
       }
}