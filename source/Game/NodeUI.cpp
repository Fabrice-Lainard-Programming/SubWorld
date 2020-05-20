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



#include "NodeUI.h"
#include "GameWorld.h"
#include <UnigineApp.h>
#include <UnigineUserInterface.h>
#include "GamePlay.h"
#include "GameLevel.h"
#include "GameNode.h"
#include "Converter.h"
#include "ResourceCapacitySystem.h"
#include "SteeringBehaviors.h"
#include <sstream>

// ----------------------------------------------------------------------------

using namespace Unigine;
using namespace Math;
using namespace SubWorld;


// ----------------------------------------------------------------------------

REGISTER_COMPONENT(NodeUI);

#define getUIWidget(name)  _ui->getWidget(_ui->findWidget(name))


// ----------------------------------------------------------------------------


void NodeUI::shutdown()
{


}

// ----------------------------------------------------------------------------


void NodeUI::update()
{

}


// ----------------------------------------------------------------------------




void NodeUI::init()
{
	_uiVisible = false;
	_activePanel = enumNodePanel::WELCOME_MANAGER;
	auto screen_gui = Gui::get();
	_ui = UserInterface::create(screen_gui, "SubWorld\\Assets\\UserInterfaces\\node_gui.ui");
	_window = getUIWidget("window");
	screen_gui->setExposeSpeed(4);
	screen_gui->setFadeOutSpeed(4);
	_window->arrange();
	initCallBack();

}

// ----------------------------------------------------------------------------



void NodeUI::initCallBack()
{
	// WELCOM MANAGER
	_ui->addCallback("bt_close", Gui::CLICKED, MakeCallback(this, &NodeUI::close_window_cb));
	_ui->addCallback("bt_back", Gui::CLICKED, MakeCallback(this, &NodeUI::back_window_cb));

	// RESOURCE MANAGER
	_ui->addCallback("bt_Manageresource", Gui::CLICKED, MakeCallback(this, &NodeUI::manageresource_cb));
	_ui->addCallback("bt_addResource", Gui::CLICKED, MakeCallback(this, &NodeUI::create_resources_cb));
	_ui->addCallback("listbox_Resources", Gui::CLICKED, MakeCallback(this, &NodeUI::select_listbox_resource));

	// STEERING BEHAVIOR
	
	_ui->addCallback("bt_ManageSteering", Gui::CLICKED, MakeCallback(this, &NodeUI::managesteering_cb));
	_ui->addCallback("bt_Travel", Gui::CLICKED, MakeCallback(this, &NodeUI::travel_bt));
	_ui->addCallback("bt_Travel_circular", Gui::CLICKED, MakeCallback(this, &NodeUI::travel_circular_bt));
	_ui->addCallback("spinbox_speed", Gui::CLICKED, MakeCallback(this, &NodeUI::travel_speed_bt));
	

}

// ----------------------------------------------------------------------------




void NodeUI::show_ui()
{
	if (!_window) return;
 
	// ask to change the game mode to UI 
	if (!GamePlay::Game->pushGameMode(enumCurrentGameMode::USER_INTERFACE_ACTIVATED)) return;
	auto screen_gui = Gui::get();
	screen_gui->addChild(_window, Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER );
	_window->arrange();
	_window->setHidden(false);
	_uiVisible = true;

	showPanel(enumNodePanel::WELCOME_MANAGER);

}
//

// ----------------------------------------------------------------------------

void NodeUI::close_window_cb()
{
	
	auto screen_gui = Gui::get();

	_window->setHidden(true);
	screen_gui->removeChild(_window);
	_uiVisible = false;
	GamePlay::Game->popGameMode();

}


// ----------------------------------------------------------------------------



void NodeUI::showPanel(enumNodePanel panel)
{
	switch (panel)
	{

	case WELCOME_MANAGER:
	{
		getUIWidget("WelcomePane")->setHidden(false);
		getUIWidget("ResourceManagementPane")->setHidden(true);
		getUIWidget("SteeringManagerPane")->setHidden(true);
		getUIWidget("bt_close")->setHidden(false);
		getUIWidget("bt_back")->setHidden(true);

	} break;
	case RESOURCE_MANAGER:
	{
		getUIWidget("WelcomePane")->setHidden(true);
		getUIWidget("ResourceManagementPane")->setHidden(false);
		getUIWidget("SteeringManagerPane")->setHidden(true);

		getUIWidget("bt_close")->setHidden(true);
		getUIWidget("bt_back")->setHidden(false);
		initResourceManager();

	} break;
	case STEERING_MANAGER:
	{
		getUIWidget("WelcomePane")->setHidden(true);
		getUIWidget("ResourceManagementPane")->setHidden(true);
		getUIWidget("SteeringManagerPane")->setHidden(false);
		getUIWidget("bt_close")->setHidden(true);
		getUIWidget("bt_back")->setHidden(false);
		initSteeringManager();

	} break;

	}

	_activePanel = panel;
	// hide module not connected with our node
	if (!ComponentSystem::get()->getComponent<ResourceCapacitySystem>(node))
	{
		getUIWidget("ResourceManagementPane")->setHidden(true);
		getUIWidget("bt_Manageresource")->setHidden(true);

	}
	// hide module not connected with our node
	if (!ComponentSystem::get()->getComponent<SteeringBehaviors>(node))
	{
		getUIWidget("SteeringManagerPane")->setHidden(true);
		getUIWidget("bt_ManageSteering")->setHidden(true);


	}
}




