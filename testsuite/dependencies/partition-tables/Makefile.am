#
# Makefile.am for libstorage/testsuite/dependencies/partition-tables
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	install1.test delete1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =									\
	install1-probed.xml install1-staging.xml install1-expected.txt		\
	delete1-probed.xml delete1-staging.xml delete1-expected.txt

