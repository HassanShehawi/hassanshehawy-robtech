cmake_minimum_required(VERSION 2.8.3)
project(using_markers)

set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  visualization_msgs
  std_msgs
  message_generation
)


 add_message_files(
   FILES
   MouseEvent.msg
 )


 generate_messages(
   DEPENDENCIES
   std_msgs
 )

catkin_package(

)


include_directories(include)



add_executable(basic_shapes src/basic_shapes.cpp)
target_link_libraries(basic_shapes ${catkin_LIBRARIES})

