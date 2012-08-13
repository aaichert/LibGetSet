get_filename_component(myDir ${CMAKE_CURRENT_LIST_FILE} PATH) # get the directory where I myself am
get_filename_component(rootDir ${myDir} ABSOLUTE) # get the chosen install prefix

# set the version of myself
set(GETSET_VERSION_MAJOR 0)
set(GETSET_VERSION_MINOR 9)
set(GETSET_VERSION_PATCH )
set(GETSET_VERSION ${GETSET_VERSION_MAJOR}.${GETSET_VERSION_MINOR}.${GETSET_VERSION_PATCH} )

# what is my include directory
# set(GETSET_INCLUDE_DIRS "${rootDir}/include")
set(GETSET_INCLUDE_DIRS "${rootDir}/" "${rootDir}/GetSet" "${rootDir}/GetSetGui")

# import the exported targets
include(${myDir}/GetSetTargets.cmake)

# set the expected library variable
set(GETSET_LIBRARIES GetSet GetSetGui )
