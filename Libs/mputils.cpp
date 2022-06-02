//
//  @file mputils.cpp
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

void GetStrNumberBase(const std::string& sValue, int& nBase, int& nOffset)
{
	if (sValue.length() > 1 && sValue[0] == '0' && sValue[1] >= '1' && sValue[1] <= '7')
		nBase = 8;
	else if (sValue.length() > 1 && sValue[0] == '0' && (sValue[1] == 'x' || sValue[1] == 'X'))
		nBase = 16, nOffset = 2;
	else if (sValue.length() > 1 && sValue[0] == '0' && (sValue[1] == 'b' || sValue[1] == 'B'))
		nBase = 2, nOffset = 2;
}

mpz_class MPZFromString(const std::string& sValue)
{
	int nBase = 0, nOffset = 0;
	GetStrNumberBase(sValue, nBase, nOffset);
	return mpz_class(sValue.c_str() + nOffset, nBase);
}

mpf_class MPFFromString(const std::string& sValue)
{
	try
	{
		int nBase = 0, nOffset = 0;
		GetStrNumberBase(sValue, nBase, nOffset);
		return mpf_class(sValue.c_str() + nOffset, mpf_get_default_prec(), nBase);
	}
	catch (std::invalid_argument pException)
	{
	}
	return MPZFromString(sValue);
}

std::string ToString(const mpz_class& pValue, int nBase, const mpz_class& pNegative)
{

	std::string sValue = pNegative > 0 ? pNegative.get_str(nBase) : pValue.get_str(nBase);
	if (nBase == 16)
		std::transform(sValue.begin(), sValue.end(), sValue.begin(), ::toupper);

	return sValue;
}

std::string ToString(const mpf_class& pValue)
{

	mp_exp_t expo;
	std::string sResult = pValue.get_str(expo);

	if (sResult.length() && sResult[0] == '-')
		sResult = sResult.substr(1);

	if (expo <= 0)
	{
		for (; expo < 0; expo++, sResult = '0' + sResult);
		sResult = "0." + sResult;
	}
	else if (expo < (mp_exp_t)sResult.length())
		sResult.insert(sResult.begin() + expo, '.');
	else
		sResult.insert(sResult.end(), expo - sResult.length(), '0');

	if (pValue < 0)
		sResult = "-" + sResult;

	return sResult;
}

mpz_class ToAligned(const mpz_class& pValue, uint32_t nBytesSize)
{

	mpz_class pResult;

	bool fNegative = pValue < 0;
	if (fNegative)
	{
		size_t nLength = pValue.get_str(16).length() - 1, nDigits = 8;
		for (; nLength > nDigits; nDigits <<= 1);
		std::string sHex = "1" + std::string(nDigits, '0');
		if(mpz_class(sHex, 16) / 2 < -pValue)
			sHex = "1" + std::string(nDigits << 1, '0');
		pResult = mpz_class(sHex, 16) + pValue;
	}
	else
		pResult = pValue;

	if (nBytesSize)
	{
		mpz_class pMask(0xFF);
		for (; --nBytesSize; )
			pResult = (pResult << 8) | 0xFF;
		pResult &= pMask;
	}

	return pResult;
}

#endif
