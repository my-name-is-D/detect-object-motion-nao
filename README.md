# object detection/motion-nao webots
first step. Detect and extract object position. 

## Image processing

Use find_2d_object, ROS

See objects, detect them, extract position in camera coordinates send the position and object ID as a topic (in cm). 
Either before or after a workspace filtering (if in send in topic, else it doesn't)

## kinematic nao

either receive the object pose as a topic or from a txt file (must be in mm, the reference base is the centre of NAO's torso)
transform it into angle joint and send them back as a topic.
A version consider Nao's workspace before calculating the angle joints.

## world

Subscribe to the topic publsihed by kinematic nao and move nao to the given position.
The joint pose are recorded at given interval and stored in csv files. 
They can then be ploted later on with 
,,,bash
world/data/./plotdata
,,,
