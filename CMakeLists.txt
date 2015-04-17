#cmaking
cmake_minimum_required(VERSION 2.8.11)
project(EGNGINE)
add_library(egngine SHARED egmem egcollision egcore egentity eggl3renderer egmath egrenderer iqm model util)
find_library(SDL2_LIB SDL2 ./ /usr/lib/ /usr/lib32/)
find_library(SOIL_LIB SOIL ./ /usr/lib/ /usr/lib32/)
target_link_libraries(egngine ${SDL2_LIB} ${SOIL_LIB})
