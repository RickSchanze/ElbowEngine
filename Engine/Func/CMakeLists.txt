project(Func)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE FUNC_HEADER_FILES *.hpp)
file(GLOB_RECURSE FUNC_CPP_FILES *.cpp)

add_library(Func
        ${FUNC_CPP_FILES} ${FUNC_HEADER_FILES}
        UI/IconConstantName.cpp
        UI/IconConstantName.hpp
        UI/Style.cpp
        UI/Style.hpp
        UI/UIEventDispatcher.cpp
        UI/UIEventDispatcher.hpp
        UI/Widget/Layout/Layout.cpp
        UI/Widget/Layout/Layout.hpp
        UI/Widget/Layout/VerticalLayout.cpp
        UI/Widget/Layout/VerticalLayout.hpp
)

target_link_libraries(Func PRIVATE Core)
target_link_libraries(Func PRIVATE Platform)
