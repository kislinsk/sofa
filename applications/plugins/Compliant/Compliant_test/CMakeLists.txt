cmake_minimum_required(VERSION 3.1)

project(Compliant_test)

set(HEADER_FILES
    Compliant_test.h
)

set(SOURCE_FILES
    AssembledRigidRigidMapping_test.cpp
    Assembly_test.cpp
    CompliantImplicitSolver_test.cpp
    CompliantNLImplicitSolver_test.cpp
    DampedOscillator_test.cpp
    RigidJointMapping_test.cpp
    DotProductMapping_test.cpp
    NormalizationMapping_test.cpp
    SafeDistanceMapping_test.cpp
)

set(PYTHON_FILES
    python/AffineMultiMapping.py
    python/Example.py
    python/GenerateRigid.py
    python/LambdaPropagation.py
    python/UniformCompliance.py
    python/friction.py
)

find_package(SofaPython QUIET)

if(SofaPython_FOUND)
    add_definitions("-DCOMPLIANT_TEST_PYTHON_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/python\"")
    list(APPEND SOURCE_FILES python_test_list.cpp)
endif()

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${PYTHON_FILES})
target_link_libraries(${PROJECT_NAME} Compliant SofaTest SofaGTestMain)
if(SofaPython_FOUND)
    target_link_libraries(${PROJECT_NAME} SofaPython)
endif()

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
