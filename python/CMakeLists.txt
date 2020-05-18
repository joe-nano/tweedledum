# Distributed under the MIT License (See accompanying file /LICENSE)

pybind11_add_module(py_tweedledum EXCLUDE_FROM_ALL bindings/mockturtle.cpp
bindings/module.cpp bindings/tweedledum.cpp)
target_link_libraries(py_tweedledum PUBLIC tweedledum)
set_target_properties(py_tweedledum PROPERTIES OUTPUT_NAME tweedledum)
