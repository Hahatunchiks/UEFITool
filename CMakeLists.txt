cmake_minimum_required(VERSION 3.22)
project(uefi_image_parser)

set(CMAKE_CXX_STANDARD 11)
add_executable(uefi_image_parser main.cpp
        UEFIParser.h UEFIParser.cpp
        UEFIImage.h
        Region.h
        parser/JSONParser.h parser/JSONParser.cpp)