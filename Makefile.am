###############################################################################
#  Copyright (c) 2014-2015 libbitcoin-protocol developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# Automake settings.
#==============================================================================
# Look for macros in the m4 subdirectory.
#------------------------------------------------------------------------------
ACLOCAL_AMFLAGS = -I m4


# Distribute, make and install products.
#==============================================================================
# files => ${pkgconfigdir}
#------------------------------------------------------------------------------
pkgconfig_DATA = \
    libbitcoin-protocol.pc

# files => ${docdir}
#------------------------------------------------------------------------------
doc_DATA = \
    AUTHORS \
    COPYING \
    ChangeLog \
    INSTALL \
    NEWS \
    README

# src/libbitcoin-protocol.la => ${libdir}
#------------------------------------------------------------------------------
lib_LTLIBRARIES = src/libbitcoin-protocol.la
src_libbitcoin_protocol_la_CPPFLAGS = -I${srcdir}/include ${zmq_CPPFLAGS} ${bitcoin_CPPFLAGS}
src_libbitcoin_protocol_la_LIBADD = ${zmq_LIBS} ${bitcoin_LIBS}
src_libbitcoin_protocol_la_SOURCES = \
    src/settings.cpp \
    src/zmq/authenticator.cpp \
    src/zmq/certificate.cpp \
    src/zmq/context.cpp \
    src/zmq/frame.cpp \
    src/zmq/identifiers.cpp \
    src/zmq/message.cpp \
    src/zmq/poller.cpp \
    src/zmq/socket.cpp \
    src/zmq/worker.cpp \
    src/zmq/zeromq.cpp

# local: test/libbitcoin_protocol_test
#------------------------------------------------------------------------------
if WITH_TESTS

TESTS = libbitcoin_protocol_test_runner.sh

check_PROGRAMS = test/libbitcoin_protocol_test
test_libbitcoin_protocol_test_CPPFLAGS = -I${srcdir}/include ${zmq_CPPFLAGS} ${bitcoin_CPPFLAGS}
test_libbitcoin_protocol_test_LDADD = src/libbitcoin-protocol.la ${boost_unit_test_framework_LIBS} ${zmq_LIBS} ${bitcoin_LIBS}
test_libbitcoin_protocol_test_SOURCES = \
    test/converter.cpp \
    test/main.cpp \
    test/utility.hpp \
    test/zmq/authenticator.cpp \
    test/zmq/certificate.cpp \
    test/zmq/context.cpp \
    test/zmq/frame.cpp \
    test/zmq/identifiers.cpp \
    test/zmq/message.cpp \
    test/zmq/poller.cpp \
    test/zmq/socket.cpp \
    test/zmq/worker.cpp

endif WITH_TESTS

# files => ${includedir}/bitcoin
#------------------------------------------------------------------------------
include_bitcoindir = ${includedir}/bitcoin
include_bitcoin_HEADERS = \
    include/bitcoin/protocol.hpp

include_bitcoin_protocoldir = ${includedir}/bitcoin/protocol
include_bitcoin_protocol_HEADERS = \
    include/bitcoin/protocol/define.hpp \
    include/bitcoin/protocol/settings.hpp \
    include/bitcoin/protocol/version.hpp

include_bitcoin_protocol_zmqdir = ${includedir}/bitcoin/protocol/zmq
include_bitcoin_protocol_zmq_HEADERS = \
    include/bitcoin/protocol/zmq/authenticator.hpp \
    include/bitcoin/protocol/zmq/certificate.hpp \
    include/bitcoin/protocol/zmq/context.hpp \
    include/bitcoin/protocol/zmq/frame.hpp \
    include/bitcoin/protocol/zmq/identifiers.hpp \
    include/bitcoin/protocol/zmq/message.hpp \
    include/bitcoin/protocol/zmq/poller.hpp \
    include/bitcoin/protocol/zmq/socket.hpp \
    include/bitcoin/protocol/zmq/worker.hpp \
    include/bitcoin/protocol/zmq/zeromq.hpp

