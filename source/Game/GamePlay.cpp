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

#include "GamePlay.h"
#include "GameWorld.h"
#include "GameLevel.h"
#include "GameFactory.h"
#include "GameNode.h"
#include <UnigineVisualizer.h>


// ----------------------------------------------------------------------------

// levels
#include "Level_Discovery.h"

// stations
#include "BasicStation.h"

// subs
#include "SubClassA.h"


// ----------------------------------------------------------------------------

using namespace SubWorld;
using namespace OpenSteer;

// ----------------------------------------------------------------------------


GamePlay* GamePlay::Game = nullptr;

// ----------------------------------------------------------------------------

GamePlay::GamePlay(GameWorld* world)
	: _world(world), _game_factory(nullptr),_current_level(nullptr), _mode(enumCurrentGameMode::PLAY)
{
	Game = this;
	_selectionColor = Unigine::Math::Vec4(1.0f, 0.4f, 0.0f, 1);
	_selectionLightColor = Unigine::Math::Vec4(0.5f, 0.5f, 0.5f, 1);
}

// ----------------------------------------------------------------------------

GamePlay::~GamePlay()
{
	safe_delete(_game_factory);
}

// ----------------------------------------------------------------------------


void GamePlay::loadGame()
{
	_game_factory = new GameFactory(this);
	_game_factory->registerAssets();
	load_next_level();
}

// ----------------------------------------------------------------------------


void GamePlay::closeGame()
{
	if (_current_level)
	{
		_current_level->closeLevel();
		safe_delete(_current_level);
	}
}


// ----------------------------------------------------------------------------

void GamePlay::update(const float currentTime, const float elapsedTime)
{
	if (_current_level)
	{
		_current_level->update(currentTime, elapsedTime);
	}

	for (GamePlayCallBack* cb : _updateCallBack)
	{
		cb->update(currentTime, elapsedTime);
	}
}

// ----------------------------------------------------------------------------

void GamePlay::update_on_400ms(const float currentTime, const float elapsedTime)
{
	if (_current_level)
	{
		_current_level->update_on_400ms(currentTime, elapsedTime);
	}
}




// ----------------------------------------------------------------------------


void GamePlay::updatePhysic(const float currentTime, const float elapsedTime)
{
	if (_current_level)
	{
		_current_level->updatePhysic(currentTime, elapsedTime);
	}
}



// ----------------------------------------------------------------------------


void GamePlay::drawAnnotations(const float elapsedTime)
{
	if (_current_level)
	{
		_current_level->drawAnnotations(elapsedTime);
	}
}



// ----------------------------------------------------------------------------

void GamePlay::load_next_level()
{
	GameLevel* level =	static_cast<GameLevel*>(_game_factory->buildAsset("LEVEL::DISCOVERY"));
	loadLevel(level);

}

// ----------------------------------------------------------------------------


void GamePlay::loadLevel(GameLevel* level)
{
	if (_current_level)
	{
		_current_level->closeLevel();
		safe_delete(_current_level);
	}
	_current_level = level;
	_current_level->loadLevel();
}


// ----------------------------------------------------------------------------

bool GamePlay::isEnemy(enumFaction a, enumFaction b)
{
	return a != b;
}

 
// ----------------------------------------------------------------------------


void GamePlay::mouseClick(enumGameZone zone, const Unigine::Math::vec3& pt)
{
	if (_mode != enumCurrentGameMode::USER_INTERFACE_ACTIVATED)
	{
		// propaget mouse click only if UI is not shown
		_current_level->mouseClick(zone, pt);
	}
}


// ----------------------------------------------------------------------------

void GamePlay::popGameMode()
{
	_mode = _mode_stack.top();
	printf("\npopGameMode return to mode %s ", toString(_mode).c_str());
	_mode_stack.pop();
}

// ----------------------------------------------------------------------------

bool GamePlay::pushGameMode(enumCurrentGameMode mode)
{
	printf("\npushGameMode %s ", toString(mode).c_str());

	switch (mode)
	{

	case enumCurrentGameMode::PLAY:
	{
	} break;
	case enumCurrentGameMode::ACQUIRE_MOUSE_CLICK:
	{
		// no constraints here 
	}break;
	case enumCurrentGameMode::USER_INTERFACE_ACTIVATED : 
	{
		// if UI is already open : forbidden
		if (_mode == enumCurrentGameMode::USER_INTERFACE_ACTIVATED)
		{
			printf(" \t forbidden (old mode = %s)", toString(_mode).c_str());
			return false;
		}
		// if mouse click acquisition is in progress : forbidden
		if (_mode == enumCurrentGameMode::ACQUIRE_MOUSE_CLICK)
		{
			printf(" \t forbidden (old mode = %s)", toString(_mode).c_str());

			return false;
		}
	} break;
	}
	printf(" \t authorized (push old mode : %s)",toString(_mode).c_str());
	_mode_stack.push(_mode);
	_mode = mode;
	return true; // authorize this mode
}

// ----------------------------------------------------------------------------

std::string GamePlay::toString(enumCurrentGameMode mode)
{
	std::string strmode;
	switch (mode)
	{

	case enumCurrentGameMode::PLAY:
	{
		strmode = "PLAY";
	} break;
	case enumCurrentGameMode::ACQUIRE_MOUSE_CLICK:
	{
		strmode = "ACQUIRE_MOUSE_CLICK";
	}break;
	case enumCurrentGameMode::USER_INTERFACE_ACTIVATED:
	{
		strmode = "USER_INTERFACE_ACTIVATED";
	} break;
	}
	return strmode;
}

// ----------------------------------------------------------------------------

void GamePlay::showMessage(const std::string& str, const Unigine::Math::vec4 &color)
{
	Unigine::Math::Vec3 pos(0.2f, 0.9f, 1);
	Unigine::Math::Vec3 center(-1, -1, 0);
	Unigine::Math::Vec4 vcolor(color.x, color.y, color.z, 1.0);
	Unigine::Visualizer::renderMessage2D(pos, center, str.c_str(), vcolor, 1,24);
}

// ----------------------------------------------------------------------------
// GamePlayCallBack
// ----------------------------------------------------------------------------

GamePlayCallBack::GamePlayCallBack()
{
	GamePlay::Game->_updateCallBack.push_back(this);
}

// ----------------------------------------------------------------------------


GamePlayCallBack::~GamePlayCallBack()
{
	auto it = std::remove_if(GamePlay::Game->_updateCallBack.begin(), GamePlay::Game->_updateCallBack.end(),
		[this](GamePlayCallBack* g)
	{
		return g == this;
	});
	GamePlay::Game->_updateCallBack.erase(it, GamePlay::Game->_updateCallBack.end());

	 
}

