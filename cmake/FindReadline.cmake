# Minimal Find module for GNU Readline
# Sets Readline_FOUND, Readline_INCLUDE_DIRS, Readline_LIBRARIES

find_path(Readline_INCLUDE_DIR
  NAMES readline/readline.h readline.h
  PATH_SUFFIXES include
)

find_library(Readline_LIBRARY
  NAMES readline
)

if(Readline_INCLUDE_DIR AND Readline_LIBRARY)
  set(Readline_FOUND TRUE)
  set(Readline_INCLUDE_DIRS ${Readline_INCLUDE_DIR})
  set(Readline_LIBRARIES ${Readline_LIBRARY})

  if(NOT TARGET Readline::Readline)
    add_library(Readline::Readline UNKNOWN IMPORTED)
    set_target_properties(Readline::Readline PROPERTIES
      IMPORTED_LOCATION "${Readline_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${Readline_INCLUDE_DIR}"
    )
  endif()
else()
  set(Readline_FOUND FALSE)
endif()
