function(add_example example_name)
  add_executable(${example_name} ${example_name}.cpp)
  target_link_libraries(${example_name} binsparse fmt)
endfunction()

add_example(convert_binsparse)
add_example(text2hdf5)
add_example(inspect_binsparse)
add_example(convert_matrixmarket)
