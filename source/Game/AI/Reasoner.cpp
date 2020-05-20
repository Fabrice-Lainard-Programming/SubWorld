

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



#include "Reasoner.h"
#include <iostream>
extern "C"
{
#include "../AI/CLIPS/clips.h"
}

// ----------------------------------------------------------------------------

using namespace SubWorld;

// ----------------------------------------------------------------------------


Reasoner::Reasoner()
	: _thread(&Reasoner::run,this), _theEnv(CreateEnvironment())
{


}

// ----------------------------------------------------------------------------


Reasoner::~Reasoner()
{
	if (_theEnv)
	{
		DestroyEnvironment(_theEnv);
	}
	_stop_thread = true;
	if (_thread.joinable())
		_thread.join();
}


// ----------------------------------------------------------------------------


void Reasoner::run()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	
	// Print thread ID
	std::cout << "Reasoner Thread ID : " << std::this_thread::get_id() << "\n";
	initKnowledge();
	while (!_stop_thread)
	{
		std::cout << "Reasoner Thread ID : " << std::this_thread::get_id() << "\n";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

// ----------------------------------------------------------------------------
/*

	SensorIA
		






*/
// ----------------------------------------------------------------------------
 
void Reasoner::def(char *templateDef)
{
	EnvBuild(_theEnv, templateDef);
}


 


// ----------------------------------------------------------------------------


void Reasoner::fact(char *f, char * r, char *v)
{
	char buff[255];
	sprintf(buff,"(%s (%s \"%s\")",f,r,v);
	EnvAssertString(_theEnv, buff);
}


void Reasoner::fact(char *f, char * r, long v)
{
	char buff[255];
	sprintf(buff, "(%s (%s %ld)", f, r, v);
	EnvAssertString(_theEnv, buff);
}

void Reasoner::fact(char *f, char * r, float v)
{
	char buff[255];
	sprintf(buff, "(%s (%s %f)", f, r, v);
	EnvAssertString(_theEnv, buff);
}

// ----------------------------------------------------------------------------
  

void Reasoner::initKnowledge()
{
	/*std::lock_guard<std::mutex> lockGuard(_mutex);
	void *theFact;
	void *templatePtr;
	DATA_OBJECT theValue;

	 
	EnvIncrementGCLocks(_theEnv);

	def("(deftemplate Enemy (slot Name) (slot x) (slot y) (slot z))");
	
	fact("Student", "Name", "v1");
	fact("Student", "x", 0);
	fact("Student", "y", 1);
	fact("Student", "z", 2);

 
	templatePtr = EnvFindDeftemplate(_theEnv, "Student");

	for (theFact = EnvGetNextFact(_theEnv, NULL);
		theFact != NULL;
		theFact = EnvGetNextFact(_theEnv, theFact))
	{
		if (EnvFactDeftemplate(_theEnv, theFact) != templatePtr) continue;

		EnvGetFactSlot(_theEnv, theFact, "Name", &theValue);
		EnvPrintRouter(_theEnv, STDOUT, DOToString(theValue));
		EnvPrintRouter(_theEnv, STDOUT, "\n");
	}

	EnvPrintRouter(_theEnv, STDOUT, "-------------\n");

	for (theFact = EnvGetNextFactInTemplate(_theEnv, templatePtr, NULL);
		theFact != NULL;
		theFact = EnvGetNextFactInTemplate(_theEnv, templatePtr, theFact))
	{
		EnvGetFactSlot(_theEnv, theFact, "Name", &theValue);
		EnvPrintRouter(_theEnv, STDOUT, DOToString(theValue));
		EnvPrintRouter(_theEnv, STDOUT, "\n");
	}
	EnvDecrementGCLocks(_theEnv);*/
}