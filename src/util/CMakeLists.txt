add_library(util placeholder.cpp)
target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_precompile_headers(util PRIVATE
  <string>
  <vector>
  <map>
  <memory>
)
