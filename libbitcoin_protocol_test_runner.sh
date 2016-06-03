#!/bin/sh
###############################################################################
#  Copyright (c) 2014-2015 libbitcoin-protocol developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# Define tests and options.
#==============================================================================
BOOST_UNIT_TEST_OPTIONS=\
"--run_test=authenticator_tests,certificate_tests,context_tests,frame_tests,identifiers_tests,message_tests,poller_tests,socket_tests,worker_tests "\
"--show_progress=no "\
"--detect_memory_leak=0 "\
"--report_level=no "\
"--build_info=yes"


# Run tests.
#==============================================================================
./test/libbitcoin_protocol_test ${BOOST_UNIT_TEST_OPTIONS} > test.log
