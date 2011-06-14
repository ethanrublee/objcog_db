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

  
#includes
include_directories(
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#install headers
set(DB_HEADERS
  include/objcog/db/couch.hpp
  )

INSTALL(FILES ${DB_HEADERS}
  DESTINATION include/objcog/db
  COMPONENT main
  )

#create an objcog_dbConfig.cmake for easy find_package(objcog_db)
set(objcog_db_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
set(objcog_db_LIBRARIES objcog_db)
set(objcog_db_LIBRARIES_DIR ${CMAKE_INSTALL_PREFIX}/lib)

configure_file(${CMAKE_SOURCE_DIR}/cmake/objcog_dbConfig.cmake.in 
  ${CMAKE_BINARY_DIR}/unix_install/objcog_dbConfig.cmake @ONLY)
  
#create an ectoConfig.cmake for easy find_package(ecto)
set(objcog_db_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)
set(objcog_db_LIBRARIES objcog_db)
set(objcog_db_LIBRARIES_DIR ${LIBRARY_OUTPUT_PATH})

configure_file(${CMAKE_SOURCE_DIR}/cmake/objcog_dbConfig.cmake.in 
  ${CMAKE_BINARY_DIR}/objcog_dbConfig.cmake @ONLY)
  
  
#install the ectoConfig.cmake
INSTALL(FILES ${CMAKE_BINARY_DIR}/unix_install/objcog_dbConfig.cmake
  DESTINATION share/objcog_db
  COMPONENT main
  )
add_subdirectory(doc)
add_subdirectory(src)
ENABLE_TESTING()
add_subdirectory(testing)
