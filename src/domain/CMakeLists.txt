add_library(domain placeholder.cpp)
target_include_directories(domain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_precompile_headers(domain PRIVATE
  <string>
  <vector>
  <map>
  <memory>
)
