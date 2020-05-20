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



#include "BattleUnit.h"
#include "GameLevel.h"
#include "GameWorld.h"
#include "GamePlay.h"
#include "GameNode.h"
#include "PathFinder.h"
#include "Converter.h"
#include <UnigineApp.h>
#include <UnigineVisualizer.h>
#include <UnigineWorld.h>
#include "AutoActionTimer.h"
#include <list>
#pragma warning( push )
#pragma warning( disable : 4458) 
#include <gdiplus.h>
#pragma warning( pop ) 
#include <comdef.h> 


// ----------------------------------------------------------------------------

#include "WeaponControlSystem.h"
#include "SteeringBehaviors.h"
#include "BattleUnitUI.h"


// ----------------------------------------------------------------------------

using namespace Unigine;
using namespace Math;
using namespace SubWorld;


// ----------------------------------------------------------------------------

REGISTER_COMPONENT(BattleUnit);



// ----------------------------------------------------------------------------


void BattleUnit::shutdown()
{

	safe_delete(_ui);
}



// ----------------------------------------------------------------------------


void BattleUnit::init()
{
	_amount_of_noise_emitted = 0;
	_amount_of_energy = 0;
	_amount_of_enemy = 0;
	_amount_of_damage = 0;
	_explosion_on_destroy = true;
	_emergency_level = enumAlertLevel::ALERT_LEVEL_LOW;

	GameNodePtr gamenode = getGameNode();
	_ui = new BattleUnitUI(gamenode, [this](Gdiplus::Graphics *g, int w, int h)
	{
		return paintUI(g, w, h);
	});
}



// ----------------------------------------------------------------------------

void BattleUnit::updateUnitStatus(const float elapsedTime)
{
	// refresh war status using the Weapon Control System component data
	updateWCSStatus(elapsedTime);
	// refresh war status using the steering engine (noise, etc)
	updateSteeringStatus(elapsedTime);
	// determine the final status of this unit
	determineCombinedstatus(elapsedTime);
	// refresh UI
	if (_ui)
	{
		_ui->draw(elapsedTime);
	}
}

// ----------------------------------------------------------------------------

void BattleUnit::impactDamage(float power)
{
	_amount_of_damage += power;
	if (_amount_of_damage >= 1)
	{
		GameNodePtr gamenode = getGameNode();
		if (_explosion_on_destroy)
		{
			buildExplosion();
		}

		gamenode->_level->removeGameNode(gamenode);

	}
}


// ----------------------------------------------------------------------------


void BattleUnit::buildExplosion()
{
	GameNodePtr gamenode = getGameNode();
	Unigine::Ptr<Unigine::Node> smoke = Unigine::World::loadNode("vfx\\nodes\\vfx_smoke_black.node", 0);
	OpenSteer::Vec3 gamePos = gamenode->position();
	//OpenSteer::Vec3 pos(gamePos.x, GamePlay::Game->_see_level + 0.5, wandgamePoserPos.z);

	smoke->setWorldPosition(Converter::toUnigine(gamePos));
	new AutoActionTimer<Unigine::NodePtr>(smoke, 1000 * 20,
		[](AutoActionTimer<Unigine::NodePtr>& update_delegate, const float currentTime, const float elapsedTime)
	{

	},
		[this](AutoActionTimer<Unigine::NodePtr>& delete_delegate)
	{
		// destroy smoke
		delete_delegate._ptr->deleteLater();

	});
}
// ----------------------------------------------------------------------------


