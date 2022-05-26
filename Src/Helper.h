//
//  @file Helper.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/XDevCalc/
//
//  Created on 26.05.22.
//  Copyright 2022 Sergii Oryshchenko. All rights reserved.
//

#ifndef _HELPER_H_
#define _HELPER_H_

#include <gmpxx.h>
#include <mpfr.h>
#include <string>

std::string ToString(const mpz_class& pValue, int nBase, const mpz_class& pNegative);
std::string ToString(const mpf_class& pValue);
mpz_class ToAligned(const mpz_class& pValue, uint32_t nBytesSize);
std::string ToUpperCase(std::string sValue);

#endif

