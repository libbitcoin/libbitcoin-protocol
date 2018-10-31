#!/bin/sh
###############################################################################
#  Copyright (c) 2014-2018 libbitcoin-protocol developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

set +e
set -x

# Define tests and options.
#==============================================================================
BOOST_UNIT_TEST_OPTIONS=\
"--run_test=* "\
"--show_progress=no "\
"--detect_memory_leak=0 "\
"--report_level=no "\
"--build_info=yes " \
"--log_level=all"

# Run tests.
#==============================================================================
./test/libbitcoin-protocol-test ${BOOST_UNIT_TEST_OPTIONS} > test.log
