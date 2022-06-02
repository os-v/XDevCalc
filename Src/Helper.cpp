//
//  @file Helper.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/XDevCalc/
//
//  Created on 26.05.22.
//  Copyright 2022 Sergii Oryshchenko. All rights reserved.
//

#include "Helper.h"
#include <algorithm>

std::string ToUpperCase(std::string sValue)
{
	std::transform(sValue.begin(), sValue.end(), sValue.begin(), ::toupper);
	return sValue;
}

