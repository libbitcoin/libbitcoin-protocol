###############################################################################
#  Copyright (c) 2014-2019 libbitcoin-protocol developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################
# FindZmq
#
# Use this module by invoking find_package with the form::
#
#   find_package( Zmq
#     [version]              # Minimum version
#     [REQUIRED]             # Fail with error if zmq is not found
#   )
#
#   Defines the following for use:
#
#   zmq_FOUND                             - true if headers and requested libraries were found
#   zmq_INCLUDE_DIRS                      - include directories for zmq libraries
#   zmq_LIBRARY_DIRS                      - link directories for zmq libraries
#   zmq_LIBRARIES                         - zmq libraries to be linked
#   zmq_PKG                               - zmq pkg-config package specification.
#

if (MSVC)
    if ( Zmq_FIND_REQUIRED )
        set( _zmq_MSG_STATUS "SEND_ERROR" )
    else ()
        set( _zmq_MSG_STATUS "STATUS" )
    endif()

    set( zmq_FOUND false )
    message( ${_zmq_MSG_STATUS} "MSVC environment detection for 'zmq' not currently supported." )
else ()
    # required
    if ( Zmq_FIND_REQUIRED )
        set( _zmq_REQUIRED "REQUIRED" )
    endif()

    # quiet
    if ( Zmq_FIND_QUIETLY )
        set( _zmq_QUIET "QUIET" )
    endif()

    # modulespec
    if ( Zmq_FIND_VERSION_COUNT EQUAL 0 )
        set( _zmq_MODULE_SPEC "libzmq" )
    else ()
        if ( Zmq_FIND_VERSION_EXACT )
            set( _zmq_MODULE_SPEC_OP "=" )
        else ()
            set( _zmq_MODULE_SPEC_OP ">=" )
        endif()

        set( _zmq_MODULE_SPEC "libzmq ${_zmq_MODULE_SPEC_OP} ${Zmq_FIND_VERSION}" )
    endif()

    pkg_check_modules( zmq ${_zmq_REQUIRED} ${_zmq_QUIET} "${_zmq_MODULE_SPEC}" )
    set( zmq_PKG "${_zmq_MODULE_SPEC}" )
endif()
