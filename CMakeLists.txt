cmake_minimum_required(VERSION 3.13.0)
project(ETQWSDK VERSION 1.4.4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# I'm a bit sloppy with headers and just glob them in..
# they're only handled in CMake at all so they turn up in Visual Studio solutions..

# globs all the headers from ${PATHPREFIX}/ and adds them to ${SRCLIST}
function(add_globbed_headers SRCLIST PATHPREFIX)
	file(GLOB_RECURSE tmp_hdrs RELATIVE "${CMAKE_SOURCE_DIR}" "${PATHPREFIX}/*.h")
	set(${SRCLIST} ${tmp_hdrs} ${${SRCLIST}} PARENT_SCOPE)
endfunction()

# game_sdk
set(src_game
    "source/framework/BuildVersion.cpp"
    "source/game/Actor.cpp"
    "source/game/AF.cpp"
    "source/game/AFEntity.cpp"
    "source/game/AnimatedEntity.cpp"
    "source/game/anim/Anim.cpp"
    "source/game/anim/Anim_Blend.cpp"
    "source/game/anim/Anim_FrameCommands.cpp"
    "source/game/anim/Anim_Import.cpp"
    "source/game/anim/Anim_Testmodel.cpp"
    "source/game/AntiLag.cpp"
    "source/game/AORManager.cpp"
    "source/game/Atmosphere.cpp"
    "source/game/AtmosphereRenderable.cpp"
    "source/game/botai/aas/AAS.cpp"
    "source/game/botai/aas/AASCallback_AvoidLocation.cpp"
    "source/game/botai/aas/AASCallback_FindAreaOutOfRange.cpp"
    "source/game/botai/aas/AASCallback_FindCoverArea.cpp"
    "source/game/botai/aas/AASCallback_FindFlaggedArea.cpp"
    "source/game/botai/aas/AAS_debug.cpp"
    "source/game/botai/aas/AAS_pathing.cpp"
    "source/game/botai/aas/AAS_routing.cpp"
    "source/game/botai/aas/ObstacleAvoidance.cpp"
    "source/game/botai/Bot.cpp"
    "source/game/botai/BotAI_Actions.cpp"
    "source/game/botai/BotAI_CombatNet.cpp"
    "source/game/botai/BotAI_Debug.cpp"
    "source/game/botai/BotAI_DMNet.cpp"
    "source/game/botai/BotAI_FightUtils.cpp"
    "source/game/botai/BotAI_Goals.cpp"
    "source/game/botai/BotAI_LTGNet.cpp"
    "source/game/botai/BotAI_Main.cpp"
    "source/game/botai/BotAI_MoveNet.cpp"
    "source/game/botai/BotAI_MoveUtils.cpp"
    "source/game/botai/BotAI_Navigation.cpp"
    "source/game/botai/BotAI_NBGNet.cpp"
    "source/game/botai/BotAI_NodeCommands.cpp"
    "source/game/botai/BotAI_Obstacles.cpp"
    "source/game/botai/BotAI_Routes.cpp"
    "source/game/botai/BotAI_Utils.cpp"
    "source/game/botai/BotAI_VCombatNet.cpp"
    "source/game/botai/BotAI_VCombatUtils.cpp"
    "source/game/botai/BotAI_VDMNet.cpp"
    "source/game/botai/BotAI_VMain.cpp"
    "source/game/botai/BotAI_VMoveNet.cpp"
    "source/game/botai/BotAI_VNodes.cpp"
    "source/game/botai/BotAI_VUtils.cpp"
    "source/game/botai/BotThread.cpp"
    "source/game/botai/BotThreadData.cpp"
    "source/game/Camera.cpp"
    "source/game/client/ClientEffect.cpp"
    "source/game/client/ClientEntity.cpp"
    "source/game/client/ClientMoveable.cpp"
    "source/game/CommandMapInfo.cpp"
    "source/game/CrosshairInfo.cpp"
    "source/game/demos/DemoAnalyzer.cpp"
    "source/game/demos/DemoCamera.cpp"
    "source/game/demos/DemoManager.cpp"
    "source/game/demos/DemoScript.cpp"
    "source/game/docs/wiki.cpp"
    "source/game/Effect.cpp"
    "source/game/effects/Effects.cpp"
    "source/game/effects/Flares.cpp"
    "source/game/effects/FootPrints.cpp"
    "source/game/effects/HardcodedParticleSystem.cpp"
    "source/game/effects/RainEffect.cpp"
    "source/game/effects/SnowEffect.cpp"
    "source/game/effects/TireTread.cpp"
    "source/game/effects/Wakes.cpp"
    "source/game/effects/WaterEffects.cpp"
    "source/game/Entity.cpp"
    "source/game/GameEdit.cpp"
    "source/game/gamesys/Class.cpp"
    "source/game/gamesys/DebugGraph.cpp"
    "source/game/gamesys/DeviceContextHelper.cpp"
    "source/game/gamesys/Event.cpp"
    "source/game/gamesys/Pvs.cpp"
    "source/game/gamesys/SysCmds.cpp"
    "source/game/gamesys/SysCvar.cpp"
    "source/game/Game_local.cpp"
    "source/game/Game_network.cpp"
    "source/game/Game_Repeater.cpp"
    "source/game/Game_ui.cpp"
    "source/game/guis/GuiSurface.cpp"
    "source/game/guis/GuiSurfaceRenderable.cpp"
    "source/game/guis/UIBinder.cpp"
    "source/game/guis/UICinematic.cpp"
    "source/game/guis/UICreditScroll.cpp"
    "source/game/guis/UICrosshairInfo.cpp"
    "source/game/guis/UIEdit.cpp"
    "source/game/guis/UIEditW.cpp"
    "source/game/guis/UIIconNotification.cpp"
    "source/game/guis/UILayout.cpp"
    "source/game/guis/UIList.cpp"
    "source/game/guis/UIList_Script.cpp"
    "source/game/guis/UIMarquee.cpp"
    "source/game/guis/UINews.cpp"
    "source/game/guis/UIObject.cpp"
    "source/game/guis/UIObjectScriptState.cpp"
    "source/game/guis/UIProgress.cpp"
    "source/game/guis/UIRadialMenu.cpp"
    "source/game/guis/UIRenderWorld.cpp"
    "source/game/guis/UISlider.cpp"
    "source/game/guis/UITimeline.cpp"
    "source/game/guis/UIWindow.cpp"
    "source/game/guis/UIWindow_Script.cpp"
    "source/game/guis/UIWindow_Shaped.cpp"
    "source/game/guis/UserInterfaceExpressions.cpp"
    "source/game/guis/UserInterfaceLocal.cpp"
    "source/game/guis/UserInterfaceLocal_Eval.cpp"
    "source/game/guis/UserInterfaceLocal_Script.cpp"
    "source/game/guis/UserInterfaceLocal_Script_Game.cpp"
    "source/game/guis/UserInterfaceManagerLocal.cpp"
    "source/game/guis/UserInterfaceScript.cpp"
    "source/game/guis/UserInterfaceTypes.cpp"
    "source/game/IK.cpp"
    "source/game/InputMode.cpp"
    "source/game/interfaces/TaskInterface.cpp"
    "source/game/Item.cpp"
    "source/game/Light.cpp"
    "source/game/LimboProperties.cpp"
    "source/game/Misc.cpp"
    "source/game/misc/AdEntity.cpp"
    "source/game/misc/DefenceTurret.cpp"
    "source/game/misc/DeliveryVehicle.cpp"
    "source/game/misc/Door.cpp"
    "source/game/misc/General.cpp"
    "source/game/misc/Parachute.cpp"
    "source/game/misc/PlayerBody.cpp"
    "source/game/misc/ProfileHelper.cpp"
    "source/game/misc/RenderEntityBundle.cpp"
    "source/game/misc/RepairDrone.cpp"
    "source/game/Moveable.cpp"
    "source/game/Mover.cpp"
    "source/game/network/SnapshotState.cpp"
    "source/game/physics/Clip.cpp"
    "source/game/physics/ClipModel.cpp"
    "source/game/physics/Force.cpp"
    "source/game/physics/Force_Constant.cpp"
    "source/game/physics/Force_Drag.cpp"
    "source/game/physics/Force_Field.cpp"
    "source/game/physics/Force_Spring.cpp"
    "source/game/physics/Physics.cpp"
    "source/game/physics/PhysicsEvent.cpp"
    "source/game/physics/Physics_Actor.cpp"
    "source/game/physics/Physics_AF.cpp"
    "source/game/physics/Physics_Base.cpp"
    "source/game/physics/Physics_JetPack.cpp"
    "source/game/physics/Physics_Linear.cpp"
    "source/game/physics/Physics_Monster.cpp"
    "source/game/physics/Physics_Parabola.cpp"
    "source/game/physics/Physics_Parametric.cpp"
    "source/game/physics/Physics_Player.cpp"
    "source/game/physics/Physics_RigidBody.cpp"
    "source/game/physics/Physics_RigidBodyMultiple.cpp"
    "source/game/physics/Physics_Simple.cpp"
    "source/game/physics/Physics_SimpleRigidBody.cpp"
    "source/game/physics/Physics_SimpleSpline.cpp"
    "source/game/physics/Physics_Static.cpp"
    "source/game/physics/Physics_StaticMulti.cpp"
    "source/game/physics/Push.cpp"
    "source/game/physics/TraceModelCache.cpp"
    "source/game/Player.cpp"
    "source/game/PlayerIcon.cpp"
    "source/game/PlayerProperties.cpp"
    "source/game/PlayerView.cpp"
    "source/game/PredictionErrorDecay.cpp"
    "source/game/proficiency/ProficiencyManager.cpp"
    "source/game/proficiency/StatsTracker.cpp"
    "source/game/Projectile.cpp"
    "source/game/roles/FireTeams.cpp"
    "source/game/roles/HudModule.cpp"
    "source/game/roles/Inventory.cpp"
    "source/game/roles/ObjectiveManager.cpp"
    "source/game/roles/RoleManager.cpp"
    "source/game/roles/Tasks.cpp"
    "source/game/roles/WayPointManager.cpp"
    "source/game/rules/AdminSystem.cpp"
    "source/game/rules/GameRules.cpp"
    "source/game/rules/GameRules_Campaign.cpp"
    "source/game/rules/GameRules_Objective.cpp"
    "source/game/rules/GameRules_StopWatch.cpp"
    "source/game/rules/GUIDState.cpp"
    "source/game/rules/UserGroup.cpp"
    "source/game/rules/VoteManager.cpp"
    "source/game/ScriptEntity.cpp"
    "source/game/script/ScriptEntityHelpers.cpp"
    "source/game/script/Script_Compiler.cpp"
    "source/game/script/Script_DLL.cpp"
    "source/game/script/Script_Exporter.cpp"
    "source/game/script/Script_Helper.cpp"
    "source/game/script/Script_Interface.cpp"
    "source/game/script/Script_Interpreter.cpp"
    "source/game/script/Script_Program.cpp"
    "source/game/script/Script_ScriptObject.cpp"
    "source/game/script/Script_SysCall.cpp"
    "source/game/script/Script_Thread.cpp"
    "source/game/sdnet/SDNetManager.cpp"
    "source/game/sdnet/SDNetProperties.cpp"
    "source/game/Sound.cpp"
    "source/game/structures/CommandMapPost.cpp"
    "source/game/structures/DeployMask.cpp"
    "source/game/structures/DeployRequest.cpp"
    "source/game/structures/DeployZone.cpp"
    "source/game/structures/TeamManager.cpp"
    "source/game/StuffSystem.cpp"
    "source/game/Target.cpp"
    "source/game/Teleporter.cpp"
    "source/game/Trigger.cpp"
    "source/game/UpdateManager.cpp"
    "source/game/vehicles/Attachments.cpp"
    "source/game/vehicles/JetPack.cpp"
    "source/game/vehicles/Pathing.cpp"
    "source/game/vehicles/RouteConstraint.cpp"
    "source/game/vehicles/SoundControl.cpp"
    "source/game/vehicles/Transport.cpp"
    "source/game/vehicles/TransportComponents.cpp"
    "source/game/vehicles/TransportExtras.cpp"
    "source/game/vehicles/TransportSystems.cpp"
    "source/game/vehicles/VehicleControl.cpp"
    "source/game/vehicles/VehicleIK.cpp"
    "source/game/vehicles/VehicleSuspension.cpp"
    "source/game/vehicles/VehicleView.cpp"
    "source/game/vehicles/VehicleWeapon.cpp"
    "source/game/vehicles/Vehicle_RigidBody.cpp"
    "source/game/vehicles/Walker.cpp"
    "source/game/Waypoints/LocationMarker.cpp"
    "source/game/Weapon.cpp"
    "source/game/WorldSpawn.cpp")
add_globbed_headers(src_game "source/game")

# idlib_sdk
set(src_idlib
    "source/idlib/BitMsg.cpp"
    "source/idlib/bv/Bounds.cpp"
    "source/idlib/bv/Bounds2D.cpp"
    "source/idlib/bv/BoundsShort.cpp"
    "source/idlib/bv/Box.cpp"
    "source/idlib/bv/Frustum.cpp"
    "source/idlib/bv/Sphere.cpp"
    "source/idlib/Callable.cpp"
    "source/idlib/Color.cpp"
    "source/idlib/containers/HashIndex.cpp"
    "source/idlib/Dict.cpp"
    "source/idlib/FunctionSignals.cpp"
    "source/idlib/geometry/JointTransform.cpp"
    "source/idlib/geometry/Surface.cpp"
    "source/idlib/geometry/Surface_Patch.cpp"
    "source/idlib/geometry/Surface_Polytope.cpp"
    "source/idlib/geometry/Surface_SweptSpline.cpp"
    "source/idlib/geometry/Surface_Traceable.cpp"
    "source/idlib/geometry/TraceModel.cpp"
    "source/idlib/geometry/TraceSurface.cpp"
    "source/idlib/geometry/Winding.cpp"
    "source/idlib/geometry/Winding2D.cpp"
    "source/idlib/hashing/CRC16.cpp"
    "source/idlib/hashing/CRC32.cpp"
    "source/idlib/hashing/CRC8.cpp"
    "source/idlib/hashing/Honeyman.cpp"
    "source/idlib/hashing/MD4.cpp"
    "source/idlib/hashing/MD5.cpp"
    "source/idlib/Heap.cpp"
    "source/idlib/ImagePacker.cpp"
    "source/idlib/LangDict.cpp"
    "source/idlib/Lib.cpp"
    "source/idlib/MapFile.cpp"
    "source/idlib/math/Angles.cpp"
    "source/idlib/math/Complex.cpp"
    "source/idlib/math/Lcp.cpp"
    "source/idlib/math/Mat3x4.cpp"
    "source/idlib/math/Math.cpp"
    "source/idlib/math/Matrix.cpp"
    "source/idlib/math/Ode.cpp"
    "source/idlib/math/Perlin.cpp"
    "source/idlib/math/Plane.cpp"
    "source/idlib/math/Pluecker.cpp"
    "source/idlib/math/Polynomial.cpp"
    "source/idlib/math/Quat.cpp"
    "source/idlib/math/Radians.cpp"
    "source/idlib/math/Random.cpp"
    "source/idlib/math/Rotation.cpp"
    "source/idlib/math/Simd.cpp"
    "source/idlib/math/Simd_3DNow.cpp"
    "source/idlib/math/Simd_Generic.cpp"
    "source/idlib/math/Simd_MMX.cpp"
    "source/idlib/math/Simd_SSE.cpp"
    "source/idlib/math/Simd_SSE2.cpp"
    "source/idlib/math/Simd_SSE3.cpp"
    "source/idlib/math/Simd_Xenon.cpp"
    "source/idlib/math/Vector.cpp"
    "source/idlib/PoolAllocator.cpp"
    "source/idlib/Properties.cpp"
    "source/idlib/sys/threading/SysLock_win.cpp"
    "source/idlib/sys/threading/SysSignal_win.cpp"
    "source/idlib/sys/threading/SysThread_win.cpp"
    "source/idlib/TextUtilities.cpp"
    "source/idlib/text/Base64.cpp"
    "source/idlib/text/CmdArgs.cpp"
    "source/idlib/text/Lexer.cpp"
    "source/idlib/text/LexerBinary.cpp"
    "source/idlib/text/Parser.cpp"
    "source/idlib/text/Str.cpp"
    "source/idlib/text/StrBuilder.cpp"
    "source/idlib/text/StrSimple.cpp"
    "source/idlib/text/Token.cpp"
    "source/idlib/text/UTF8.cpp"
    "source/idlib/text/WLexer.cpp"
    "source/idlib/text/WStr.cpp"
    "source/idlib/text/WToken.cpp"
    "source/idlib/threading/Lock.cpp"
    "source/idlib/threading/Signal.cpp"
    "source/idlib/threading/Thread.cpp"
    "source/idlib/Timer.cpp")
add_globbed_headers(src_idlib "source/idlib")

# libgamedecl_sdk
set(src_gamedecl
    "source/decllib/DeclType.cpp"
    "source/game/decls/DeclAmmoType.cpp"
    "source/game/decls/DeclAOR.cpp"
    "source/game/decls/DeclCampaign.cpp"
    "source/game/decls/DeclDamage.cpp"
    "source/game/decls/DeclDamageFilter.cpp"
    "source/game/decls/DeclDeployableObject.cpp"
    "source/game/decls/DeclDeployableZone.cpp"
    "source/game/decls/DeclDeployMask.cpp"
    "source/game/decls/DeclGUI.cpp"
    "source/game/decls/DeclGUITheme.cpp"
    "source/game/decls/DeclHeightMap.cpp"
    "source/game/decls/DeclInvItem.cpp"
    "source/game/decls/DeclInvItemType.cpp"
    "source/game/decls/DeclInvSlot.cpp"
    "source/game/decls/DeclItemPackage.cpp"
    "source/game/decls/DeclKeyBinding.cpp"
    "source/game/decls/DeclMapInfo.cpp"
    "source/game/decls/DeclPlayerClass.cpp"
    "source/game/decls/DeclPlayerTask.cpp"
    "source/game/decls/DeclProficiencyItem.cpp"
    "source/game/decls/DeclProficiencyType.cpp"
    "source/game/decls/DeclQuickChat.cpp"
    "source/game/decls/DeclRadialMenu.cpp"
    "source/game/decls/DeclRank.cpp"
    "source/game/decls/DeclRating.cpp"
    "source/game/decls/DeclRequirement.cpp"
    "source/game/decls/DeclStringMap.cpp"
    "source/game/decls/DeclTargetInfo.cpp"
    "source/game/decls/DeclToolTip.cpp"
    "source/game/decls/DeclVehiclePath.cpp"
    "source/game/decls/declVehicleScript.cpp"
    "source/game/decls/DeployMask.cpp"
    "source/game/decls/GameDeclIdentifiers.cpp"
    "source/game/decls/GameDeclInfo.cpp"
    "source/game/decls/HeightMap.cpp")
add_globbed_headers(src_gamedecl "source/game/decls")

# Qt
if(WIN32 AND NOT DEFINED QT_BASEDIR)
	message(WARNING "QT install not define, skipping SDKLauncher, pass the dir with -DQT_BASEDIR=\"C:/your/qt6/here\"")
endif()

# SDK only supports Debug and Release
if(CMAKE_CONFIGURATION_TYPES) 
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE) 
endif()

