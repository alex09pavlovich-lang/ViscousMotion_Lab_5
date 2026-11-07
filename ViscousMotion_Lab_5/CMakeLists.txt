# CMakeList.txt : CMake project for ViscousMotion_Lab_5, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (ViscousMotion_Lab_5 "ViscousMotion_Lab_5.cpp" "ViscousMotion_Lab_5.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ViscousMotion_Lab_5 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
