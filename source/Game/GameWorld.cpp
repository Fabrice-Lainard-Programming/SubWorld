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


#include "GameWorld.h"
#include "GameLevel.h"
#include "GamePlay.h"
#include "BattleUnitUI.h"


// ----------------------------------------------------------------------------

using namespace SubWorld;

// ----------------------------------------------------------------------------

GameWorld::GameWorld()
	: _gameplay(nullptr), _dt_sec(std::chrono::system_clock::now())
{

}

// ----------------------------------------------------------------------------


void GameWorld::start()
{
	BattleUnitUI::globalInit();
	_gameplay = new GamePlay(this);
	_gameplay->loadGame();

}

// ----------------------------------------------------------------------------



void GameWorld::close()
{
	_gameplay->closeGame();
	safe_delete(_gameplay);
	BattleUnitUI::globalShutdown();

}

// ----------------------------------------------------------------------------


void GameWorld::update()
{
	// update global simulation clock
	_clock.update();
	// update level	
	if (_gameplay)
	{
		_gameplay->update(_clock.getTotalSimulationTime(), _clock.getElapsedSimulationTime());


		// emit update callback each seconds
		auto now = std::chrono::system_clock::now();
		auto elapsed_s = std::chrono::duration_cast<std::chrono::milliseconds>(now - _dt_sec).count();
		if (elapsed_s > 400)
		{
			_gameplay->update_on_400ms(_clock.getTotalSimulationTime(), _clock.getElapsedSimulationTime());
			_dt_sec = std::chrono::system_clock::now();
		}
		
	}

	//draw annotations
	_gameplay->drawAnnotations(_clock.getElapsedSimulationTime());
}
 

 

// ----------------------------------------------------------------------------


void GameWorld::updatePhysic()
{
	// update global simulation clock
	_clock.update();
	// update level	
	if (_gameplay)
	{
		_gameplay->updatePhysic(_clock.getTotalSimulationTime(), _clock.getElapsedSimulationTime());
	}
}

// ----------------------------------------------------------------------------


void GameWorld::drawAnnotations(const float elapsedTime)
{
	_gameplay->drawAnnotations(elapsedTime);

}