set(INSTALL_PATH "" CACHE STRING "Path to Enemy Territory: Quake Wars SDK Installation" FORCE)
set(MOD_NAME "" CACHE STRING "Name of your mod" FORCE)

# Defaults to our locally store copy of the SDK
if(INSTALL_PATH STREQUAL "")
    set(INSTALL_PATH "${CMAKE_SOURCE_DIR}/sdk_output")
endif()

if(MOD_NAME STREQUAL "")
    set(MOD_NAME "base")
endif()

if(UNIX)
    set(CMAKE_STATIC_LIBRARY_PREFIX "")
    set(CMAKE_SHARED_LIBRARY_PREFIX "")

    add_compile_definitions(__linux__)
endif(UNIX)

if(WIN32)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /D_DEBUG /Od /MDd /RTC1 /Z7")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /Oi /Ot /Gy /MD /Z7 /GS- /LTCG-")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_DEBUG /Od /MDd /RTC1 /Z7")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oi /Ot /Gy /MD /Z7 /GS- /LTCG-")

	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /LARGEADDRESSAWARE /SAFESEH:NO /STACK:4194304,4194304")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /LARGEADDRESSAWARE /SAFESEH:NO /STACK:4194304,4194304")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /LARGEADDRESSAWARE /SAFESEH:NO /STACK:4194304,4194304")

    add_compile_definitions(_WINDOWS)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_DEPRECATE)

    add_compile_options(/MP) # parallel build (use all cores, or as many as configured in VS)

    add_compile_options(/W4)

    # ignore the following warnings:
    add_compile_options(/wd4456) # Declaration of 'X' hides previous local declaration
    add_compile_options(/wd4189) # Local variable is initialized but not referenced
    add_compile_options(/wd4701) # Potentially uninitialized local variable 'X' used
    add_compile_options(/wd4458) # Declaration of 'X' hides class member
    add_compile_options(/wd4389) # Signed/unsigned mismatch
    add_compile_options(/wd4706) # Assignment within conditional expression
    add_compile_options(/wd4245) # Conversion from 'type1' to 'type2', signed/unsigned mismatch
    add_compile_options(/wd4838) # Conversion from 'type' to 'type' requires a narrowing conversion
    add_compile_options(/wd4459) # Declaration of 'X' hides global declaration
    add_compile_options(/wd4457) # Declaration of 'X' hides function parameter
    add_compile_options(/wd4703) # Potentially uninitialized local pointer variable 'X' used
    add_compile_options(/wd4702) # Unreachable code
    add_compile_options(/wd4477) # Format string 'X' requires an argument of type 'Y', but variadic argument 'Z' has type 'A'
    add_compile_options(/wd4611) # Interaction between 'function' and 'function' causes undefined behavior
    add_compile_options(/wd4505) # 'function': unreferenced local function has been removed
