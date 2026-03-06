cmake_minimum_required(VERSION 3.10)
project(ArcheroClone)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к SFML в проекте
set(SFML_DIR "${CMAKE_SOURCE_DIR}/libs/SFML-3.0.2")

# Подключаем заголовочные файлы
include_directories(${SFML_DIR}/include ${CMAKE_SOURCE_DIR}/include)

# Подключаем библиотеки
link_directories(${SFML_DIR}/lib)

# Собираем все исходники
file(GLOB SOURCES "src/*.cpp")

# Создаём исполняемый файл
add_executable(ArcheroClone ${SOURCES})

# Определяем платформу и подключаем нужные библиотеки
if(WIN32)
    # Windows
    target_link_libraries(ArcheroClone
        sfml-graphics
        sfml-window
        sfml-audio
        sfml-system
    )
    
    # Копируем DLL в папку с exe
    add_custom_command(TARGET ArcheroClone POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SFML_DIR}/bin/sfml-graphics-3.dll
            ${SFML_DIR}/bin/sfml-window-3.dll
            ${SFML_DIR}/bin/sfml-audio-3.dll
            ${SFML_DIR}/bin/sfml-system-3.dll
            $<TARGET_FILE_DIR:ArcheroClone>)
    
elseif(APPLE)
    # macOS
    target_link_libraries(ArcheroClone
        sfml-graphics
        sfml-window
        sfml-audio
        sfml-system
    )
    
    # Для Mac дополнительные флаги
    set_target_properties(ArcheroClone PROPERTIES
        MACOSX_RPATH TRUE
        INSTALL_RPATH "@executable_path/../libs/SFML-3.0.2/lib")
endif()

# Копируем ассеты (для всех платформ)
add_custom_command(TARGET ArcheroClone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ArcheroClone>/assets
)
