# Distributed under the MIT License (See accompanying file /LICENSE)
add_custom_target(py_bindings COMMENT "Build python interfaces.")

pybind11_add_module(py_mockturtle EXCLUDE_FROM_ALL mockturtle_export.cpp)
target_link_libraries(py_mockturtle PUBLIC tweedledum)
set_target_properties(py_mockturtle PROPERTIES OUTPUT_NAME mockturtle)
add_dependencies(py_bindings py_mockturtle)

pybind11_add_module(py_tweedledum EXCLUDE_FROM_ALL tweedledum_export.cpp)
target_link_libraries(py_tweedledum PUBLIC tweedledum)
set_target_properties(py_tweedledum PROPERTIES OUTPUT_NAME tweedledum)
add_dependencies(py_bindings py_tweedledum)