endif(WIN32)

add_compile_definitions(SD_SDK_BUILD SD_PUBLIC_BUILD)

# Needed so that VSCode can syntax check correctly.
include_directories(${CMAKE_SOURCE_DIR}/source ${CMAKE_SOURCE_DIR}/idlib PUBLIC)

if(DEFINED QT_BASEDIR)
	add_subdirectory(${CMAKE_SOURCE_DIR}/source/sdklauncher)
endif()

# idLib
add_library(idlib STATIC ${src_idlib})
target_precompile_headers(idlib PRIVATE source/idlib/precompiled.h)
target_compile_definitions(idlib PRIVATE __IDLIB__ _LIB)
target_include_directories(idlib PRIVATE ${CMAKE_SOURCE_DIR}/source/idlib)
if(WIN32)
    source_group(TREE ${CMAKE_SOURCE_DIR} PREFIX source FILES ${src_idlib})
endif(WIN32)

# GameDecl
add_library(gamedecl STATIC ${src_gamedecl})
target_precompile_headers(gamedecl PRIVATE source/game/decls/GameDecl_Precompiled.h)
target_compile_definitions(gamedecl PRIVATE __GAMEDECL__ _LIB)
target_include_directories(gamedecl PRIVATE ${CMAKE_SOURCE_DIR}/source/game ${CMAKE_SOURCE_DIR}/source/game/decls)
if(WIN32)
    source_group(TREE ${CMAKE_SOURCE_DIR} PREFIX source FILES ${src_gamedecl})
