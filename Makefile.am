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
src_libbitcoin_protocol_la_CPPFLAGS = -I${srcdir}/include ${bitcoin_CPPFLAGS} ${protobuf_CPPFLAGS} ${czmq___CPPFLAGS}
src_libbitcoin_protocol_la_LIBADD = ${bitcoin_LIBS} ${protobuf_LIBS} ${czmq___LIBS}
src_libbitcoin_protocol_la_SOURCES = \
    src/converter.cpp \
    src/interface.pb.cc \
    src/message.cpp

# local: test/libbitcoin_protocol_test
#------------------------------------------------------------------------------
if WITH_TESTS

TESTS = libbitcoin_protocol_test_runner.sh

check_PROGRAMS = test/libbitcoin_protocol_test
test_libbitcoin_protocol_test_CPPFLAGS = -I${srcdir}/include ${bitcoin_CPPFLAGS} ${protobuf_CPPFLAGS} ${czmq___CPPFLAGS}
test_libbitcoin_protocol_test_LDADD = src/libbitcoin-protocol.la ${boost_unit_test_framework_LIBS} ${bitcoin_LIBS} ${protobuf_LIBS} ${czmq___LIBS}
test_libbitcoin_protocol_test_SOURCES = \
    test/converter.cpp \
    test/main.cpp \
    test/protocol.cpp

endif WITH_TESTS

# files => ${includedir}/bitcoin
#------------------------------------------------------------------------------
include_bitcoindir = ${includedir}/bitcoin
include_bitcoin_HEADERS = \
    include/bitcoin/protocol.hpp

include_bitcoin_protocoldir = ${includedir}/bitcoin/protocol
include_bitcoin_protocol_HEADERS = \
    include/bitcoin/protocol/converter.hpp \
    include/bitcoin/protocol/define.hpp \
    include/bitcoin/protocol/interface.pb.h \
    include/bitcoin/protocol/message.hpp \
    include/bitcoin/protocol/primitives.hpp \
    include/bitcoin/protocol/version.hpp

