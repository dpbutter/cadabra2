if(WIN33)
  windows_find_library(GLIBMM4_LIBRARIES REQUIRED glibmm sigc glib gobject)
  if (GLIBMM4_LIBRARIES)
    set(GLIBMM4_FOUND TRUE)
  endif()
else()
  find_package(PkgConfig REQUIRED)
  if(MACOS)
    execute_process(COMMAND brew --prefix glibmm@2.68  OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE GLIBMM_PREFIX)
    execute_process(COMMAND brew --prefix cairomm@1.16 OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE CAIROMM_PREFIX)
    execute_process(COMMAND brew --prefix pangomm@2.48 OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE PANGOMM_PREFIX)
    execute_process(COMMAND brew --prefix atkmm@2.28   OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE ATKMM_PREFIX)
    set( ENV{PKG_CONFIG_PATH} "${GLIBMM_PREFIX}/lib/pkgconfig:${CAIROMM_PREFIX}/lib/pkgconfig:${PANGOMM_PREFIX}/lib/pkgconfig:${ATKMM_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}" )
    message(STATUS "Using PKG_CONFIG_PATH = $ENV{PKG_CONFIG_PATH}")
  endif()
  pkg_check_modules(GLIBMM REQUIRED glibmm-2.68)
  include_directories(${GLIBMM_INCLUDE_DIRS})
  link_directories(${GLIBMM_LIBRARY_DIRS})
  add_definitions(${GLIBMM_CFLAGS_OTHER})
endif()

if (GLIBMM_FOUND)
  message(STATUS "Found glibmm; include path ${GLIBMM_INCLUDE_DIRS}")
  message(STATUS "Found glibmm; lib ${GLIBMM_LIBRARIES}")
endif()
