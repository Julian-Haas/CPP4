#include "stdafx.h"
#include "test.h"
#include <iostream>

namespace me
{
	Test::Test()
	{
		std::cout << "Test::Test";
	}

	Test::~Test()
	{
		std::cout << "Test::~Test";
	}

	void Test::Foo()
	{
		std::cout << "Test::Foo";
	}
}