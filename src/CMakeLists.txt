add_subdirectory(json_spirit)

find_package(CURL REQUIRED)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_library(objcog_db SHARED couch.cpp)
target_link_libraries(objcog_db  ${Boost_LIBRARIES} ${CURL_LIBRARIES} json_spirit)
