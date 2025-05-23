# cmake_minimum_required(VERSION ${CADABRA_CMAKE_VERSION})
project(Cadabra)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

#---------------------------------------------------------------------------
# Preamble.
#---------------------------------------------------------------------------

print_header("Configuring client-server")

set(INSTALL_LATEX_DIR "share/cadabra2")


#---------------------------------------------------------------------------
# Locate libraries.
#---------------------------------------------------------------------------

if(USE_GTK4)
  find_package(GLIBMM4  REQUIRED)
else()
  find_package(GLIBMM3  REQUIRED)
endif()
  
find_package(SQLITE3 REQUIRED)
# Don't set pthreads to required. Either we're on a platform where explict
# linking with -lpthread is the norm (e.g. Linux) and it'll be found, or we're
# on a platform that include pthreads by default (e.g. BSD, macOS) where this
# won't find anything, or we're on a road-much-less-traveled OS where the user
# can figure out what's wrong without a hard error here.
find_package(Threads)

# We need at least Boost 1.71.0 because we now use `beast` (for the
# websocket functionality) which was not stable before that version.
# Actually, it is probably not stable before 1.75.0, but if we go that
# high we cannot build on Ubuntu 20.04 anymore.
find_package(Boost 1.71.0 COMPONENTS system program_options date_time filesystem REQUIRED)

# OpenSSL needs to be linked in explicitly (probably because the boost
# material referring to it is all in headers?). If you use a cmake module,
# you will run into issues on MSYS2, so do *not* be tempted to do that
# unless you test on MSYS2 first.
set(OpenSSL_USE_STATIC_LIBS OFF)
find_package(OpenSSL REQUIRED)
message(STATUS "OPENSSL_LIBRARIES:      ${OPENSSL_LIBRARIES}")
message(STATUS "OPENSSL_SSL_LIBRARY:    ${OPENSSL_SSL_LIBRARY}")
message(STATUS "OPENSSL_CRYPTO_LIBRARY: ${OPENSSL_CRYPTO_LIBRARY}")
message(STATUS "OPENSSL_INCLUDE_DIR:    ${OPENSSL_INCLUDE_DIR}")

#---------------------------------------------------------------------------
# Enumerate input files.
#---------------------------------------------------------------------------

set(CADABRA_SERVER_SRC
  cadabra-server.cc
  Server.cc
  Snoop.cc
  websocket_client.cc
  websocket_server.cc
  ${CADABRA_CORE_DIR}/InstallPrefix.cc
  ${CADABRA_CORE_DIR}/DataCell.cc
  ${CADABRA_CORE_DIR}/Exceptions.cc
  ${CADABRA_CORE_DIR}/CdbPython.cc 
  ${CADABRA_CORE_DIR}/Stopwatch.cc 
  ${CADABRA_CORE_DIR}/pythoncdb/py_helpers.cc
  ${CADABRA_LIBS_DIR}/whereami/whereami.c
  ${CADABRA_LIBS_DIR}/base64/base64.cc
)

set(CADABRA_CLIENT_SRC
  ScriptThread.cc
  ComputeThread.cc 
  DocumentThread.cc 
  Actions.cc 
  Snoop.cc
  websocket_client.cc
  websocket_server.cc
  ${CADABRA_CORE_DIR}/DataCell.cc 
  ${CADABRA_CORE_DIR}/Exceptions.cc 
  ${CADABRA_CORE_DIR}/InstallPrefix.cc
  ${CADABRA_CORE_DIR}/Stopwatch.cc
  ${CADABRA_LIBS_DIR}/whereami/whereami.c
)
 
set(JUPYTER_KERNEL_SRC
  cadabra-jupyter-kernel.cc
  cadabra-jupyter-kernel.hh
  cadabra-jupyter-main.cc
  Server.cc
  ${CADABRA_CORE_DIR}/InstallPrefix.cc
  ${CADABRA_CORE_DIR}/DataCell.cc
  ${CADABRA_CORE_DIR}/Exceptions.cc 
  ${CADABRA_CORE_DIR}/CdbPython.cc 
  ${CADABRA_CORE_DIR}/Stopwatch.cc 
  ${CADABRA_LIBS_DIR}/whereami/whereami.c
  ${CADABRA_LIBS_DIR}/base64/base64.cc
)
  

# set(CONNECTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/connection.json)
# 
# add_custom_command(TARGET echo_kernel POST_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E
#                    copy "${CONNECTION_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/")
# 
# add_custom_command(TARGET echo_kernel POST_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E
#                  copy "${CMAKE_BINARY_DIR}/${XEUS_RUNTIME_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/")

#---------------------------------------------------------------------------
# Include directories and preprocessor directives.
#---------------------------------------------------------------------------

include_directories(
   "." 
   "${CADABRA_CORE_DIR}"
   "${CADABRA_LIBS_DIR}/pybind11/include"
#   "${CADABRA_LIBS_DIR}/websocketpp" 
   "${CADABRA_LIBS_DIR}/internal/include"
   "${CADABRA_LIBS_DIR}/whereami"
   "${CADABRA_LIBS_DIR}/base64"
   "${CADABRA_LIBS_DIR}/nlohmann"
   ${OPENSSL_INCLUDE_DIR}
   ${Boost_INCLUDE_DIRS}
   ${Python_INCLUDE_DIRS}
   ${SQLITE3_INCLUDE_DIR}
)

