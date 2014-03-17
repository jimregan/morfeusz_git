/* 
 * File:   cli.hpp
 * Author: mlenart
 *
 * Created on 17 marzec 2014, 18:32
 */

#ifndef CLI_HPP
#define	CLI_HPP

#ifdef _WIN64
#define TMPDUPA_IN IN
#define IN  IN
#else
#ifdef _WIN32
#define TMPDUPA_IN IN
#define IN  IN
#endif
#endif

#include <iostream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wlong-long"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wformat-extra-args"

#include "ezOptionParser.hpp"

#pragma GCC diagnostic pop

void printCLIUsage(ez::ezOptionParser& opt, std::ostream& out) {
	std::string usage;
	opt.getUsage(usage);
	out << usage;
}

#ifdef _WIN64
#define IN  TMPDUPA_IN
#else
#ifdef _WIN32
#define IN  TMPDUPA_IN
#endif
#endif

#endif	/* CLI_HPP */

