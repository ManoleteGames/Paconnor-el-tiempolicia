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
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/images/logo/ILOGO.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/images/menu/IMENU.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/images/ui/IUI.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/images/scn0/ISCN0.DAT
    ${CMAKE_BINARY_DIR}/    
)

add_custom_target(maps
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/maps/scn0/MAPSCN0.DAT
    ${CMAKE_BINARY_DIR}/    
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/maps/scn1/MAPSCN1.DAT
    ${CMAKE_BINARY_DIR}/    
)

add_custom_target(sounds  
     COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sounds/MSCENE0.DAT
    ${CMAKE_BINARY_DIR}/
     COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sounds/MSCENE1.DAT
    ${CMAKE_BINARY_DIR}/
)

add_custom_target(tilesets
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/tilesets/scn0/TSCN0.DAT
    ${CMAKE_BINARY_DIR}/    
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/tilesets/scn1/TSCN1.DAT
    ${CMAKE_BINARY_DIR}/    
)

add_custom_target(sprites
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/actor/ACTOR.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/cursor/CURSOR.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/misc/MISC.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/bullets/BULLETS.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/panel/PANEL.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/enemies/ENEMY.DAT
    ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/buttons/BUTTON.DAT
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