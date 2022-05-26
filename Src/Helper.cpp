//
//  @file Helper.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/XDevCalc/
//
//  Created on 26.05.22.
//  Copyright 2022 Sergii Oryshchenko. All rights reserved.
//

#include "Helper.h"

std::string ToString(const mpf_class& pValue);
std::string ToString(const mpz_class& pValue, int nBase, const mpz_class& pNegative);
mpz_class ToAligned(const mpz_class& pValue, uint32_t nBytesSize);
std::string ToUpperCase(std::string sValue);

std::string ToString(const mpz_class& pValue, int nBase, const mpz_class& pNegative)
{

	std::string sValue = pNegative > 0 ? pNegative.get_str(nBase) : pValue.get_str(nBase);
	if (nBase == 16)
		return ToUpperCase(sValue);

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

std::string ToUpperCase(std::string sValue)
{
	std::transform(sValue.begin(), sValue.end(), sValue.begin(), ::toupper);
	return sValue;
}

