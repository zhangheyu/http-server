cmake_minimum_required(VERSION 2.8)
#########################################################################

# Basic environment setting
SET(BUILD_PROJECT_NAME "http_server")

#########################################################################
#INCLUDE_DIRECTORIES(${LIBVZNET_INCLUDE_DIR})
MESSAGE(STATUS "Print the include directores")
get_property(inc_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(inc_dir ${inc_dirs})
  message(STATUS "    include directory='${inc_dir}'")
endforeach()

#########################################################################
#Step 2 : Add library directories
MESSAGE(STATUS "Step 2 : Add library directories")
#LINK_DIRECTORIES(${LIBVZNET_LIBRARY_DIR})
MESSAGE(STATUS "Print the link directores")
get_property(link_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
foreach(link_dir ${link_dirs})
  message(STATUS "    link directory='${link_dir}'")
endforeach()

if(WIN32)
  ADD_DEFINITIONS(
  )
else()
  ADD_DEFINITIONS(
  )
endif()
#########################################################################
#Step 3 : Add code source
SET(SRC_LIST 
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CHttpServerMgr.h
  ${CMAKE_CURRENT_SOURCE_DIR}/CHttpServerMgr.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/HttpServer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HttpServer.cpp
  
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/cbase64.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/cbase64.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/httppost.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/httppost.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/HttpServerSoapService.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/HttpServerSoapService.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/nsmap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/soapC.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/soapH.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/soapStub.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/thread_setup.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/stdsoap2.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/stdsoap2.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/threads.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/threads.cpp
)
SOURCE_GROUP(${BUILD_PROJECT_NAME} main
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CHttpServerMgr.h
  ${CMAKE_CURRENT_SOURCE_DIR}/CHttpServerMgr.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/HttpServer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HttpServer.cpp
)

SOURCE_GROUP(${BUILD_PROJECT_NAME} gsoap
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/cbase64.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/cbase64.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/httppost.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/httppost.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/HttpServerSoapService.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/HttpServerSoapService.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/nsmap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/soapC.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/soapH.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/soapStub.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/thread_setup.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/stdsoap2.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/stdsoap2.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/threads.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gsoap/threads.cpp
)
#########################################################################
#Step 4 : Add PROJECT define 
MESSAGE(STATUS "Step 4 : Add code source")

#########################################################################
#Step 5 : Add executable or library target
MESSAGE(STATUS "Step 5 : Add executable or library target")
ADD_executable(${BUILD_PROJECT_NAME} ${SRC_LIST})
set_property(TARGET ${BUILD_PROJECT_NAME} PROPERTY FOLDER ${PROJECT_SET_NAME_RUN_APP})

#########################################################################
#Step 6 : link with other library
MESSAGE(STATUS "Step 6 : link with other library")
IF(UNIX AND CMAKE_BUILD_TYPE MATCHES Release)
    add_custom_command(TARGET ${BUILD_PROJECT_NAME} POST_BUILD 
      #COMMAND ${CMAKE_STRIP} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BUILD_PROJECT_NAME}
      COMMAND ls -lh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BUILD_PROJECT_NAME}
      # COMMAND cp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BUILD_PROJECT_NAME} /mnt/nfs/  
    )
ENDIF()

SET(VZ_BASEEVENT_LIB

)

IF(WIN32)
  TARGET_LINK_LIBRARIES(${BUILD_PROJECT_NAME} 
    ${VZPROJECT_LINK_LIB}

    #libstp
  )
ELSEIF(CMAKE_BUILD_PLATFORM MATCHES "HIMIX200H1L")
  SET(HISI_LINK_LIBS

  )

  TARGET_LINK_LIBRARIES(${BUILD_PROJECT_NAME}
 
    ${VZ_BASEEVENT_LIB}
    ${HISI_LINK_LIBS}
    ${VZPROJECT_LINK_LIB}
  )
 
ELSEIF(CMAKE_BUILD_PLATFORM MATCHES "HIMIX200H1M")
  SET(HISI_LINK_LIBS

  )

  TARGET_LINK_LIBRARIES(${BUILD_PROJECT_NAME}
    -Wl,--start-group
    
    ${VZ_BASEEVENT_LIB}
    ${HISI_LINK_LIBS}
    -Wl,--end-group
    ${VZPROJECT_LINK_LIB}
  ) 
 
ENDIF()