# add_definitions(
#    -D_WEBSOCKETPP_CPP11_STL_
#    -DBOOST_ASIO_HAS_STD_CHRONO
#    -DBOOST_BIND_GLOBAL_PLACEHOLDERS
#  )

#---------------------------------------------------------------------------
# Make targets.
#---------------------------------------------------------------------------

# add_executable(tst tst.cc)
# target_link_libraries(tst
#  ${Boost_LIBRARIES}
#  Threads::Threads
#  )

# Server executable
if(WIN32)
   message(STATUS "Using the win32 subsystem for cadabra-server")
   add_executable(cadabra-server WIN32 ${CADABRA_SERVER_SRC}) 
else()
   add_executable(cadabra-server ${CADABRA_SERVER_SRC})
endif()

# target_link_libraries(cadabra-server cadabra_server)
target_link_libraries(cadabra-server
   pybind11::embed
   ${Boost_LIBRARIES}
   ${SQLITE3_LIBRARIES}
   ${Python_LIBRARIES}
   ${GLIBMM_LIBRARIES} 
   ${OPENSSL_LIBRARIES}
   Threads::Threads
   pthread
   )
if(WIN32)
   target_link_libraries(cadabra-server ws2_32 mswsock bcrypt)
 endif()
 if(UNIX)
    if(${CMAKE_SYSTEM_NAME} MATCHES ".*BSD|DragonFly")
       target_link_libraries(cadabra-server util)
    else()
       target_link_libraries(cadabra-server dl util)
    endif()
 endif()

# Client library
add_library(cadabra_client STATIC ${CADABRA_CLIENT_SRC})
target_link_libraries(cadabra_client
  ${Boost_LIBRARIES}
  ${SQLITE3_LIBRARIES}
  ${Python_LIBRARIES}
  ${GLIBMM_LIBRARIES}
  Threads::Threads
  ${OPENSSL_LIBRARIES}
  )
if(WIN32)
  target_link_libraries(cadabra_client ws2_32 bcrypt)
endif()

# cadabra2html
add_executable(cadabra2html 
  cadabra2html.cc 
  ${CADABRA_LIBS_DIR}/base64/base64.cc
  )
target_link_libraries(cadabra2html cadabra_client)

# cadabra2latex
add_executable(cadabra2latex 
  cadabra2latex.cc 
  ${CADABRA_LIBS_DIR}/base64/base64.cc
  )
target_link_libraries(cadabra2latex cadabra_client)

# Make sure sqlite3.dll is copied into the build directory on Windows.
if (WIN32)
   add_custom_command(TARGET cadabra_client POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${SQLITE3_LIBRARIES}
      ${PROJECT_BINARY_DIR}/${CADABRA_BUILD_TYPE}/sqlite3.dll
   )
endif()


#---------------------------------------------------------------------------
# Installation.
#---------------------------------------------------------------------------

# The preamble 'notebook.tex' should be kept in sync with the one in
# ../frontend/common/texengine; this one is meant for printing so
# slightly different, but cadabra-specific macros should be copied.

if (NOT INSTALL_TARGETS_ONLY)
   install(FILES  notebook.tex  DESTINATION ${INSTALL_LATEX_DIR})
   install(FILES  notebook.html DESTINATION ${INSTALL_LATEX_DIR})
endif()
#set(INSTALL_LIB_DIR     lib           CACHE PATH "Installation directory for libraries")

install(TARGETS cadabra-server             DESTINATION ${CDB_BIN_PATH})
install(TARGETS cadabra2html cadabra2latex DESTINATION ${CDB_BIN_PATH})

# We don't need to install the libcadabra_client library as it is statically
# linked with our binaries anyway.
# install(
#  TARGETS cadabra_client 
#  RUNTIME DESTINATION bin COMPONENT runtime 
#  LIBRARY ARCHIVE DESTINATION lib 
#  LIBRARY DESTINATION lib
# )
# install(TARGETS cadabra_server LIBRARY ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)

if(ENABLE_JUPYTER)
  install(TARGETS cadabra-jupyter-kernel      DESTINATION ${CDB_BIN_PATH})
  install(FILES   kernel.json connection.json DESTINATION share/jupyter/kernels/cadabra)
endif()

if (MSVC AND NOT INSTALL_TARGETS_ONLY)
   # GTK helpers
   install(
      FILES
         "${_VCPKG_ROOT_DIR}/installed/x64-windows/tools/glib/gspawn-win64-helper.exe"    
         "${_VCPKG_ROOT_DIR}/installed/x64-windows/tools/glib/gspawn-win64-helper-console.exe"
      DESTINATION
         ${CDB_BIN_PATH}
         )
   # Python (for some reason this is not copied in)
   get_filename_component(CDB_PYTHON_BASE ${Python_LIBRARIES} NAME_WE)
   get_filename_component(CDB_PYTHON_PATH ${Python_LIBRARIES} DIRECTORY)
   message("--- Python directory: ${CDB_PYTHON_PATH}")
   string(TOLOWER ${CDB_PYTHON_BASE} CDB_PYTHON_BASE)
   message("--- Python library:   ${CDB_PYTHON_BASE}.dll")
   install(FILES "${CDB_PYTHON_PATH}/../${CDB_PYTHON_BASE}.dll"
     DESTINATION    ${CDB_BIN_PATH}
   )
endif()

# manual pages
if(NOT MSVC)
  install(
    FILES
    ../man/man1/cadabra-server.1
    DESTINATION
    share/man/man1
    )
endif()