void BattleUnit::drawAnnotations(const float elapsedTime)
{
	return;
	GameNodePtr gamenode = getGameNode();

	Unigine::Math::Vec4 vcolor;
	std::string text;
	float lineHeight = 0.0f;
	float dH = 1.2f;// *(1.0 / gamenode->getCameraDistance());
	Unigine::Math::Vec3 pos(Converter::toUnigine(gamenode->position()));
	Unigine::Math::Vec3 center(-1, -1, 0);
	float r = gamenode->radius();

	if (gamenode->getCameraDistance() > 20)// && gamenode->getCameraDistance() < 200)
	{
		// Battle Unit data
		switch (_emergency_level)
		{
		case enumAlertLevel::ALERT_LEVEL_LOW: text = _tr("Defcon: Low");  vcolor = Unigine::Math::Vec4(0, 1, 0, 1);  break;
		case enumAlertLevel::ALERT_LEVEL_MEDIUM:text = _tr("Defcon: Medium"); vcolor = Unigine::Math::Vec4(0.937, 0.737, 0.164, 1);  break;
		case enumAlertLevel::ALERT_LEVEL_HIGH:text = _tr("Defcon: High"); vcolor = Unigine::Math::Vec4(0.898, 0.188, 0.043, 1); break;
		case enumAlertLevel::ALERT_LEVEL_CRITICAL:text = _tr("Defcon: Critical");  vcolor = Unigine::Math::Vec4(0.898, 0.043, 0.043, 1); break;

		}

		text += "  Damage:" + std::to_string(_amount_of_damage);


		Unigine::Math::Vec3 u = Converter::toUnigine(OpenSteer::Vec3(0, r + lineHeight, 0));
		Unigine::Visualizer::renderMessage3D(pos + u, center, text.c_str(), vcolor, 1);
		lineHeight += dH;

	}

	//if (gamenode->getCameraDistance() > 20)
	{
		// Steering data
		SteeringBehaviors* sb = ComponentSystem::get()->getComponent<SteeringBehaviors>(gamenode->_node);
		if (sb)
		{
			std::string steeringStr = sb->toHUDString();
			Unigine::Math::Vec3 u = Converter::toUnigine(OpenSteer::Vec3(0, r + lineHeight, 0));
			Unigine::Visualizer::renderMessage3D(pos + u, center, steeringStr.c_str(), vcolor, 1);
			lineHeight += dH;
		}
	}

	//  if (gamenode->getCameraDistance()< 50)
	{
		// WCS
		WeaponControlSystem* wcs = ComponentSystem::get()->getComponent<WeaponControlSystem>(gamenode->_node);
		if (wcs)
		{
			std::vector<std::string> wcsStr = wcs->toHUDString();

			for (auto s : wcsStr)
			{
				Unigine::Math::Vec3 u = Converter::toUnigine(OpenSteer::Vec3(0, r + lineHeight, 0));
				Unigine::Visualizer::renderMessage3D(pos + u, center, s.c_str(), vcolor, 1);
				lineHeight += dH;
			}

		}
	}

}



bool BattleUnit::paintUI(Gdiplus::Graphics *g, int w, int h)
{
	GameNodePtr gamenode = getGameNode();
	if (gamenode->cameraDistance() > 80) return false;

	g->FillRectangle(BattleUnitUI::_blackBrush, Gdiplus::Rect(0, 0, w, 20));
	//g->DrawRectangle(BattleUnitUI::_whitepen, Gdiplus::Rect(0, 0, w, 20));
	// -- DAMAGE

	float damage = 1.0f - _amount_of_damage;

	Gdiplus::LinearGradientBrush linGrBrush(
		Gdiplus::Point(0, 20),
		Gdiplus::Point(w, 20),
		InvColor(255, 0, 0),   // opaque red
		InvColor(0, 255, 0));

	Gdiplus::SolidBrush  damageBrush(InvColor((BYTE)(255.0f*_amount_of_damage), 0, 0));
	int widthDamage = (int)(damage * w);
	g->FillRectangle(&linGrBrush, Gdiplus::Rect(1, 1, widthDamage, 20));

	// -- ALERT LEVEL

	if (_emergency_level != enumAlertLevel::ALERT_LEVEL_LOW)
	{



		Gdiplus::Color defcon_Color;
		std::wstring text;


		switch (_emergency_level)
		{
		default:
		case enumAlertLevel::ALERT_LEVEL_LOW: text = _tr(L"Low");  defcon_Color = InvColor(128, 255, 0);  break;
		case enumAlertLevel::ALERT_LEVEL_MEDIUM:text = _tr(L"Alert: Medium"); defcon_Color = InvColor(255, 255, 0);  break;
		case enumAlertLevel::ALERT_LEVEL_HIGH:text = _tr(L"Alert: Hight"); defcon_Color = InvColor(255, 128, 0); break;
		case enumAlertLevel::ALERT_LEVEL_CRITICAL:text = _tr(L"Alert: Citical");  defcon_Color = InvColor(204, 0, 0); break;

		}

		Gdiplus::SolidBrush  defconBrush(defcon_Color);
		g->FillRectangle(&defconBrush, Gdiplus::Rect(w - 20, 1, w, 20));
	}

	SteeringBehaviors* sb = ComponentSystem::get()->getComponent<SteeringBehaviors>(gamenode->_node);
	if (sb)
	{

	//	const std::string sbstr = sb->toHUDString();
	//	const std::wstring steeringStr(sbstr.c_str(), sbstr.c_str() + sbstr.length());
	// g->DrawString(steeringStr.c_str(), -1, BattleUnitUI::_font, Gdiplus::PointF(2, 24), BattleUnitUI::_withBrush);
	/* Gdiplus::LinearGradientBrush linGrBrush(
		 Gdiplus::Point(0, 20),
		 Gdiplus::Point(20, 20),
		 InvColor(0, 0, 0),    
		 InvColor(0, 0, 255));
	 g->FillRectangle(&linGrBrush, Gdiplus::Rect(w - 40, 1, w-20, 20));*/
	}

		/*

		//g->FillRectangle(&defconBrush, Gdiplus::Rect(2, 24, 15, 35));
		g->DrawString(text.c_str(), -1, &font, Gdiplus::PointF(2, 24), &withBrush);
		
		const char * dataPath = Unigine::Engine::get()->getDataPath();

		
		const std::wstring path(std::wstring(dataPath, dataPath + strlen(dataPath)) + L"SubWorld\\Assets\\Radar\\radar_icon.png");
		Gdiplus::Image radar(path.c_str());
		Gdiplus::ImageAttributes attr;


		g->FillEllipse(&defconBrush, Gdiplus::Rect(w- radar.GetWidth(), 25, radar.GetWidth(), radar.GetHeight()));

		g->DrawImage(&radar,
			Gdiplus::Rect(w - radar.GetWidth(), 25, radar.GetWidth(), radar.GetHeight()));
			*/
	/*	Gdiplus::ColorMatrix ClrMatrix = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.5f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		attr.SetColorMatrix(&ClrMatrix, Gdiplus::ColorMatrixFlagsDefault,
			Gdiplus::ColorAdjustTypeBitmap);

		g->DrawImage(&radar, 
			Gdiplus::Rect(0, 0, radar.GetWidth(), radar.GetHeight()),
			0, 0, radar.GetWidth(), radar.GetHeight(), Gdiplus::UnitPixel,
			&attr);*/
	 
	//g->DrawString(L"Important Damage", -1, &font, Gdiplus::PointF(0, 0), &blackBrush);

	/*for (int x = 0; x < w; x += (w / 10))
	{
		g->DrawLine(&blackpen, x, 2, x, 20);
	}*/


	//g->DrawString(text.c_str(), -1, &font, Gdiplus::PointF(2, 2), &withBrush);
	return true;
}


