//
//  @file mputils.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/CrossPlatformCodeUtils/
//
//  Created on 02.06.22.
//  Copyright 2022 Sergii Oryshchenko. All rights reserved.
//

#ifndef MPUTILS_H
#define MPUTILS_H

#include <gmpxx.h>
#include <mpfr.h>

mpz_class MPZFromString(const std::string& sValue);
mpf_class MPFFromString(const std::string& sValue);
std::string ToString(const mpz_class& pValue, int nBase, const mpz_class& pNegative);
std::string ToString(const mpf_class& pValue);
mpz_class ToAligned(const mpz_class& pValue, uint32_t nBytesSize);

#endif
