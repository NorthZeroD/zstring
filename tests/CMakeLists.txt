file(GLOB CPP_FILES CONFIGURE_DEPENDS "*.cpp")

foreach(src ${CPP_FILES})
    get_filename_component(target ${src} NAME_WLE)
    add_executable(${target} ${src})
    target_include_directories(${target} PRIVATE ../src)
endforeach()
