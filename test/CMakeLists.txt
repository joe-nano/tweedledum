# Distributed under the MIT License (See accompanying file /LICENSE)
# CMake build : tweedledum library tests
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # I wish: -fno-rtti, but percy requires it :(

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/unit/catch2")

#  All Tests
#  =============================================================================
add_custom_target(tweedledum_tests COMMENT "Build all tests.")

set(tweedledum_tests_files
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/run_tests.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/decomposition/barenco.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/decomposition/decompose.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/mapping/placement/line_placement.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/mapping/placement/random_placement.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/mapping/placement/hsat_placement.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/mapping/jit_map.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/mapping/sabre_map.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/mapping/sat_map.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/optimization/gate_cancellation.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/optimization/phase_folding.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/synthesis/oracles/xag_synth.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/synthesis/cnot_patel.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/synthesis/cnot_rz.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/synthesis/dbs.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/synthesis/diagonal_synth.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/synthesis/esop_phase_synth.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/synthesis/gray_synth.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/synthesis/linear_synth.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/synthesis/stg.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/synthesis/swap_network.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/synthesis/tbs.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/utility/asap_reschedule.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/algorithms/utility/to_logic_network.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/ir/Circuit.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/ir/Gate.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/ir/Networks.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/ir/Unitary.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/ir/Wire.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/ir/operations/operations.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/support/Angle.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/views/LayersView.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/unit/writers/write_qasm.cpp"
  )

add_executable(run_tests "${tweedledum_tests_files}")
target_link_libraries(run_tests tweedledum)
add_dependencies(tweedledum_tests run_tests)
