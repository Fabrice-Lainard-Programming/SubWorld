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


#include "GameFactory.h"
#include "GamePlay.h"
#include "GameWorld.h"
#include "GameLevel.h"
#include "GameNode.h"



// ----------------------------------------------------------------------------

// levels
#include "Level_Discovery.h"

// stations
#include "BasicStation.h"

// subs
#include "SubClassA.h"

// drones
#include "DroneClassA.h"

// Torpedo
#include "Torpedo.h"


// ----------------------------------------------------------------------------

using namespace SubWorld;
using namespace OpenSteer;

// ----------------------------------------------------------------------------


std::map<std::string, GameAssetTemplate> GameFactory::_templates;

// ----------------------------------------------------------------------------

GameFactory::GameFactory(GamePlay* gameplay)
	: _gameplay(gameplay)
{

}

// ----------------------------------------------------------------------------

GameFactory::~GameFactory()
{

}
 
// ----------------------------------------------------------------------------

void* GameFactory::buildAsset(const std::string& name, GameNodePtr game_node)
{
	
	GameAssetTemplate* asset_template = getAssetTemplate(name);
	if(!asset_template) return(nullptr);
	return asset_template->_build_fct(asset_template, GamePlay::Game->_current_level, game_node);
}



// ----------------------------------------------------------------------------


GameAssetTemplate* GameFactory::getAssetTemplate(const std::string& name)
{
	std::map<std::string, GameAssetTemplate>::iterator  it = _templates.find(name);
	if (it == _templates.end())
	{
		return nullptr;
	}
	 
	return &it->second;
}



// ----------------------------------------------------------------------------


void GameFactory::registerAssets()
{
	_templates["LEVEL::DISCOVERY"] = GameAssetTemplate(_tr("Discovery level"), enumGameTemplateType::TEMPLATE_LEVEL,
		[this](GameAssetTemplate*asset_template, GameLevel* level, GameNodePtr game_node) -> void*
	{
		auto asset = new LevelDiscovery(_gameplay);
		return asset;
	});


	_templates["STATION::BASIC"] = GameAssetTemplate(_tr("Basic Station"), enumGameTemplateType::TEMPLATE_STATION,
		[this](GameAssetTemplate*asset_template, GameLevel* level, GameNodePtr game_node)-> void*
	{
		auto asset = new BasicStation(level, GameNode::current_vehicule_unique_id++);
		asset->_template = asset_template;
		return asset;
	});

	_templates["SUB::CLASSA"] = GameAssetTemplate(_tr("Submarine Class Alpha 01"), enumGameTemplateType::TEMPLATE_SUBMARINE,
		[this](GameAssetTemplate*asset_template, GameLevel* level, GameNodePtr game_node)-> void*
	{
		auto asset = new SubClassA(level, GameNode::current_vehicule_unique_id++);
		asset->_template = asset_template;
		return asset;
	});

	_templates["SUB::CLASSB"] = GameAssetTemplate(_tr("Submarine Class Beta XU"), enumGameTemplateType::TEMPLATE_SUBMARINE,
		[this](GameAssetTemplate*asset_template,GameLevel* level, GameNodePtr game_node)-> void*
	{
		auto asset = new SubClassA(level, GameNode::current_vehicule_unique_id++);
		asset->_template = asset_template;
		return asset;
	});


	_templates["DRONE::DroneClassA"] = GameAssetTemplate(_tr("Drone Alpha ZT91"), enumGameTemplateType::TEMPLATE_DRONE,
		[this](GameAssetTemplate*asset_template,GameLevel* level, GameNodePtr game_node)-> void*
	{
		auto asset = new DroneClassA(level, GameNode::current_vehicule_unique_id++);
		asset->_template = asset_template;
		return asset;
	});
	

	_templates["TORPEDO::MKI"] = GameAssetTemplate(_tr("Torpedo MKI"), enumGameTemplateType::TEMPLATE_TORPEDO,
		[this](GameAssetTemplate*asset_template,  GameLevel* level, GameNodePtr game_node)-> void*
	{
		auto asset = new Torpedo(level, GameNode::current_vehicule_unique_id++);
		asset->_template = asset_template;
		return asset;
	});

	

}





// ----------------------------------------------------------------------------
