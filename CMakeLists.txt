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
src/db/DBQueryRsp.cpp 
src/settings/MPSSystemSettings.cpp 
src/db/DBDcmTagTable.cpp 
src/db/DBManager.cpp 
src/tasks/TaskDcmSCP.cpp 
src/utils/exception/WaitingMessageErrorExcep.cpp 
src/dicom/net/DIMSEMessajeFactory.cpp 
src/tasks/DcmTask.cpp 
src/utils/exception/CalledAETErrorExcep.cpp 
src/tasks/TaskQRMoveSCP.cpp 
src/tasks/TaskQRFindSCP.cpp 
src/tasks/TaskHandleRqDCMConn.cpp 
src/tasks/TaskStoreSCP.cpp 
src/tasks/TaskFactory.cpp 
src/tasks/TaskQRMoveSCU.cpp 
src/tasks/TaskQRFindSCU.cpp 
src/tasks/TaskEchoSCP.cpp 
src/tasks/TaskStoreSCU.cpp 
src/tasks/TaskEchoSCU.cpp 
src/tasks/TaskManager.cpp 
src/tasks/Task.cpp  
main.cpp
src/system/SystemManager.cpp 
src/system/MPSSystem.cpp 
src/settings/aet/AetTableManager.cpp  
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
                        /home/freddy/.local/lib/libofstd.so
                        /home/freddy/.local/lib/libdcmnet.so
                        /home/freddy/.local/lib/libdcmdata.so
                        /home/freddy/.local/lib/liboflog.so
                        /home/freddy/.local/lib/libcharls.so
                        /home/freddy/.local/lib/libdcmdsig.so
                        /home/freddy/.local/lib/libdcmimage.so
                        /home/freddy/.local/lib/libdcmimgle.so
                        /home/freddy/.local/lib/libdcmjpeg.so
                        /home/freddy/.local/lib/libdcmjpls.so
                        /home/freddy/.local/lib/libdcmpstat.so
                        /home/freddy/.local/lib/libdcmqrdb.so
                        /home/freddy/.local/lib/libdcmrt.so
                        /home/freddy/.local/lib/libdcmsr.so
                        /home/freddy/.local/lib/libdcmtls.so
                        /home/freddy/.local/lib/libdcmwlm.so
                        
                        # BOOS LIBRARIES
                        /usr/lib/libboost_system.so
			/usr/lib/libboost_thread.so
			/usr/lib/libboost_filesystem.so
			
			/usr/lib/libmongoclient.a
			/usr/lib/libpthread.so
                        
                        libpthread.so
                        libz.so
                     )
# Use the Widgets module from Qt 5.
qt5_use_modules(mps-qt5 Widgets Core)

include_directories(
./src 
~/.local/include/
${Qt5Widgets_INCLUDE_DIRS}
${Qt5Core_INCLUDE_DIRS}
)
set(CMAKE_CXX_FLAGS "-std=c++0x")
add_subdirectory(src)

install(TARGETS mps-qt5 RUNTIME DESTINATION bin)
