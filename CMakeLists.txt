cmake_minimum_required(VERSION 4.2)

project(QuizApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release")

add_executable(QuizApp
    main.cpp
    Quiz.cpp
    Quiz.h
    StartPage.cpp
    StartPage.h
    QuizPage.cpp
    QuizPage.h
    EndPage.cpp
    EndPage.h
    CrawlPage.cpp
    CrawlPage.h
    Answer.cpp
    Answer.h
    StarWarsCrawl.h StarWarsCrawl.cpp
    results.h results.cpp
)

add_custom_command(TARGET QuizApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:QuizApp>/assets"
    COMMENT "Copying assets next to executable"
)

target_link_libraries(QuizApp PRIVATE Qt6::Widgets Qt6::Multimedia)
