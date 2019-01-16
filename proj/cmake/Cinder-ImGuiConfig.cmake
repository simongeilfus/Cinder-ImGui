if( NOT TARGET Cinder-ImGui )
  get_filename_component( IMGUI_ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE )

  list( APPEND IMGUI_SOURCES
	${IMGUI_ROOT_PATH}/src/CinderImGui.cpp
	${IMGUI_ROOT_PATH}/lib/imgui/imgui.cpp
	${IMGUI_ROOT_PATH}/lib/imgui/imgui_draw.cpp
	${IMGUI_ROOT_PATH}/lib/imgui/imgui_widgets.cpp
	)

  add_library( Cinder-ImGui ${IMGUI_SOURCES} )

  list( APPEND IMGUI_INCLUDE_DIRS
	${IMGUI_ROOT_PATH}/include
	${IMGUI_ROOT_PATH}/lib/imgui
	)
  
  target_include_directories( Cinder-ImGui PUBLIC "${IMGUI_INCLUDE_DIRS}" )

  if( NOT TARGET cinder )
	include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
	find_package( cinder REQUIRED PATHS
	  "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
	  "$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
  endif()
  
  target_link_libraries( Cinder-ImGui PRIVATE cinder )
  
endif()