/*


	Gdiplus::SolidBrush  solidBrush(Gdiplus::Color(255, 0, 0, 255));
	g->DrawString(L"Look at this text!", -1, &font, Gdiplus::PointF(0, 0), &solidBrush);
	Gdiplus::Pen blackpen(Gdiplus::Color(255, 0, 0, 255), 3);
	g->DrawLine(&blackpen, 0, 1, w, h);
Gdiplus::LinearGradientBrush linGrBrush(
		Gdiplus::Point(0, 10),
		Gdiplus::Point(200, 10),
		Gdiplus::Color(255, 0, 255, 0),   // opaque red
		Gdiplus::Color(255, 0, 0, 255));

		*/

		// ----------------------------------------------------------------------------

void BattleUnit::determineCombinedstatus(const float elapsedTime)
{

	if (_amount_of_enemy == 0)
	{
		_emergency_level = enumAlertLevel::ALERT_LEVEL_LOW;
	}
	else
		if (_amount_of_enemy == 1)
		{
			_emergency_level = enumAlertLevel::ALERT_LEVEL_MEDIUM;
		}
		else
			if (_amount_of_enemy > 1 && _amount_of_enemy < 3)
			{
				_emergency_level = enumAlertLevel::ALERT_LEVEL_HIGH;
			}
			else
			{
				_emergency_level = enumAlertLevel::ALERT_LEVEL_CRITICAL;
			}
}

// ----------------------------------------------------------------------------

void BattleUnit::updateWCSStatus(const float elapsedTime)
{
	WeaponControlSystem* wcs = ComponentSystem::get()->getComponent<WeaponControlSystem>(node);
	if (!wcs) return;
	_amount_of_enemy = (int)wcs->getThreats().size();

}

// ----------------------------------------------------------------------------

void BattleUnit::updateSteeringStatus(const float elapsedTime)
{
	SteeringBehaviors* steering = ComponentSystem::get()->getComponent<SteeringBehaviors>(node);
	if (!steering) return;
	_amount_of_noise_emitted = steering->getAmountOfNoisEmitted();

}



// ----------------------------------------------------------------------------

GameNodePtr BattleUnit::getGameNode()
{
	return static_cast<GameNode*>(node->getPtr());
}


// ----------------------------------------------------------------------------

