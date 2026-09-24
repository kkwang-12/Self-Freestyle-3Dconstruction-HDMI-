# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\Construction3DHMI_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Construction3DHMI_autogen.dir\\ParseCache.txt"
  "Construction3DHMI_autogen"
  )
endif()
