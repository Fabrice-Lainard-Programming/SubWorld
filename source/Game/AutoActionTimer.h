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

 
#include <functional>
#include <chrono>
#include "GamePlayCallBack.h"

template<typename To>
class AutoActionTimer : public GamePlayCallBack
{
public:
	AutoActionTimer(To ptr, long delay_ms, 
		std::function<void(AutoActionTimer<To>& AutoActionTimer,const float currentTime, const float elapsedTime)> update_delegate,
		std::function<void(AutoActionTimer<To>& AutoActionTimer)> delete_delegate)
 	: _ptr(ptr), _delay_ms(delay_ms), _update_delegate(update_delegate), _delete_delegate(delete_delegate)
	{
		_start = std::chrono::system_clock::now();;
	}

	~AutoActionTimer()
	{
		
		_delete_delegate(*this);
		_ptr = nullptr;
	}

	virtual void update(const float currentTime, const float elapsedTime) 
	{
		auto now = std::chrono::system_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _start).count();
		if (elapsed_ms > _delay_ms)
		{
			
			delete this;
		}
		else
		{
			_update_delegate(*this,currentTime,  elapsedTime);
		}
	}

	std::chrono::system_clock::time_point _start;
	long _delay_ms;
	To _ptr;
	std::function<void(AutoActionTimer<To>& AutoActionTimer,const float currentTime, const float elapsedTime)> _update_delegate;
	std::function<void(AutoActionTimer<To>& AutoActionTimer)> _delete_delegate;
};