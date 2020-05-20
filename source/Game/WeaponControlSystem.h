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

#include "../ComponentSystem/ComponentSystem.h"
#include "Opensteer/include/OpenSteer/SimpleVehicle.h"
#include <UnigineMathLib.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>
#include "GameFactory.h"
#include "GameLevel.h"
#include <string>
#include <vector>
#include <functional>
#include <time.h>
#include <chrono>


namespace SubWorld
{
	class GameNode;
	class ResourceCapacitySystem;

	enum enumWeaponType
	{
	 
		ANTI_AIR_MISSILE_BASED,
		ANTI_SURFACE_MISSILE_BASED,
		ANTI_SUBMARINE_MISSILE_BASED, 
	};


	struct Radar
	{
		float _radar_detection_range;

		Radar() : _radar_detection_range(0) {}

		Radar(float radar_detection_range)
			: _radar_detection_range(radar_detection_range)
		{

		}
	};


	 

	 struct weapon_missile_def
	 {
		 int _load_time_ms;
		 std::string _missile_resource_name; // name definied in associated RCS (Resource Capacity System)

		 weapon_missile_def() : _load_time_ms(0) {}
		 weapon_missile_def(const std::string& missile_resource_name, int load_time_ms)
			 : _load_time_ms(load_time_ms) , _missile_resource_name(missile_resource_name)
		 {
		 }
	 };

	struct Weapon
	{
		enumWeaponType _type;
		// position of the weapon on the local coordinates of the nodes
		Unigine::Math::vec3 _local_position;
		// actual rotation of the weapon to align to the target
		Unigine::Math::vec3 _rotation_angles, _smoothed_orientation;
		// axis which can move (1 = can move)
		Unigine::Math::vec3 _authorized_rotation_angles;
		// name of the mesh in the model which can be rotated along its x axis 
		std::string _x_axis_rotation_node;
		// name of the mesh in the model which can be rotated along its y axis 
		std::string _y_axis_rotation_node;
		// name of the mesh in the model which can be rotated along its z  axis 
		std::string _z_axis_rotation_node;
		// last time of fire used to calm the weapons...
		std::chrono::system_clock::time_point _lastFireTime;
		// time between each fire
		float _fire_interval_ms;
		// true if the target is acquired
		bool _target_acquired;
		// range of this weapons
		float _range;
		// definition of the bullet
		std::vector <weapon_missile_def> _missile_definition;

		Weapon() :  _target_acquired(false), _range(0){}
		Weapon(enumWeaponType type, const Unigine::Math::vec3& local_position,float range,
			const std::string& x_axis_rotation_node, 
			const std::string& y_axis_rotation_node,
			const std::string& z_axis_rotation_node,
			const Unigine::Math::vec3& authorized_rotation_angles,
			const std::vector <weapon_missile_def>& missile_definition, float fire_interval_ms)
			: _type(type), _local_position(local_position),_range(range),
			_x_axis_rotation_node(x_axis_rotation_node),
			_y_axis_rotation_node(y_axis_rotation_node),
			_z_axis_rotation_node(z_axis_rotation_node),
			_authorized_rotation_angles(authorized_rotation_angles),
			 _target_acquired(false), _smoothed_orientation(0,0,0),
			_missile_definition(missile_definition),_fire_interval_ms(fire_interval_ms), _lastFireTime(std::chrono::system_clock::now())
		{
		
		}
	};

	struct Threat
	{
		// associated game node
		long _gamenode_id;
		// weapon which are focused on this threat
		std::vector<int> _attached_weapon_index;
		// number of fire
		int _fire_count;

		Threat() :_gamenode_id(-1), _fire_count(0){}

		Threat(long gamenode_id)
			: _gamenode_id(gamenode_id), _fire_count(0)
		{

		}
	};


	//   computers and radars to track and guide weapons to destroy enemy targets.
	class WeaponControlSystem : public ComponentBase
	{
	public:

		// methods
		COMPONENT(WeaponControlSystem, ComponentBase);
		COMPONENT_UPDATE(update);
		COMPONENT_INIT(init)
		COMPONENT_SHUTDOWN(shutdown);

		PROP_NAME("WeaponControlSystem");
		PROP_PARAM(String, name, "DummyParam");

		// init WCS system with radar and weapons
		void initWCS(Radar radar, std::vector< Weapon>& weapons, ResourceCapacitySystem* resourceCapacitySystem);
		// update weapons states
		void compute_weapons(const float elapsedTime);
		// align motor of weapons with targets
		void align_weapons(const float elapsedTime);
		// gets the radar
		Radar& getRadar() { return _radar; }
		// gets the list of managed weapons
		std::vector< Weapon>& getWeapons() { return _weapons; }
		// detected threats
		std::vector< Threat>& getThreats() { return _threats; }
		// gets attached game node
		GameNodePtr getGameNode();
		 // sets the RCS needed to build bullets
		void setResourceCapacity(ResourceCapacitySystem* rcs) {	_resourceCapacitySystem = rcs;	}
		// show or hide hud
		void setHudVisibility(bool visible) {  _showHUD = visible; }
		// build string which is visible in the hud
		std::vector<std::string> toHUDString();
	protected:
		void init();
		void update();
		void shutdown();
		// detect threat
		void detect_targets();
		// track threat already detected
		void track_targets(const float elapsedTime);
		// fire
		void weapons_fire(const float elapsedTime);
		 
		 
	protected:
		// weapons

		bool try_fire_weapon(const float elapsedTime, Threat& threat, Weapon& weapon);
		void adjust_weapons_guidance(const float currentTime);
		void adjust_weapon_guidance(const float currentTime,Threat& threat, Weapon& weapon);
		void align_weapon(const float elapsedTime, Threat& threat, Weapon& weapon);
		bool is_threat_manageable_by_weapon(const Threat& threat, const Weapon& weapon);
		bool is_threat_in_weapon_range(const Threat& threat, const Weapon& weapon);
	protected:
		// threat
		void attach_threats_to_weapons();
		bool try_attach_threat_to_weapon(Threat& threat);
		void remove_threat_out_of_range();
		bool is_threat_in_radar_range(const Threat& threat);
		const Threat*  find_threat(long gamenode_id);
		void add_threat(long gamenode_id);

	protected:
		std::vector< Weapon> _weapons;	
		std::vector< Threat> _threats;
		Radar _radar;
		ResourceCapacitySystem* _resourceCapacitySystem;
		bool _showHUD;
	};



}

