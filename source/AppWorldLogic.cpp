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


#include "AppWorldLogic.h"
#include <UnigineVisualizer.h>
#include <UnigineApp.h>
#include <UnigineGame.h>
 

using namespace Unigine;


// World logic, it takes effect only when the world is loaded.
// These methods are called right after corresponding world script's (UnigineScript) methods.

AppWorldLogic::AppWorldLogic()
{
}

AppWorldLogic::~AppWorldLogic()
{
}

int AppWorldLogic::init()
{
	
	Unigine::PlayerSpectatorPtr spectator = Unigine::static_ptr_cast<Unigine::PlayerSpectator>(Unigine::Game::getPlayer());
	Unigine::ControlsPtr controls = spectator->getControls();
 
	//int key = Unigine::ControlsApp::getStateButton(Unigine::Controls::STATE_TURN_LEFT);

	// Unigine::ControlsApp::setStateButton(Unigine::Controls::STATE_FORWARD, Unigine::App::BUTTON_RIGHT);
	 

//if (controls->getState(Controls::STATE_MOVE_LEFT) || controls->getState(Controls::STATE_TURN_LEFT))

	Unigine::ControlsApp::setMouseHandle(Unigine::Input::MOUSE_HANDLE_SOFT);
	Unigine::Visualizer::setEnabled(true);
	Unigine::Visualizer::setMode(Unigine::Visualizer::MODE::MODE_ENABLED_DEPTH_TEST_DISABLED);
	_world = new SubWorld::GameWorld();
	_world->start();
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// start of the main loop
////////////////////////////////////////////////////////////////////////////////

int AppWorldLogic::update()
{
	float ifps = Game::getIFps();
	_world->update();
	
	 
	return 1;
}

int AppWorldLogic::postUpdate()
{
	// The engine calls this function after updating each render frame: correct behavior after the state of the node has been updated.
	return 1;
}

int AppWorldLogic::updatePhysics()
{
	_world->updatePhysic();
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// end of the main loop
////////////////////////////////////////////////////////////////////////////////

int AppWorldLogic::shutdown()
{
	_world->close();
	return 1;
}

int AppWorldLogic::save(const Unigine::StreamPtr &stream)
{
	// Write here code to be called when the world is saving its state (i.e. state_save is called): save custom user data to a file.
	UNIGINE_UNUSED(stream);
	return 1;
}

int AppWorldLogic::restore(const Unigine::StreamPtr &stream)
{
	// Write here code to be called when the world is restoring its state (i.e. state_restore is called): restore custom user data to a file here.
	UNIGINE_UNUSED(stream);
	return 1;
}
