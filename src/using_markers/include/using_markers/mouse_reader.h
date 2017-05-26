#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <glob.h>
#include <sstream>

#include "ros/ros.h"

#include "using_markers/MouseEvent.h"

#ifndef USING_MARKERS_H
#define USING_MARKERS_H

class Mouse
{
public:
  // Constructor
  Mouse (std::string mouse_event_path)
  {
    if ( mouse_event_path.empty() )
    {
      descriptor_ = findMouse();
    }
    else
    {
      descriptor_ = openMouse( mouse_event_path.c_str() );
    }
  };
  
  int findMouse();

  int openMouse(const char *device_path);

  void closeMouse();

  bool isReadable();

  bool processEvent(struct input_event *ev, ros::Publisher& ros_publisher);

  bool spinMouse(ros::Publisher& ros_publisher);

private:
  int descriptor_;
  
  bool is_pressed_left_ = 0;
  bool is_pressed_right_ = 0;
  
  /** A list of substrings that would indicate that a device is a mouse. */
  std::vector<std::string> valid_substrings_ =
  {
    "Logitech",
    "Optical",
    "mouse",
//    "Touchpad",
//    "touchpad",
//    "touchPad",
//    "TouchPad"
  };
 
}; // end class 

#endif
