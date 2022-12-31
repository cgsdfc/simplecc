// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/******************************************************************************
 *
 *  file:  XorHandler.h
 *
 *  Copyright (c) 2003, Michael E. Smoot .
 *  Copyright (c) 2004, Michael E. Smoot, Daniel Aarno.
 *  All rights reserved.
 *
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/

#ifndef TCLAP_XORHANDLER_H
#define TCLAP_XORHANDLER_H

#include <algorithm>
#include <iostream>
#include <string>
#include <tclap/Arg.h>
#include <vector>

namespace TCLAP {

/**
 * This class handles lists of Arg's that are to be XOR'd on the command
 * line.  This is used by CmdLine and you shouldn't ever use it.
 */
class XorHandler {
protected:
  /**
   * The list of of lists of Arg's to be or'd together.
   */
  std::vector<std::vector<Arg *>> _orList;

public:
  /**
   * Constructor.  Does nothing.
   */
  XorHandler() : _orList(std::vector<std::vector<Arg *>>()) {}

  /**
   * Add a list of Arg*'s that will be xor'd together.
   * \param ors - list of Arg* that will be xor'd.
   */
  void add(std::vector<Arg *> &ors);

  /**
   * Checks whether the specified Arg is in one of the xor lists and
   * if it does match one, returns the size of the xor list that the
   * Arg matched.  If the Arg matches, then it also sets the rest of
   * the Arg's in the list. You shouldn't use this.
   * \param a - The Arg to be checked.
   */
  int check(const Arg *a);

  /**
   * Returns the XOR specific short usage.
   */
  std::string shortUsage();

  /**
   * Prints the XOR specific long usage.
   * \param os - Stream to print to.
   */
  void printLongUsage(std::ostream &os);

  /**
   * Simply checks whether the Arg is contained in one of the arg
   * lists.
   * \param a - The Arg to be checked.
   */
  bool contains(const Arg *a);

  std::vector<std::vector<Arg *>> &getXorList();
};

//////////////////////////////////////////////////////////////////////
// BEGIN XOR.cpp
//////////////////////////////////////////////////////////////////////
inline void XorHandler::add(std::vector<Arg *> &ors) { _orList.push_back(ors); }

inline int XorHandler::check(const Arg *a) {
  // iterate over each XOR list
  for (int i = 0; static_cast<unsigned int>(i) < _orList.size(); i++) {
    // if the XOR list contains the arg..
    ArgVectorIterator ait = std::find(_orList[i].begin(), _orList[i].end(), a);
    if (ait != _orList[i].end()) {
      // first check to see if a mutually exclusive switch
      // has not already been set
      for (ArgVectorIterator it = _orList[i].begin(); it != _orList[i].end();
           it++)
        if (a != (*it) && (*it)->isSet())
          throw (CmdLineParseException(
              "Mutually exclusive argument already set!", (*it)->toString()));

      // go through and set each arg that is not a
      for (ArgVectorIterator it = _orList[i].begin(); it != _orList[i].end();
           it++)
        if (a != (*it))
          (*it)->xorSet();

      // return the number of required args that have now been set
      if ((*ait)->allowMore())
        return 0;
      else
        return static_cast<int>(_orList[i].size());
    }
  }

  if (a->isRequired())
    return 1;
  else
    return 0;
}

inline bool XorHandler::contains(const Arg *a) {
  for (int i = 0; static_cast<unsigned int>(i) < _orList.size(); i++)
    for (ArgVectorIterator it = _orList[i].begin(); it != _orList[i].end();
         it++)
      if (a == (*it))
        return true;

  return false;
}

inline std::vector<std::vector<Arg *>> &XorHandler::getXorList() {
  return _orList;
}

//////////////////////////////////////////////////////////////////////
// END XOR.cpp
//////////////////////////////////////////////////////////////////////

} // namespace TCLAP

#endif
