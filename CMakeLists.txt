project(mps-qt5)

cmake_minimum_required(VERSION 2.8.9)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)

add_executable(
mps-qt5 
src/utils/thread/Thread.cpp 
main.cpp
src/system/SystemManager.cpp 
src/system/MPSSystem.cpp 
src/dicom/net/AssociationHandler.cpp 
src/settings/aet/AetTableManager.cpp  
src/utils/thread/ThreadManager.cpp 
src/utils/thread/ThreadCEchoSCP.cpp
src/dicom/net/DcmNetSCP.cpp 
src/utils/callback/progress/CFindeRSPProgress.cpp 
src/utils/callback/progress/Progress.cpp 
src/utils/callback/Callback.cpp 
src/dicom/net/DcmQuery.cpp
src/utils/exception/InvalidIntegerNumberExcep.cpp
src/utils/exception/Exception.cpp
src/utils/filesystem/FilesManager.cpp
src/dicom/UID.cpp
src/dicom/net/DcmAET.cpp
src/dicom/net/DcmNetSCU.cpp
src/utils/Singleton.cpp
src/utils/Status.cpp
)

target_link_libraries(  mps-qt5
                        /usr/lib/dcmtk/libofstd.so
                        /usr/lib/dcmtk/libdcmnet.a
                        /usr/lib/dcmtk/libdcmdata.a
                        /usr/lib/dcmtk/liboflog.a
                        libpthread.so
                        libz.so
                     )
# Use the Widgets module from Qt 5.
qt5_use_modules(mps-qt5 Widgets Core)

include_directories(./src ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS})

add_subdirectory(src)

install(TARGETS mps-qt5 RUNTIME DESTINATION bin)
