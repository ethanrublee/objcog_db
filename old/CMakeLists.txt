cmake_minimum_required(VERSION 2.8)
project(objcog_db)

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

#set flags so that we have awesome compile time errors on all warnings
#notice the -Wl,--no-undefined helps with making sure all symbols are found during
#link time.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wl,--no-undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DBOOST_DISABLE_ASSERTS")


set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


#deps
find_package(Boost COMPONENTS
  system
  serialization
  thread
  filesystem
  regex
  REQUIRED
  )
find_package(CURL REQUIRED)
add_executable( couch_surfing couch_surfing.cpp)
target_link_libraries(couch_surfing ${Boost_LIBRARIES} ${CURL_LIBRARIES})
