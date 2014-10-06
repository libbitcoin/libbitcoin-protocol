#
# pkg-config
#
pkgconfig_DATA = libbitcoin_protocol.pc

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
lib_LTLIBRARIES = libbitcoin_protocol.la

libbitcoin_protocol_la_CPPFLAGS = \
    -I$(srcdir)/include \
    $(bitcoin_CFLAGS) \
    $(protobuf_CFLAGS) \
    $(czmqpp_CFLAGS) 

libbitcoin_protocol_la_SOURCES = \
    src/interface.pb.cc \
    src/message.cpp

libbitcoin_protocol_la_LIBADD = \
    $(bitcoin_LIBS) \
    $(protobuf_LIBS) \
    $(czmqpp_LIBS)

#
# tests
#
TESTS = libbitcoin_protocol_test

check_PROGRAMS = libbitcoin_protocol_test

libbitcoin_protocol_test_CPPFLAGS = \
    -I$(srcdir)/include \
    ${bitcoin_CFLAGS} \
    ${protobuf_CFLAGS} \
    $(czmqpp_CFLAGS) \
    -DBOOST_TEST_DYN_LINK

libbitcoin_protocol_test_SOURCES = \
    test/main.cpp \
    test/protocol.cpp

libbitcoin_protocol_test_LDADD = \
    libbitcoin_protocol.la \
    ${bitcoin_LIBS} \
    ${protobuf_LIBS} \
    ${czmqpp_LIBS}
