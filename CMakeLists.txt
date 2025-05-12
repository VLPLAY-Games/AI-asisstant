cmake_minimum_required(VERSION 3.17)
project(AI-assistant VERSION 0.9.0 LANGUAGES CXX)

# Базовые настройки Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core)

# Определения для Windows
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN)
endif()

# Список исходных файлов
set(PROJECT_SOURCES
    AI-assistant.cpp
    src/log.cpp
    src/config.cpp
    src/recorder.cpp
    src/recognizer.cpp
    src/tts.cpp
    src/kobold_client.cpp
    src/device_connection.cpp
    include/log.h
    include/config.h
    include/recorder.h
    include/recognizer.h
    include/tts.h
    include/kobold_client.h
    include/device_connection.h
    include/nlohmann/json.hpp
    # Подключаем MainWindow (UI) и соответствующие файлы
    mainwindow.cpp
    mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui
)

# Включение директорий
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Создаём директорию files в build и копируем config.cfg с синхронизацией
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/files)
configure_file(${CMAKE_SOURCE_DIR}/files/config.cfg ${CMAKE_BINARY_DIR}/files/config.cfg COPYONLY)

# Создание исполняемого файла
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AI-assistant
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(AI-assistant
        ${PROJECT_SOURCES}
    )
endif()

# Подключение библиотек Qt
target_link_libraries(AI-assistant PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
)

# Поиск и подключение PortAudio
find_package(portaudio QUIET)
if(portaudio_FOUND)
    target_link_libraries(AI-assistant PRIVATE portaudio)
else()
    message(FATAL_ERROR "PortAudio not found. Please install it using vcpkg or specify the correct path.")
endif()

# Поиск и подключение CURL
find_package(CURL QUIET)
if(CURL_FOUND)
    target_include_directories(AI-assistant PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries(AI-assistant PRIVATE ${CURL_LIBRARIES})
else()
    message(FATAL_ERROR "CURL not found. Please install it using vcpkg or your package manager.")
endif()

# Windows-специфичные библиотеки
if(WIN32)
    target_link_libraries(AI-assistant PRIVATE winmm ole32 sapi ws2_32)
endif()

# Настройки для macOS (опционально)
if(APPLE)
    set_target_properties(AI-assistant PROPERTIES
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
    )
endif()

# Установка
include(GNUInstallDirs)
install(TARGETS AI-assistant
    BUNDLE DESTINATION . 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Финальная настройка для Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AI-assistant)
endif()
