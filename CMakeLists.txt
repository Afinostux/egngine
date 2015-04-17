#cmaking
cmake_minimum_required(VERSION 2.8.11)
project(EGNGINE)
add_library(egngine SHARED egmem egcollision egcore egentity eggl3renderer egmath egrenderer iqm model util)
target_include_directories(egngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/util})
