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
  python
  system
  serialization
  thread
  filesystem
  regex
  )
find_package(PythonLibs)
find_package(OpenCV)

#includes
include_directories(
  ${Boost_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(srcs src/document.cpp)

if(OpenCV_FOUND)
set(srcs ${srcs} src/cv.cpp src/common_inserts.cpp)
endif()

add_library(objcog_db SHARED ${srcs})


target_link_libraries(objcog_db 
  ${Boost_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${OpenCV_LIBS}
  )

  
macro(bpmodule NAME)

  add_library(${NAME}_bpmodule SHARED
    ${ARGN}
    )

  set_target_properties(${NAME}_bpmodule
    PROPERTIES
    OUTPUT_NAME ${NAME}
    PREFIX ""
    #SUFFIX ".pyd"
    )

  target_link_libraries(${NAME}_bpmodule
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
    )
endmacro()

macro(link_bpmodule NAME)
  target_link_libraries(${NAME}_bpmodule
    ${ARGN}
  )
endmacro()


bpmodule(objcog_db src/pywrap.cpp)
link_bpmodule(objcog_db objcog_db)

#if(OpenCV_FOUND)
#add_executable(mat_test test/mat_test.cpp)
#target_link_libraries(mat_test objcog_db  ${Boost_LIBRARIES} ${OpenCV_LIBS})
#endif()

add_subdirectory(doc)
