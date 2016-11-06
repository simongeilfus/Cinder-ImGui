if(NOT TARGET Cinder-ImGui)
  get_filename_component( CINDER-IMGUI_PATH "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
	get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE )
  list( APPEND CINDER-IMGUI_SOURCES
    ${CINDER-IMGUI_PATH}/src/CinderImGui.cpp
    ${CINDER-IMGUI_PATH}/lib/imgui/imgui.cpp
    ${CINDER-IMGUI_PATH}/lib/imgui/imgui_draw.cpp
    ${CINDER-IMGUI_PATH}/lib/imgui/imgui_demo.cpp
    )
  add_library(Cinder-ImGui ${CINDER-IMGUI_SOURCES})
  target_include_directories( Cinder-ImGui PUBLIC
    "${CINDER-IMGUI_PATH}/include"
    "${CINDER-IMGUI_PATH}/lib/imgui")
	target_include_directories( Cinder-ImGui SYSTEM BEFORE PUBLIC "${CINDER_PATH}/include" )
	if( NOT TARGET cinder )
		include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
		find_package( cinder REQUIRED PATHS
		  "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
		  "$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
	endif()
  target_link_libraries(Cinder-ImGui PRIVATE cinder)
endif()