endif(WIN32)

# Gamex86
add_library(gamex86 SHARED ${src_game})
add_dependencies(gamex86 idlib gamedecl)
target_precompile_headers(gamex86 PRIVATE source/game/Game_Precompiled.h)
target_compile_definitions(gamex86 PRIVATE GAME_DLL)
target_link_libraries(gamex86 PRIVATE idlib gamedecl)
target_include_directories(gamex86 PRIVATE ${CMAKE_SOURCE_DIR}/source/game)
if(WIN32)
    set_target_properties(gamex86 PROPERTIES VS_DEBUGGER_COMMAND ${INSTALL_PATH}/etqw.exe)
    set_target_properties(gamex86 PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${INSTALL_PATH}")
    set_target_properties(gamex86 PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "+set fs_game ${MOD_NAME} +set r_fullscreen 0 +set com_allowConsole 1  +set win_allowmultipleInstances 1")
    set_target_properties(gamex86 PROPERTIES RUNTIME_OUTPUT_DIRECTORY $<1:>${CMAKE_SOURCE_DIR}/sdk_output/${MOD_NAME} LIBRARY_OUTPUT_DIRECTORY $<1:>${CMAKE_SOURCE_DIR}/sdk_output/${MOD_NAME})
    source_group(TREE ${CMAKE_SOURCE_DIR} PREFIX source FILES ${src_game})
endif(WIN32)
