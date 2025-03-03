function(download_data url file_name)
  set(DATASET_ARCHIVE ${CMAKE_BINARY_DIR}/data/${file_name})

  file(DOWNLOAD
       ${url}
       ${DATASET_ARCHIVE})

  string(REPLACE
        ".tar.gz" ""
        DATASET_DIR
        ${DATASET_ARCHIVE})

  cmake_path(REMOVE_FILENAME DATASET_DIR)

  file(ARCHIVE_EXTRACT INPUT ${DATASET_ARCHIVE} DESTINATION ${DATASET_DIR})

  message(STATUS "Downloading file ${DATASET_ARCHIVE}, extracting to ${DATASET_DIR}.")
endfunction()

enable_testing()

add_executable(
  binsparse-tests
  csr_test.cpp
  coo_test.cpp
)

target_link_libraries(binsparse-tests binsparse fmt GTest::gtest_main)

download_data(https://suitesparse-collection-website.herokuapp.com/MM/DIMACS10/chesapeake.tar.gz
              chesapeake.tar.gz)

download_data(https://suitesparse-collection-website.herokuapp.com/MM/HB/1138_bus.tar.gz
              1138_bus.tar.gz)

download_data(https://suitesparse-collection-website.herokuapp.com/MM/Belcastro/mouse_gene.tar.gz
              mouse_gene.tar.gz)

include(GoogleTest)
gtest_discover_tests(binsparse-tests)
