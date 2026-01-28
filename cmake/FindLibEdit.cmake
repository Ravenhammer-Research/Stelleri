# Minimal Find module for libedit (editline)
# Sets LibEdit_FOUND, LibEdit_INCLUDE_DIRS, LibEdit_LIBRARIES

find_path(LibEdit_INCLUDE_DIR
  NAMES editline/readline.h readline.h
  PATH_SUFFIXES editline include
)

find_library(LibEdit_LIBRARY
  NAMES edit libedit
)

if(LibEdit_INCLUDE_DIR AND LibEdit_LIBRARY)
  set(LibEdit_FOUND TRUE)
  set(LibEdit_INCLUDE_DIRS ${LibEdit_INCLUDE_DIR})
  set(LibEdit_LIBRARIES ${LibEdit_LIBRARY})

  if(NOT TARGET Editline::Editline)
    add_library(Editline::Editline UNKNOWN IMPORTED)
    set_target_properties(Editline::Editline PROPERTIES
      IMPORTED_LOCATION "${LibEdit_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${LibEdit_INCLUDE_DIR}"
    )
  endif()
else()
  set(LibEdit_FOUND FALSE)
endif()
# FindLibEdit.cmake - locate libedit (editline) library and headers
# Provides:
#  LibEdit_FOUND - True if libedit was found
#  Editline_INCLUDE_DIR - include dir containing editline/readline.h
#  Editline_LIBRARY - path to libedit library

find_path(Editline_INCLUDE_DIR
    NAMES editline/readline.h
    PATHS /usr/include /usr/local/include /usr/include/editline /usr/local/include/editline
)

find_library(Editline_LIBRARY
    NAMES edit libedit editline
    PATHS /usr/lib /usr/local/lib /usr/lib64 /usr/local/lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibEdit
    REQUIRED_VARS Editline_LIBRARY Editline_INCLUDE_DIR
)

if(LibEdit_FOUND AND NOT TARGET Editline::Editline)
    add_library(Editline::Editline UNKNOWN IMPORTED)
    set_target_properties(Editline::Editline PROPERTIES
        IMPORTED_LOCATION "${Editline_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Editline_INCLUDE_DIR}"
    )
endif()
