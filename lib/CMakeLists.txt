# Initialize cmake project
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)
PROJECT(WooAlphabettySolver CXX)

# Camera Option
OPTION(CAMERA_SUPPORT "Enbale camera support if active" OFF)

# Include common configuration
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src/Data)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src/Solver)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src/Widget)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src/GameImageProcessing)

# Configure Qt package
FIND_PACKAGE( OpenCV REQUIRED )
FIND_PACKAGE( Qt5    REQUIRED Core Widgets)
SET(CMAKE_AUTOMOC ON)
ADD_DEFINITIONS(
    ${Qt5Core_DEFINITIONS}
    ${Qt5Widgets_DEFINITIONS}
    )
INCLUDE_DIRECTORIES(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    )
QT5_ADD_RESOURCES(
    RESOURCE_ADDED ${CMAKE_CURRENT_LIST_DIR}/rsc/list.qrc
    )

if(CAMERA_SUPPORT)
    MESSAGE("== CAMERA_SUPPORT enabled")
    FIND_PACKAGE(Qt5 REQUIRED Multimedia MultimediaWidgets )
    ADD_DEFINITIONS(-DWOO_CAMERA_SUPPORT)
else()
    MESSAGE("== CAMERA_SUPPORT disabled")
endif()

# Configure executable

SET(LIST_SOURCE
    src/main.cxx
    src/Application.cxx

    src/Data/Grid.cxx
    src/Data/Tile.cxx

    src/Solver/Dico.cxx
    src/Solver/Solver.cxx
    src/Solver/SolverBlock.cxx
    src/Solver/GridWidget.cxx
    src/Solver/TileWidget.cxx
    src/Solver/TileEditor.cxx
    src/Solver/TileEditorButton.cxx

    src/Widget/VisualTeacher.cxx
    src/Widget/InteractiveScene.cxx

    src/GameImageProcessing/GridTeacher.cxx
    src/GameImageProcessing/LevelGrid.cxx
    src/GameImageProcessing/TileAnalyser.cxx
    src/GameImageProcessing/CvBridge.cxx
    src/GameImageProcessing/StatsTool.cxx
    src/GameImageProcessing/ToolGridBuilder.cxx
    src/GameImageProcessing/ImageDialog.cxx
    src/GameImageProcessing/GameImageProcessor.cxx
    ${RESOURCE_ADDED}
    )

ADD_EXECUTABLE(
    woo-alphabetty-solver
    ${LIST_SOURCE}
    )

SET_PROPERTY(
    TARGET   woo-alphabetty-solver
    PROPERTY CXX_STANDARD 14
    )

TARGET_LINK_LIBRARIES(
    woo-alphabetty-solver
    ${OpenCV_LIBS}
    )

QT5_USE_MODULES(
    woo-alphabetty-solver
    Core Widgets Multimedia MultimediaWidgets
    )
