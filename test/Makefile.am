AUTOMAKE_OPTIONS = subdir-objects

TESTS = libbitcoin_protocol_test

check_PROGRAMS = libbitcoin_protocol_test

libbitcoin_protocol_test_CPPFLAGS = \
    -I$(srcdir)/../include \
    ${bitcoin_CFLAGS} \
    ${protobuf_CFLAGS} \
    $(czmqpp_CFLAGS) \
    -DBOOST_TEST_DYN_LINK

libbitcoin_protocol_test_SOURCES = \
    main.cpp \
    protocol.cpp

libbitcoin_protocol_test_LDADD = \
    -L$(srcdir)/../src/ -lbitcoin_protocol \
    ${bitcoin_LIBS} \
    ${protobuf_LIBS} \
    ${czmqpp_LIBS}

