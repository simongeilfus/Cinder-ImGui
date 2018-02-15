if( NOT TARGET Cinder-ImGui )
  get_filename_component( IMGUI_ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE )

  if(MSVC)
	  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  else()
	  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
  endif()

  if(WIN32)
	  set(CLIPBOARD_LITE_PLATFORM_SRC
			  ${IMGUI_ROOT_PATH}/lib/clipboard_lite/include/windows/Clipboard.h
			  ${IMGUI_ROOT_PATH}/lib/clipboard_lite/src/windows/Clipboard.cpp
			  )
	  set(CLIPBOARD_LITE_PLATFORM_INC
			  ${IMGUI_ROOT_PATH}/lib/clipboard_lite/include/windows
			  )
  elseif(APPLE)
	  set(CLIPBOARD_LITE_PLATFORM_SRC
			  ${IMGUI_ROOT_PATH}/lib/clipboard_lite/include/ios/Clipboard.h
			  ${IMGUI_ROOT_PATH}/lib/clipboard_lite/src/ios/Clipboard.mm
			  )

	  set(CLIPBOARD_LITE_PLATFORM_INC
			  ${IMGUI_ROOT_PATH}/lib/clipboard_lite/include/ios
			  )
  else()
	  set(CLIPBOARD_LITE_PLATFORM_SRC
			  ${IMGUI_ROOT_PATH}/lib/clipboard_lite/include/linux/Clipboard.h
			  ${IMGUI_ROOT_PATH}/lib/clipboard_lite/src/linux/Clipboard.cpp
			  )
	  find_package(X11 REQUIRED)

	  set(CLIPBOARD_LITE_PLATFORM_INC
			  ${IMGUI_ROOT_PATH}/lib/clipboard_lite/include/linux
			  ${X11_INCLUDE_DIR}
			  )
  endif()
	message("clipboard folder=${CLIPBOARD_LITE_PLATFORM_INC}")
  list( APPEND IMGUI_SOURCES
	${IMGUI_ROOT_PATH}/src/CinderImGui.cpp
	${IMGUI_ROOT_PATH}/lib/imgui/imgui.cpp
	${IMGUI_ROOT_PATH}/lib/imgui/imgui_draw.cpp
	${IMGUI_ROOT_PATH}/lib/imgui/imgui_demo.cpp
	)
list(APPEND CLIPBOARD_LITE_SOURCES
	${IMGUI_ROOT_PATH}/lib/clipboard_lite/include/Clipboard_Lite.h 
	${IMGUI_ROOT_PATH}/lib/clipboard_lite/src/Clipboard_Lite.cpp 
	${CLIPBOARD_LITE_PLATFORM_SRC}
  )
list(APPEND CLIPBOARD_LITE_INCLUDE
  ${CLIPBOARD_LITE_PLATFORM_INC}
	${IMGUI_ROOT_PATH}/lib/clipboard_lite/include
  )

add_library( Cinder-ImGui
  ${IMGUI_SOURCES}
  ${CLIPBOARD_LITE_SOURCES}
  )

  list( APPEND IMGUI_INCLUDE_DIRS
	${IMGUI_ROOT_PATH}/include
	${IMGUI_ROOT_PATH}/lib/imgui
	)
  
target_include_directories( Cinder-ImGui PUBLIC
 	${IMGUI_INCLUDE_DIRS}
  ${CLIPBOARD_LITE_INCLUDE}
	)

  if( NOT TARGET cinder )
	include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
	find_package( cinder REQUIRED PATHS
	  "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
	  "$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
  endif()
  
  target_link_libraries( Cinder-ImGui PRIVATE cinder )
  
endif()
