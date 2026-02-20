cmake_minimum_required(VERSION 3.2)
project(dos-app)

set(CMAKE_GENERATOR Ninja)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
add_compile_options(-Wall -std=gnu99 -save-temps)

file(GLOB sources CONFIGURE_DEPENDS "src/*.c" "src/*.cpp" "src/*.h" "src/**/*.c" "src/**/*.cpp" "src/**/*.h" "src/**/**/*.c" "src/**/**/*.cpp" "src/**/**/*.h" )

add_executable(paconnor ${sources})

add_custom_target(extender
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/utils/CWSDPMI.EXE
    ${CMAKE_BINARY_DIR}/   
)

add_custom_target(binaries
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/binaries/BINARIES.DAT
    ${CMAKE_BINARY_DIR}/   
)

add_custom_target(fonts
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/FONTS.DAT
    ${CMAKE_BINARY_DIR}/
)

add_custom_target(images
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/images/LOGO.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/images/INTRO.DAT
    ${CMAKE_BINARY_DIR}/
)

add_custom_target(maps
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/maps/office/OFFICE.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/maps/sewer/SEWER.DAT
    ${CMAKE_BINARY_DIR}/
)

add_custom_target(sounds
     COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sounds/MINTRO.DAT
    ${CMAKE_BINARY_DIR}/
      COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sounds/MSCENE1.DAT
    ${CMAKE_BINARY_DIR}/
)

add_custom_target(tilesets
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/tilesets/OFFICET.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/tilesets/SEWERT.DAT
    ${CMAKE_BINARY_DIR}/
)

add_custom_target(sprites
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/ACTOR.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/CURSOR.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/MISC.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/BULLETS.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/PANEL.DAT
    ${CMAKE_BINARY_DIR}/
     COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/ENEMY.DAT
    ${CMAKE_BINARY_DIR}/
)

add_custom_target(palettes
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/palettes/PALETTES.DAT
    ${CMAKE_BINARY_DIR}/
)

add_custom_target(dialogs
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/text/DIALOG.DAT
    ${CMAKE_BINARY_DIR}/
)

#add_dependencies(main assets)
add_dependencies(paconnor extender binaries images fonts maps tilesets sprites palettes dialogs sounds)