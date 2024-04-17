#pragma once

#include "package.h"
#include "loader.h"
#include "parser.h"

#include <string>
#include <vector>
#include <algorithm>


class solver {
public:
	solver(loader* loader);
	~solver();
private:
	loader* _loader;
};