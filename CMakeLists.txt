cmake_minimum_required(VERSION 3.2.0)
project( master )

find_package( OpenCV REQUIRED )

# Include headers
include_directories(${WIRINGPI_INCLUDE_DIRS})

add_executable( master Master.cpp )
target_link_libraries( master ${OpenCV_LIBS} )

install(
	FILES master
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
	DESTINATION /home/pi/Documents/GP
)
