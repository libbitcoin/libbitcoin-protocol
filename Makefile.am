#
# pkg-config
#
pkgconfig_DATA = libbitcoin-protocol.pc

# look for macros in the m4 subdirectory
ACLOCAL_AMFLAGS = -I m4

#
# headers
#
bitcoin_includedir = $(includedir)/bitcoin
bitcoin_include_HEADERS = \
    include/bitcoin/protocol.hpp

bitcoin_protocol_includedir = $(includedir)/bitcoin/protocol
bitcoin_protocol_include_HEADERS = \
    include/bitcoin/protocol/converter.hpp \
    include/bitcoin/protocol/define.hpp \
    include/bitcoin/protocol/interface.pb.h \
    include/bitcoin/protocol/message.hpp \
    include/bitcoin/protocol/primitives.hpp

#
# libraries
#
lib_LTLIBRARIES = src/libbitcoin-protocol.la

src_libbitcoin_protocol_la_CPPFLAGS = \
    -I$(srcdir)/include \
    $(bitcoin_CFLAGS) \
    $(protobuf_CFLAGS) \
    $(czmqpp_CFLAGS) 

src_libbitcoin_protocol_la_SOURCES = \
    src/converter.cpp \
    src/interface.pb.cc \
    src/message.cpp

src_libbitcoin_protocol_la_LIBADD = \
    $(bitcoin_LIBS) \
    $(protobuf_LIBS) \
    $(czmqpp_LIBS)

#
# tests
#
TESTS = test/libbitcoin_protocol_test

check_PROGRAMS = test/libbitcoin_protocol_test

test_libbitcoin_protocol_test_CPPFLAGS = \
    -I$(srcdir)/include \
    ${bitcoin_CFLAGS} \
    ${protobuf_CFLAGS} \
    $(czmqpp_CFLAGS) \
    -DBOOST_TEST_DYN_LINK

test_libbitcoin_protocol_test_SOURCES = \
    test/converter.cpp \
    test/main.cpp \
    test/protocol.cpp

test_libbitcoin_protocol_test_LDADD = \
    src/libbitcoin-protocol.la \
    ${bitcoin_LIBS} \
    ${protobuf_LIBS} \
    ${czmqpp_LIBS}
