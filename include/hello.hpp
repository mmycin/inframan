#pragma once

#include <iostream>

using namespace std;

namespace hello {
	class Hello {
	public:
		Hello() = default;
		static void say_hello();
	};
}
