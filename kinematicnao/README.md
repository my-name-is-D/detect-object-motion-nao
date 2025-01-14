## NAO Kinematic on ROS

# original work
- https://github.com/noobswestand/NaoPythonIK
(himself inspired from Kouretes github, see thesis :https://www.cs.umd.edu/~nkofinas/Projects/KofinasThesis.pdf for understanding NAO kinematic)

# How this package work
Nao kinematic has seen on the original work has been adapted to ROS and my case
 
 ### V1
 An object position and orientation can be written in a txt file and read here, 
 the kinematic find the appropriate angles to reach the point and send as a topic "jointangles" 
 the angles from the shoulder to the elbow to reach the end pose. 
```bash
roslaunch kinematicnao filekinematicnao.launch 
```
The txt file can be found in the folder data of this package, but in the code the file read is in the "world" 
package of this repository

### V2
Another version of the code has been realised. 

The object position is retrieved from a topic. 
The topic is created by the image_processing package (launch.launch)
then the rest is the same

```bash
roslaunch kinematicnao kinematicnao.launch 
```

### V3

The object position is retrieved from a file world/data/objectpoint.txt.
If the position is in Nao's left arm workspace then the inverse kinematic is done, else a "position not ok" (or something) is sent through the topic "jointangles"

```bash
roslaunch kinematicnao workspacefilenao.launch 
```
Note: the workspace is a simplified version of Nao's workspace


A python version of the workspace limitation is workspace_stimulation in this package or is present in image_processing/simulation_image_form_color.py, it send a binary stimulation if the point is in or out of the workspace (and if the color of the object is not stimulating).

The version retrieving the object pose from a topic is:

```bash
roslaunch kinematicnao mainworkspace.launch 
```
In this last version the trajectory received and the arm joints corresponding are saved in a txt file: joints.txt (saved in world/data/kinematic_joints/joints.txt)

### V4

The object position is retrieved from a topic, the stimulation and muscle error comes from another (brain model's receives 1 or 0 to indicate a stimulation and the noise associated to it (PD))
The endpoint is decomposed in several sub_motion to be able to send perturbation to the muscles (or not) according to the brain status (Pd/healthy)

```bash
roslaunch kinematicnao decomposed_kinematic_1hand.launch
```

In this version all the sub motions, received noises and the arm joints corresponding are saved in a txt file: joints_decompo.txt (saved in world/data/kinematic_joints/joints_decompo.txt)
The object is reached at an higher height in 16 submotions then in the 17th, the hand goes down on the object

Also, this launch also launchs the simulation_image_form_color.py from image_processing.


Another version tries to reach the object with two hands
```bash
roslaunch kinematicnao decomposed_kinematic_2hands.launch
```
not quite successful with the right arm.
## the python file

Is the workspace determination and the stimulation signal sent to the rat_model.
If ball is of the right color and in the workspace: stimulation, else not.
This python will probably be integrated in another python to diminuish the number of topics


