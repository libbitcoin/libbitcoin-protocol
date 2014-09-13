AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libbitcoin_protocol.la

AM_CPPFLAGS = \
    -I$(srcdir)/../include \
    $(libbitcoin_CFLAGS) \
    $(protobuf_CFLAGS)

libbitcoin_protocol_la_SOURCES = \
    interface.pb.cc

libbitcoin_protocol_la_LIBADD = \
    $(libbitcoin_LIBS) \
    $(protobuf_LIBS)

