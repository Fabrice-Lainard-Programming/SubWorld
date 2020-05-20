// ----------------------------------------------------------------------------
//
//
// SubWorld -- SubMarine Game
//
// Copyright (c) 2020, F.Lainard
// Original author: F.Lainard
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//
// ----------------------------------------------------------------------------


#pragma once

#include <map>
#include <vector>
#include <string>
#include <stack>
#include <functional>
#include "UnigineMathLib.h"
#include "GamePlayCallBack.h"
 

//#define PLAY_SOUNDS 

namespace SubWorld
{
	class GameWorld;
	class GameLevel;
	class GamePlay;
	class GameFactory;
 

	// actual mode of the  game play
	enum enumCurrentGameMode
	{
		PLAY	,					// Play game
		USER_INTERFACE_ACTIVATED,	// UI is shown
		ACQUIRE_MOUSE_CLICK,		// Wait for a mouse click
	};

	// factions
	enum enumFaction
	{
		RED_FACTION,
		BLUE_FACTION,
		WHITE_FACTION,
	};

	// zone on game
	enum enumGameZone
	{
		TERRAIN,
		NODE,
	};


	// ----------------------------------------------------------------------------


	// manage the game play logic
	class GamePlay
	{
	public:
		GamePlay(GameWorld* world);
		virtual ~GamePlay();
		// called when a game is loaded 
		virtual void loadGame();
		// called when game shutdown
		virtual void closeGame();
		// update handler
		virtual void update(const float currentTime, const float elapsedTime);
		//  callback fo update called each seconds
		virtual void update_on_400ms(const float currentTime, const float elapsedTime);
		// update physic bodies
		virtual void updatePhysic(const float currentTime, const float elapsedTime);
		// called on mouse click over terrain, node, ...
		virtual void mouseClick(enumGameZone zone, const Unigine::Math::vec3& pt);
		// gest the current mode
		enumCurrentGameMode getMode() const {	return _mode; }
		// try to change the current game mode
		bool pushGameMode(enumCurrentGameMode mode);
		// returns to the previous mode
		void popGameMode();
		// draw annotations
		virtual void drawAnnotations(const float elapsedTime);
		// show message on screen
		void showMessage(const std::string& str, const Unigine::Math::vec4 &color);
		// is the two factions enemy?
		bool isEnemy(enumFaction a, enumFaction b);
		// return the factory  
		GameFactory* factory()  { return _game_factory; }
		// returns the current level 
		GameLevel * getCurrentevel() { return _current_level; }

	protected:
		void load_next_level(); 
		void loadLevel(GameLevel* level);
		std::string toString(enumCurrentGameMode mode);
		 
	public:
		// public accessors (get and set) 
		GameWorld* _world;
		GameLevel * _current_level;
		GameFactory* _game_factory;
		static GamePlay* Game;
		Unigine::Math::Vec4 _selectionColor;
		Unigine::Math::Vec4 _selectionLightColor;
		const float _see_level = 10;

	private:
		enumCurrentGameMode _mode;
		std::vector< GamePlayCallBack*> _updateCallBack;
		friend GamePlayCallBack;
		std::stack< enumCurrentGameMode> _mode_stack;
	};
	
}