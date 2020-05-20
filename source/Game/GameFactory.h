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
#include <functional>
#include "GameNode.h"

namespace SubWorld
{
	class GameWorld;
	class GameLevel;
	class GamePlay;
 

	// ----------------------------------------------------------------------------


	// type of game asset
	enum enumGameTemplateType
	{
		TEMPLATE_NONE,
		TEMPLATE_LEVEL,
		TEMPLATE_STATION,
		TEMPLATE_SUBMARINE,
		TEMPLATE_DRONE,
		TEMPLATE_TORPEDO,

	};

	// ----------------------------------------------------------------------------


	// template of a assets in game
	struct GameAssetTemplate
	{
		std::string _user_name;
		enumGameTemplateType _type;
		std::function<void*(GameAssetTemplate*asset_template, GameLevel* level, GameNodePtr game_node)> _build_fct;

		GameAssetTemplate() : _type(TEMPLATE_NONE) {}
		GameAssetTemplate(const std::string& user_name,enumGameTemplateType type, std::function<void*(GameAssetTemplate*asset_template, GameLevel* level, GameNodePtr game_node)> build_fct)
			: _type(type), _build_fct(build_fct), _user_name(user_name)
		{}

	};


	// ----------------------------------------------------------------------------



	// manage the creation of game assets
	class GameFactory
	{
	public:
		GameFactory(GamePlay* gameplay);
		~GameFactory();
		// register all asset templates
		void registerAssets();
		// construct a new game asset based on template
		static void* buildAsset(const std::string& name, GameNodePtr game_node = nullptr);
		// getse asset based on template
		static GameAssetTemplate* getAssetTemplate(const std::string& name);

	protected:
		 
		
	public:
		// public accessors (get and set) 
		GamePlay* _gameplay;
		static std::map<std::string, GameAssetTemplate> _templates;

	};

}