// ----------------------------------------------------------------------------


void NodeUI::refresh_ui()
{
	if (!_uiVisible) return;
	if (!_window) return;
	
	// if we are in ACQUIRE_MOUSE_CLICK we need to hide the gui windows
	if (GamePlay::Game->getMode() == enumCurrentGameMode::ACQUIRE_MOUSE_CLICK)
	{
		_window->setHidden(true);
		return;
	}
	else
	{
		// otherwise we can show it
		_window->setHidden(false);
	}

	switch (_activePanel)
	{

	case WELCOME_MANAGER:
	{
	} break;
	case RESOURCE_MANAGER:
	{
		refreshResourceManager();


	} break;
	case STEERING_MANAGER:
	{
		refreshSteeringManager();
	} break;

	}
}



// ----------------------------------------------------------------------------

void NodeUI::back_window_cb()
{
	showPanel(enumNodePanel::WELCOME_MANAGER);
}



// ----------------------------------------------------------------------------


void NodeUI::showMessage(const std::string& msg)
{
	auto screen_gui = Gui::get();
	_messageDialog = WidgetDialogMessage::create(screen_gui, _tr("SubWorld"));
	_messageDialog->setMessageText(msg.c_str());
	_messageDialog->getOkButton()->addCallback(Gui::CLICKED, MakeCallback(this, &NodeUI::closeMessageDialog));
	_messageDialog->getCancelButton()->addCallback(Gui::CLICKED, MakeCallback(this, &NodeUI::closeMessageDialog));
	screen_gui->addChild(_messageDialog, Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER);
	_messageDialog->setPermanentFocus();

}

// ----------------------------------------------------------------------------

void  NodeUI::closeMessageDialog()
{
	auto screen_gui = Gui::get();

	screen_gui->removeChild(_messageDialog);

}

// ----------------------------------------------------------------------------
// STEERING BEHAVIOR MANAGER
// ----------------------------------------------------------------------------


void NodeUI::managesteering_cb()
{
	showPanel(enumNodePanel::STEERING_MANAGER);
}


void NodeUI::initSteeringManager()
{
	SteeringBehaviors*steering = ComponentSystem::get()->getComponent<SteeringBehaviors>(node);
	GameNodePtr gamenode = steering->getGameNode();
	// show current speed
	WidgetSpinBoxPtr spin = Unigine::checked_ptr_cast<WidgetSpinBox>(getUIWidget("spinbox_speed"));
	spin->setValue((int)gamenode->maxSpeed());
	spin->setMaxValue(20);
	spin->setMinValue(1);
	 
}

void NodeUI::refreshSteeringManager()
{
	
}

void NodeUI::travel_bt()
{
	
	 SteeringBehaviors*steering = ComponentSystem::get()->getComponent<SteeringBehaviors>(node);
	 steering->changeBehavior(enumSteeringBehaviors::STEERING_TRAVEL_WAIT_FOR_DESTINATION);
	 close_window_cb();

}


void NodeUI::travel_circular_bt()
{

	SteeringBehaviors*steering = ComponentSystem::get()->getComponent<SteeringBehaviors>(node);
	steering->changeBehavior(enumSteeringBehaviors::STEERING_TRAVEL_WAIT_FOR_CIRCULAR_DESTINATION);
	close_window_cb();

}



void NodeUI::travel_speed_bt()
{

	SteeringBehaviors*steering = ComponentSystem::get()->getComponent<SteeringBehaviors>(node);

	WidgetSpinBoxPtr spin = Unigine::checked_ptr_cast<WidgetSpinBox>(getUIWidget("spinbox_speed"));
	steering->change_speed((float)spin->getValue());

}


// ----------------------------------------------------------------------------
// RESOURCE MANAGER
// ----------------------------------------------------------------------------


