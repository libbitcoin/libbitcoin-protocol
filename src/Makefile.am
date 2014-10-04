AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libbitcoin_protocol.la

AM_CPPFLAGS = \
    -I$(srcdir)/../include \
    $(bitcoin_CFLAGS) \
    $(protobuf_CFLAGS) \
    $(czmqpp_CFLAGS) 

libbitcoin_protocol_la_SOURCES = \
    interface.pb.cc \
    message.cpp

libbitcoin_protocol_la_LIBADD = \
    $(bitcoin_LIBS) \
    $(protobuf_LIBS) \
    $(czmqpp_LIBS)

