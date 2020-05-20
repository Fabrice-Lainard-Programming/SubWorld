 
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
#include <cstdio>
 


namespace SubWorld
{
	class RCBase
	{
	public:
		RCBase() : mRefCount(0) {}
		virtual ~RCBase() {}

		void grab() const { ++mRefCount; }

		void release() const
		{
			if (!(mRefCount > 0))
			{
				printf("\n mRefCount > 0 !! ");
			}
			--mRefCount;
			if (mRefCount == 0)
			{
				delete (RCBase *)this; 
			}
		}


	private:
		mutable int mRefCount;
	};


/**
* A reference counting-managed pointer for classes derived from RCBase which can
* be used as C pointer
*/
	template < class T >
	class RCPtr
	{
	public:
		//Construct using a C pointer
		//e.g. RCPtr< T > x = new T();
		RCPtr(T* ptr = NULL)
			: mPtr(ptr)
		{
			if (ptr != NULL) { ptr->grab(); }
		}

		//Copy constructor
		RCPtr(const RCPtr &ptr)
			: mPtr(ptr.mPtr)
		{
			if (mPtr != NULL) { mPtr->grab(); }
		}

		~RCPtr()
		{
			if (mPtr != NULL) { mPtr->release(); }
		}

		//Assign a pointer
		//e.g. x = new T();
		RCPtr &operator=(T* ptr)
		{
			//The following grab and release operations have to be performed
			//in that order to handle the case where ptr == mPtr
			//(See comment below by David Garlisch)
			if (ptr != NULL) { ptr->grab(); }
			if (mPtr != NULL) { mPtr->release(); }
			mPtr = ptr;
			return (*this);
		}

		//Assign another RCPtr
		RCPtr &operator=(const RCPtr &ptr)
		{
			return (*this) = ptr.mPtr;
		}

		//Retrieve actual pointer
		T* get() const
		{
			return mPtr;
		}

		//Some overloaded operators to facilitate dealing with an RCPtr 
			 //as a conventional C pointer.
		//Without these operators, one can still use the less transparent 
			 //get() method to access the pointer.
		T* operator->() const { return mPtr; }		//x->member
		T &operator*() const { return *mPtr; }		//*x, (*x).member
		operator T*() const { return mPtr; }		//T* y = x;
		operator bool() const { return mPtr != NULL; }	//if(x) {/*x is not NULL*/}
		bool operator==(const RCPtr &ptr) { return mPtr == ptr.mPtr; }
		bool operator==(const T *ptr) { return mPtr == ptr; }

	private:
		T *mPtr;	//Actual pointer
	};

}