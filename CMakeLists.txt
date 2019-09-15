cmake_minimum_required(VERSION 3.2)

if(NOT DEFINED ENV{TRAVIS_BUILD_NUMBER})
  set(Boost_ARCHITECTURE "-x32")
  set(ENV{TRAVIS_BUILD_NUMBER} "1")
endif()

project(bulk VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

add_definitions("-std=c++11")

find_package(Boost COMPONENTS system REQUIRED)
find_package(Threads)

add_executable(bulk_server bulk_server.cpp)

set_target_properties(bulk_server PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON 
  COMPILE_OPTIONS "-Wall;-Wpedantic;-Wextra"
  #COMPILE_DEFINITIONS BOOST_ASIO_ENABLE_HANDLE_TRACKING
  INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
  LINK_FLAGS "-lpthread"
)

target_link_libraries(bulk_server
  ${Boost_LIBRARIES}
  ws2_32
  wsock32
)

install(TARGETS bulk_server RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT lonfort@mail.ru)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
