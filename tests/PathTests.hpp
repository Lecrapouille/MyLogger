//=====================================================================
// MyLogger: A GIS in a spreadsheet.
// Copyright 2018 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of MyLogger.
//
// MyLogger is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MyLogger.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#ifndef PATH_TESTS_HPP_
#  define PATH_TESTS_HPP_

#  include <cppunit/TestFixture.h>
#  include <cppunit/TestResult.h>
#  include <cppunit/extensions/HelperMacros.h>

#  define protected public
#  define private public
#  include "Path.hpp"
#  undef protected
#  undef private

class PathTests : public CppUnit::TestFixture
{
  // CppUnit macros for setting up the test suite
  CPPUNIT_TEST_SUITE(PathTests);
  CPPUNIT_TEST(testEmptyConstructor);
  CPPUNIT_TEST(testSplitConstructor);
  CPPUNIT_TEST(testSplitDir);
  CPPUNIT_TEST(testFindAndExpand);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testEmptyConstructor();
  void testSplitConstructor();
  void testSplitDir();
  void testFindAndExpand();
};

#endif /* PATH_TESTS_HPP_ */