void NodeUI::initResourceManager()
{
	ResourceCapacitySystem* rcs = ComponentSystem::get()->getComponent<ResourceCapacitySystem>(node);
	// add resource label in the listbox
	WidgetListBoxPtr listbox = Unigine::checked_ptr_cast<WidgetListBox>(getUIWidget("listbox_Resources"));
	listbox->clear();

	for (Resource_Capacity& def : rcs->getCapacities())
	{
		// retrieve the label of the resource template
		GameAssetTemplate* asset_template = GameFactory::getAssetTemplate(def._template_name);
		// add to list
		std::ostringstream stringStream;
		stringStream << asset_template->_user_name << " (" << def._available_capacity << ")";
		int item = listbox->addItem(stringStream.str().c_str());
		// sets its data with the index of Resource_Capacity in the vector
		listbox->setHeight(100);
		// no more left...
		if (def._available_capacity == 0)
		{
			//listbox->setItemSelectable(item, 0);
			listbox->setItemColor(item, Unigine::Math::vec4(0.5, 0.5, 0.5, 1));
		}
	}
	// sets capacity count to zero
	WidgetSpinBoxPtr spin = Unigine::checked_ptr_cast<WidgetSpinBox>(getUIWidget("spinbox_Resources"));
	spin->setValue(0);
	// hide counters
	WidgetLabelPtr label = Unigine::checked_ptr_cast<WidgetLabel>(getUIWidget("editline_resource_info"));
	label->setHidden(true);

}





// ----------------------------------------------------------------------------


void NodeUI::manageresource_cb()
{
	auto screen_gui = Gui::get();
	showPanel(enumNodePanel::RESOURCE_MANAGER);
}


// ----------------------------------------------------------------------------


void NodeUI::refreshResourceManager()
{

	// refresh selected resource
	refresh_listbox_resource();

}

// ----------------------------------------------------------------------------


void NodeUI::refresh_listbox_resource()
{
	auto screen_gui = Gui::get();

	// we need infos from the selected resource
	ResourceCapacitySystem* rcs = ComponentSystem::get()->getComponent<ResourceCapacitySystem>(node);
	WidgetListBoxPtr listbox = Unigine::checked_ptr_cast<WidgetListBox>(getUIWidget("listbox_Resources"));
	int selectedItem = listbox->getCurrentItem();
	if (selectedItem == -1) { return; }
	// extract capacity data for the selected resource
	Resource_Capacity& rc = rcs->getCapacities().at(selectedItem);
	// add info on selected resource capacity
	// search how many build are in progress for that resource 
	long nbbuild, total_build_time_s, first_time_left_s;
	rcs->get_build_counters(rc, nbbuild, total_build_time_s, first_time_left_s);
	// sets text label
	WidgetLabelPtr label = Unigine::checked_ptr_cast<WidgetLabel>(getUIWidget("editline_resource_info"));
	label->setHidden(false);
	std::ostringstream stringStream;
	if (nbbuild != 0)
	{
		stringStream << _tr("in progress: ") << nbbuild << _tr(", total time : ") << total_build_time_s << "s" << _tr(", ready in : ") << first_time_left_s << "s";
	}
	else
	{
		stringStream << _tr("build time : ") << rc._build_time_s << _tr("s, available : ") << rc._available_capacity;
	}

	label->setText(stringStream.str().c_str());
}


// ----------------------------------------------------------------------------


void NodeUI::select_listbox_resource()
{
	auto screen_gui = Gui::get();
	refresh_listbox_resource();
	ResourceCapacitySystem* rcs = ComponentSystem::get()->getComponent<ResourceCapacitySystem>(node);
	WidgetSpinBoxPtr spin = Unigine::checked_ptr_cast<WidgetSpinBox>(getUIWidget("spinbox_Resources"));
	WidgetListBoxPtr listbox = Unigine::checked_ptr_cast<WidgetListBox>(getUIWidget("listbox_Resources"));
	int selectedItem = listbox->getCurrentItem();
	if (selectedItem == -1) { return; }

	// extract capacity data for the selected resource
	Resource_Capacity& rc = rcs->getCapacities().at(selectedItem);
	// limit capacity in the spin box
	spin->setMaxValue(rc._available_capacity);
	spin->setValue(0);

}


// ----------------------------------------------------------------------------

void NodeUI::create_resources_cb()
{
	ResourceCapacitySystem* rcs = ComponentSystem::get()->getComponent<ResourceCapacitySystem>(node);
	WidgetListBoxPtr listbox = Unigine::checked_ptr_cast<WidgetListBox>(getUIWidget("listbox_Resources"));
	// gets selected item 
	int selectedItem = listbox->getCurrentItem();
	if (selectedItem == -1)
	{
		showMessage(_tr("Please select a resource !"));

		return;
	}
	// and retrieve its data ptr which is Resource_Capacity
	Resource_Capacity& rc = rcs->getCapacities().at(selectedItem);
	// gets the count of build :
	WidgetSpinBoxPtr spin = Unigine::checked_ptr_cast<WidgetSpinBox>(getUIWidget("spinbox_Resources"));
	int count = spin->getValue();
	if (count <= 0)
	{
		showMessage(_tr("Please enter a valid count !"));
		return;
	}
	// schedule creation  
	rcs->planify_creation(rc._template_name, count,nullptr);

}

