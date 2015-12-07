cmake_minimum_required(VERSION 3.2)
project(Slacks)

include(FindPkgConfig)

pkg_check_modules(GTK REQUIRED "gtk+-3.0")

include_directories(${GTK_INCLUDE_DIRS})
include_directories(include)
include_directories(usr/lib/curl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GTK_CFLAGS}")

add_executable(Slacks main.c include/slacks/common.h src/sstream.c include/slacks/json.h src/murmur.c src/symbol.c src/json.c src/json_parser.c include/slacks/http.h src/http.c include/slacks/slack/slack_webclient.h src/slack/slack_webclient.c)
target_link_libraries(Slacks ${GTK_LDFLAGS})
target_link_libraries(Slacks curl)