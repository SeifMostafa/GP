cmake_minimum_required(VERSION 3.2.0)
project( facedetect )

find_package( OpenCV REQUIRED )
find_package(Threads REQUIRED) 

# Include headers

add_executable( facedetect facedetect.cpp )
target_link_libraries( facedetect ${OpenCV_LIBS} )
target_link_libraries(facedetect ${CMAKE_THREAD_LIBS_INIT}) 

install(
	FILES facedetect
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
	DESTINATION /home/pi/Documents/GP
)
