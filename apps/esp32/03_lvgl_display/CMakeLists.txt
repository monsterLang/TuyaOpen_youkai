##
# @file CMakeLists.txt
# @brief 
#/

# APP_PATH
set(APP_PATH ${CMAKE_CURRENT_LIST_DIR})

# APP_NAME
get_filename_component(APP_NAME ${APP_PATH} NAME)

# APP_SRC
aux_source_directory(${APP_PATH}/src APP_SRC)

# 新增：收集 lvgl_ui 目录下的源文件
file(GLOB_RECURSE LVGL_UI_CUSTOM_SRC ${APP_PATH}/src/lvgl_ui/custom/*.c)
file(GLOB_RECURSE LVGL_UI_GENERATED_SRC ${APP_PATH}/src/lvgl_ui/generated/*.c)

# 合并所有源文件（原有APP_SRC + lvgl_ui的源文件）
set(ALL_APP_SRC
    ${APP_SRC}
    ${LVGL_UI_CUSTOM_SRC}
    ${LVGL_UI_GENERATED_SRC}
)

# file(GLOB_RECURSE APP_MODULE_SRCS ${APP_MODULE_PATH}/src/*.c) 

# APP_INC
set(
    APP_INC ${APP_PATH}/include
    # 新增：添加 lvgl_ui 目录的头文件路径
    ${APP_PATH}/src/lvgl_ui
    ${APP_PATH}/src/lvgl_ui/custom
    ${APP_PATH}/src/lvgl_ui/generated
)

add_subdirectory(${APP_PATH}/src/lvgl_ui)

# APP_OPTIONS
set(APP_OPTIONS "")  # -W

########################################
# Target Configure
########################################
add_library(${EXAMPLE_LIB})
message(STATUS "EXAMPLE_LIB:${APP_PATH}")

target_sources(${EXAMPLE_LIB}
    PRIVATE
        ${ALL_APP_SRC}  # not APP_SRC: add lvgl_ui code
    )

target_include_directories(${EXAMPLE_LIB}
    PRIVATE
        ${APP_INC}
    )

target_compile_options(${EXAMPLE_LIB}
    PRIVATE
        ${APP_OPTIONS}
    )

########################################
# Add subdirectory
########################################
# # 1. 添加头文件搜索路径（让编译器能找到 .h 文件）
# target_include_directories(03_lvgl_display
#     PRIVATE
#     # 添加 lvgl_ui 及其子目录的头文件路径
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/lvgl_ui
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/lvgl_ui/custom
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/lvgl_ui/generated
# )

# # 2. 收集 lvgl_ui 目录下的所有 C 源文件
# file(GLOB LVGL_UI_CUSTOM_SRC
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/lvgl_ui/custom/*.c
# )
# file(GLOB LVGL_UI_GENERATED_SRC
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/lvgl_ui/generated/*.c
# )

# # 3. 将收集的源文件添加到项目目标中（确保在 add_executable 或 target_sources 中包含）
# # 如果是已有的 add_executable，修改为：
# add_executable(03_lvgl_display
#     # 已有的源文件...
#     source/a.c
#     ${LVGL_UI_CUSTOM_SRC}
#     ${LVGL_UI_GENERATED_SRC}
# )

# # 或者如果目标已创建，使用 target_sources 添加：
# target_sources(03_lvgl_display
#     PRIVATE
#     ${LVGL_UI_CUSTOM_SRC}
#     ${LVGL_UI_GENERATED_SRC}
# )
# add_subdirectory(${APP_PATH}/src/lvgl_ui)

# target_include_directories(${APP_PATH}/src/lvgl_ui/generated ${APP_PATH}/src/lvgl_ui/custom ${APP_PATH}/src/lvgl_ui/generated/guider_customer_fonts ${APP_PATH}/src/lvgl_ui/generated/guider_fonts ${APP_PATH}/src/lvgl_ui/generated/images)
# FILE(GLOB_RECURSE SOURCES ./${APP_PATH}/src/lvgl_ui/custom/*.c ./${APP_PATH}/src/lvgl_ui/generated/*.c)
