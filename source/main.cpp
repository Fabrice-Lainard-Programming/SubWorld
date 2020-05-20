/* Copyright (C) 2005-2020, UNIGINE. All rights reserved.
 *
 * This file is a part of the UNIGINE 2.11.0.1 SDK.
 *
 * Your use and / or redistribution of this software in source and / or
 * binary form, with or without modification, is subject to: (i) your
 * ongoing acceptance of and compliance with the terms and conditions of
 * the UNIGINE License Agreement; and (ii) your inclusion of this notice
 * in any version of this software that you use or redistribute.
 * A copy of the UNIGINE License Agreement is available by contacting
 * UNIGINE. at http://unigine.com/
 */





#include <UnigineEngine.h>

#include "AppEditorLogic.h"
#include "AppSystemLogic.h"
#include "AppWorldLogic.h"
#include <UnigineApp.h>

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	// UnigineLogic
	AppSystemLogic system_logic;
	AppWorldLogic world_logic;
	AppEditorLogic editor_logic;
	// init engine
	Unigine::EnginePtr engine(UNIGINE_VERSION, argc, argv);
	//Unigine::App::setVideoMode(800, 600, 0 /*FULLSCREEN*/);

	// enter main loop
	engine->main(&system_logic, &world_logic, &editor_logic);

	return 0;
}

#ifdef _WIN32
#include <Windows.h>
extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif


/*

ASSET TABLE LIST
------------------------------


PURCHASED : 
https://3drt.com/store/index.php?dispatch=orders.downloads&product_id=311&ekey=b2e9be68dfef855bd98ba3529d99872c

https://assetstore.unity.com/packages/3d/vehicles/space/spaceship-fleet-iii-collection-iv-75093
Procédure pour extraire les FBX :
1 - exported with GameObject To Mesh (Obj) Unity Package (compre lainardfabriceprogramming@outlook.com)
	cet exporteur ne sert qu'a extraire la texture car le .obj résultant est buggé
2 - importer le FBX natif du package est lui associé la texture générée en 1
3- reconstruire la strructure des meshes pour les objects multi-fbx


*/ 
 
/*
	TOOLS :
 conversion du FBX ascii en FBX binary avec : https://www.autodesk.com/developer-network/platform-technologies/fbx-converter-archives   (présent dans le dossier Tools)
*/