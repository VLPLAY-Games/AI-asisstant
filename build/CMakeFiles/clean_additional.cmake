# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "AI-assistant_autogen"
  "CMakeFiles\\AI-assistant_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AI-assistant_autogen.dir\\ParseCache.txt"
  )
endif()
