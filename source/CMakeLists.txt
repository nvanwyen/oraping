cmake_policy(VERSION 2.8)
#
set(ORAPING_DEPENDENCIES)

#
colormsg(HIGREEN "Finding Threads library")
set(DL_FIND_QUIETLY TRUE)
include(FindThreads)

#
if(Threads_FOUND OR CMAKE_USE_WIN32_THREADS_INIT OR CMAKE_THREAD_LIBS_INIT)
    colormsg(HICYAN "Processing Threads library")
    list(APPEND ORAPING_DEPENDENCIES ${CMAKE_THREAD_LIBS_INIT})
    add_definitions(-DHAVE_THREADS=1)

else()
    colormsg(HIRED "Threads library not found!")
    message(FATAL_ERROR "No Thread library found")
endif()

#
if(NOT MSVC)

    set(DL_FIND_QUIETLY TRUE)

    colormsg(HIGREEN "Finding DL library")
    find_package(DL REQUIRED)

    if(DL_FOUND)

        colormsg(HICYAN "Processing DL library")

        list(APPEND ORAPING_DEPENDENCIES ${DL_LIBRARY})
        set_directory_properties(PROPERTIES
            INCLUDE_DIRECTORIES ${DL_INCLUDE_DIR})
        add_definitions(-DHAVE_DL=1)

    else()

        colormsg(HIRED "DL library not found!")
        message(FATAL_ERROR "No DL library found")

    endif()

endif()

#
colormsg(HIGREEN "Finding Boost library")
set(Boost_FIND_QUIETLY TRUE)
include(FindBoost)
find_package(Boost REQUIRED COMPONENTS thread program_options system)

#
if(Boost_FOUND)

    colormsg(HICYAN "Processing Boost library")

    get_property(ORAPING_COMPILE_DEFINITIONS
        DIRECTORY ${CMAKE_SOURCE_DIR}
        PROPERTY COMPILE_DEFINITIONS)
    
    list(APPEND ORAPING_COMPILE_DEFINITIONS "HAVE_BOOST=1")
    add_definitions(-DHAVE_BOOST=1)

    #
    if(Boost_DATE_TIME_FOUND)

        list(APPEND ORAPING_DEPENDENCIES
            ${Boost_DATE_TIME_LIBRARY})

        list(APPEND ORAPING_COMPILE_DEFINITIONS
            "HAVE_BOOST_DATE_TIME=1")
        add_definitions(-DHAVE_BOOST_DATE_TIME=1)

    else()

        add_definitions(-DHAVE_BOOST_DATE_TIME=0)

    endif()

    #
    if(Boost_SYSTEM_FOUND)

        list(APPEND ORAPING_DEPENDENCIES
            ${Boost_SYSTEM_LIBRARY})

        list(APPEND ORAPING_COMPILE_DEFINITIONS
            "HAVE_BOOST_SYSTEM=1")
        add_definitions(-DHAVE_BOOST_SYSTEM=1)

    else()

        add_definitions(-DHAVE_BOOST_SYSTEM=0)

    endif()

    #
    if(Boost_THREAD_FOUND)

        list(APPEND ORAPING_DEPENDENCIES
            ${Boost_THREAD_LIBRARY})

        list(APPEND ORAPING_COMPILE_DEFINITIONS
            "HAVE_BOOST_THREAD=1")
        add_definitions(-DHAVE_BOOST_THREAD=1)

    else()

        add_definitions(-DHAVE_BOOST_THREAD=0)

    endif()

    set_property(DIRECTORY ${CMAKE_SOURCE_DIR}
        PROPERTY COMPILE_DEFINITIONS
        ${ORAPING_COMPILE_DEFINITIONS})
    
    set_directory_properties(PROPERTIES
        INCLUDE_DIRECTORIES
        ${Boost_INCLUDE_DIRS})
    
    set_property(DIRECTORY
        ${CMAKE_SOURCE_DIR}
        PROPERTY INCLUDE_DIRECTORIES
        ${Boost_INCLUDE_DIRS})

    link_directories(${Boost_LIBRARY_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})

else()

    add_definitions(-DHAVE_BOOST=0)
    colormsg(HIRED "Boost library not found!")
    message(FATAL_ERROR "No Boost library found")

endif()

#
colormsg(HIGREEN "Finding Oracle library")
set(ORACLE_FIND_QUIETLY TRUE)
include(FindOracle)
find_package(ORACLE REQUIRED)

#
if(ORACLE_FOUND)

    colormsg(HICYAN "Processing oracle library")

    get_property(ORAPING_COMPILE_DEFINITIONS
        DIRECTORY ${CMAKE_SOURCE_DIR}
        PROPERTY COMPILE_DEFINITIONS)
    
    list(APPEND ORAPING_COMPILE_DEFINITIONS "HAVE_ORACLE=1")
    add_definitions(-DHAVE_ORACLE=1)
    
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR}
        PROPERTY COMPILE_DEFINITIONS
        ${ORAPING_COMPILE_DEFINITIONS})
    
    set_directory_properties(PROPERTIES
        INCLUDE_DIRECTORIES
        ${ORACLE_INCLUDE_DIR})
    
    set_property(DIRECTORY
        ${CMAKE_SOURCE_DIR}
        PROPERTY INCLUDE_DIRECTORIES
        ${ORACLE_INCLUDE_DIR})

    # on windows add the msvc directory
    if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows" )

        link_directories(${ORACLE_HOME}/sdk/lib/msvc)
        include_directories(${ORACLE_HOME}/sdk/include)

    else()

        link_directories(${ORACLE_LIBRARY})
        link_directories(${ORACLE_HOME}/lib ${ORACLE_HOME}/lib64)
        include_directories(${ORACLE_INCLUDE_DIR})

    endif()

else()

    add_definitions(-DHAVE_ORACLE=0)
    colormsg(HIRED "Oracle library not found!")
    message(FATAL_ERROR "No Oracle library found")

endif()

# # set version
# configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ver.h.in"
#     "${CMAKE_CURRENT_SOURCE_DIR}/ver.h")

#
colormsg(HIBLUE "Configuring output directories")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#
link_directories("${Boost_LIBRARY_DIRS}")
include_directories("${Boost_INCLUDE_DIRS}")

# other
if ( CMAKE_COMPILER_IS_GNUCXX )
    add_definitions(-g)
    add_definitions(-Wno-write-strings)
endif()    

#
colormsg(HIBLUE "Configuring ORAPING source files")
file( GLOB ORAPING_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" )

#
add_executable(oraping ${ORAPING_SOURCE_FILES})
#
if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows" )
    target_link_libraries(oraping oci.lib ociw32.lib)
else()
    target_link_libraries(oraping clntsh)
endif()

#
colormsg(HIBLUE "Configuring target installation destination")
install(TARGETS oraping DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
