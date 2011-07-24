# CMake projet of OpenSVCDecoder
cmake_minimum_required (VERSION 2.6)
project (OpenSVCDecoder)

find_package(SDL REQUIRED)

# Add libraries
add_subdirectory(AVC/h264_baseline_decoder/lib_baseline)
add_subdirectory(AVC/h264_main_decoder/lib_main)
add_subdirectory(CommonFiles/src)
add_subdirectory(SVC/lib_svc)
add_subdirectory(SVC/libview)

# Include directories
include_directories(AVC/h264_baseline_decoder/lib_baseline AVC/h264_main_decoder/lib_main CommonFiles/src SVC/lib_svc SVC/libview/include)
 
 # Executable
IF(APPLE)
add_executable(OpenSVCDecoder SVC/build/PC.c /Library/Frameworks/SDL.framework/Resources/SDLMain.m)
ELSE(APPLE)
add_executable(OpenSVCDecoder SVC/build/PC.c)
ENDIF(APPLE)

target_link_libraries(OpenSVCDecoder ${SDL_LIBRARY} SVC_baseline AVC_main AVC_baseline DisplaySDL CommonFiles)

INSTALL(TARGETS OpenSVCDecoder
RUNTIME DESTINATION bin
PERMISSIONS
OWNER_READ OWNER_WRITE OWNER_EXECUTE
GROUP_READ GROUP_EXECUTE
WORLD_READ WORLD_EXECUTE
)

