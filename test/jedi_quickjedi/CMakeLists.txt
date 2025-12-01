# JSON label tests.

# Specify list of test strings.
set(jedi_quickjedi_test_list
  jedi_quickjedi_test
  jedi_quickjedi_bounds_test
)

# Add custom target for building all label intersection tests.
add_custom_target(jedi_quickjedi_all_tests)

# Specify test strings in the loop.
foreach(loop_var ${jedi_quickjedi_test_list})
  # Determine data file name - bounds test uses the same data as regular test
  if(${loop_var} STREQUAL "jedi_quickjedi_bounds_test")
    set(data_file "jedi_quickjedi_test_data.txt")
  else()
    set(data_file "${loop_var}_data.txt")
  endif()
  
  # Copy data file
  file(
    COPY        ${data_file}
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
  )
  # Copy results file if it exists
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}_results.txt)
    file(
      COPY        ${loop_var}_results.txt
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )
  endif()
  
  add_executable(
    ${loop_var}_driver
    ${loop_var}.cc
  )
  target_link_libraries(
    ${loop_var}_driver
    TreeSimilarity
  )
  
  # Set test arguments based on whether results file exists
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}_results.txt)
    add_test(
      NAME    ${loop_var}
      COMMAND ${loop_var}_driver
      ${data_file} ${loop_var}_results.txt
    )
  else()
    add_test(
      NAME    ${loop_var}
      COMMAND ${loop_var}_driver
      ${data_file}
    )
  endif()
  
  # Add this target to jedi_quickjedi_all_tests target.
  add_dependencies(
    jedi_quickjedi_all_tests
    ${loop_var}_driver
  )
endforeach(loop_var)

