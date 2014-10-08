#
# pkg-config
#
pkgconfig_DATA = libbitcoin-protocol.pc

# ?
ACLOCAL_AMFLAGS = -I m4

#
# headers
#
bitcoin_includedir = $(includedir)/bitcoin
bitcoin_include_HEADERS = \
    include/bitcoin/protocol.hpp

bitcoin_protocol_includedir = $(includedir)/bitcoin/protocol
bitcoin_protocol_include_HEADERS = \
    include/bitcoin/protocol/define.hpp \
    include/bitcoin/protocol/interface.pb.h \
    include/bitcoin/protocol/message.hpp \
    include/bitcoin/protocol/primitives.hpp

#
# binaries
#
lib_LTLIBRARIES = libbitcoin-protocol.la

libbitcoin-protocol_la_CPPFLAGS = \
    -I$(srcdir)/include \
    $(bitcoin_CFLAGS) \
    $(protobuf_CFLAGS) \
    $(czmqpp_CFLAGS) 

libbitcoin-protocol_la_SOURCES = \
    src/interface.pb.cc \
    src/message.cpp

libbitcoin-protocol_la_LIBADD = \
    $(bitcoin_LIBS) \
    $(protobuf_LIBS) \
    $(czmqpp_LIBS)

#
# tests
#
TESTS = libbitcoin-protocol-test

check_PROGRAMS = libbitcoin-protocol-test

libbitcoin-protocol-test_CPPFLAGS = \
    -I$(srcdir)/include \
    ${bitcoin_CFLAGS} \
    ${protobuf_CFLAGS} \
    $(czmqpp_CFLAGS) \
    -DBOOST_TEST_DYN_LINK

libbitcoin-protocol-test_SOURCES = \
    test/main.cpp \
    test/protocol.cpp

libbitcoin-protocol-test_LDADD = \
    libbitcoin-protocol.la \
    ${bitcoin_LIBS} \
    ${protobuf_LIBS} \
    ${czmqpp_LIBS}
