check_PROGRAMS = test_fs
TESTS = $(check_PROGRAMS)

AM_CPPFLAGS = -I$(top_srcdir)/src

# Include custom gtest-lite header
nodist_test_fs_SOURCES = gtest/gtest.h

test_fs_SOURCES = test_fs.cpp $(top_srcdir)/src/fs.cc

