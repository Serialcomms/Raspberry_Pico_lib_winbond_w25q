add_library(lib_winbond_w25q INTERFACE)

add_subdirectory(source)

target_include_directories(lib_winbond_w25q INTERFACE

    ${CMAKE_CURRENT_SOURCE_DIR}/include

)

target_include_directories(lib_winbond_w25q PRIVATE

)

message("--------------------------------------------------------------")
message("-- Building serialcomms SPI flash library - lib_winbond_w25q  ")
message("--------------------------------------------------------------")