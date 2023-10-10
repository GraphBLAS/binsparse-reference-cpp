add_library(binsparse INTERFACE)

target_link_libraries(binsparse INTERFACE ${HDF5_CXX_LIBRARIES})

target_include_directories(binsparse INTERFACE . HighFive ${HDF5_INCLUDE_DIRS})
