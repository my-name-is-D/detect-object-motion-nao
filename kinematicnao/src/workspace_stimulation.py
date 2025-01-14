#! /usr/bin/env python
from __future__ import division #to have 1/10=0.1 and not 0

import matplotlib.pyplot as plt
import numpy as np
import math
import rospy
from std_msgs.msg import Float64
from geometry_msgs.msg import PointStamped, Point


def callback(data):
    pub_stimulation = rospy.Publisher('/stimulation', Float64, queue_size=5)

    stimulation=0
    execution= True
    ShoulderOffsetZ=75.0
    ShoulderOffsetY=98.0
    ElbowOffsetY=15.0
    UpperArmLength=105.0
    LowerArmLength=57.7
    HandOffsetX=55.95
    HandOffsetZ=12.31
    Dist_arm=HandOffsetX+UpperArmLength+LowerArmLength #219
    OffsetLY= ShoulderOffsetY+ElbowOffsetY#113
    OffsetLZ=ShoulderOffsetZ+HandOffsetZ #112

    lpx=data.point.x
    lpy=data.point.y
    lpz=data.point.z

    color=data.header.frame_id
    print(color)
    print(lpx,lpy,lpz)


    #np.warnings.filterwarnings('ignore')#to ignore the "invalid value if arcsin not possible"
    second_Dist_arm= Dist_arm #+ OffsetLY
    third_Dist_arm=Dist_arm #+OffsetLZ

    #To consider the Offset of y (on y, the dist max is 331.7 while on x it's 218)
    #on z the dist max is 112+ 218 (from the origin, not from shoulder)
    if (color == "red" or color == "none" or color=="yellow" or lpy<0 or lpx>Dist_arm) :
        execution= False

    try:
        if not (not np.isnan(math.acos(lpx/Dist_arm)) and not np.isnan(math.asin((lpy-OffsetLY)/Dist_arm)) and not np.isnan(math.acos((lpz-OffsetLZ)/Dist_arm))
        and not np.isnan(math.asin(lpx/Dist_arm)) ) :
            execution= False
            #print("1")
        elif not( math.sqrt(lpx*lpx+(lpy-OffsetLY)*(lpy-OffsetLY))<= second_Dist_arm and 0<= math.acos(lpx/Dist_arm) and math.acos(lpx/Dist_arm)<=math.acos(0/Dist_arm)):
            execution= False
        elif not (math.asin((-67-OffsetLY)/Dist_arm)<= math.asin((lpy-OffsetLY)/Dist_arm) and math.asin((lpy-OffsetLY)/Dist_arm)<=math.asin((331-OffsetLY)/Dist_arm)):
            #0 correspond to max point when x=Dist_arm)
            #-67 is the min y pose and 331 is the max (with offset)
            execution= False
            #print("2")
        elif not(math.sqrt(lpx*lpx+(lpz-OffsetLZ)*(lpz-OffsetLZ))<= third_Dist_arm):
            execution= False
            #print("3")
        elif not( math.acos((305.9-OffsetLZ)/Dist_arm)<= math.acos((lpz-OffsetLZ)/Dist_arm) and
        math.acos((lpz-OffsetLZ)/Dist_arm)<=math.acos((-80-OffsetLZ)/Dist_arm) and math.asin(0/Dist_arm)<=math.asin(lpx/Dist_arm)):
            #We don't let x go behind its back, 306 and -80 are the max and min z with offset.
            execution= False
            #print("4")

    except ValueError: #if values are too high
        #print("test")
        execution= False

    if execution == False:
        print("Object not interesting")
        stimulation=0.0012
    else:
        print("Object interesting")
        stimulation=0.003
    pub_stimulation.publish(stimulation)





def readobjectpose():
    """
    When not using the camera nor the topic, retrieve the object pose from this file (filled by kinematic nao filemain.cpp)
    """
    obpose=[]
    #print("in")
    while (obpose == []):
        with open("/home/cata/nao_ws/src/world/data/objectpoint.txt","r") as file2:
            for line in file2:
                List = [elt.strip() for elt in line.split("\n")]
                obpose.append(List)
        #print(obpose)
    return obpose

"""
This function is not used anymore
"""
def main_func():

    pub_stimulation = rospy.Publisher('/stimulation', Float64, queue_size=5)

    stimulation=0
    execution= True
    ShoulderOffsetZ=75.0
    ShoulderOffsetY=98.0
    ElbowOffsetY=15.0
    UpperArmLength=105.0
    LowerArmLength=57.7
    HandOffsetX=55.95
    HandOffsetZ=12.31
    Dist_arm=HandOffsetX+UpperArmLength+LowerArmLength #219
    OffsetLY= ShoulderOffsetY+ElbowOffsetY#113
    OffsetLZ=ShoulderOffsetZ+HandOffsetZ #112

    
    obpose=readobjectpose()
    lpx=float(obpose[0][0])
    lpy=float(obpose[1][0])
    lpz=float(obpose[2][0])
    color= str(obpose[6][0])
    """
    ax=float(obpose[3][0])
    ay=float(obpose[4][0])
    az=float(obpose[5][0])
    """


    #print(color,lpx,lpy,lpz)


    #np.warnings.filterwarnings('ignore')#to ignore the "invalid value if arcsin not possible"
    second_Dist_arm= Dist_arm #+ OffsetLY
    third_Dist_arm=Dist_arm #+OffsetLZ

    #To consider the Offset of y (on y, the dist max is 331.7 while on x it's 218)
    #on z the dist max is 112+ 218 (from the origin, not from shoulder)
    if (color == "red" or color == "none" or color=="yellow" or lpy<0 or lpx>Dist_arm) :
        execution= False

    try:
        if not (not np.isnan(math.acos(lpx/Dist_arm)) and not np.isnan(math.asin((lpy-OffsetLY)/Dist_arm)) and not np.isnan(math.acos((lpz-OffsetLZ)/Dist_arm))
        and not np.isnan(math.asin(lpx/Dist_arm)) ) :
            execution= False
            #print("1")
        elif not( math.sqrt(lpx*lpx+(lpy-OffsetLY)*(lpy-OffsetLY))<= second_Dist_arm and 0<= math.acos(lpx/Dist_arm) and math.acos(lpx/Dist_arm)<=math.acos(0/Dist_arm)):
            execution= False
        elif not (math.asin((-67-OffsetLY)/Dist_arm)<= math.asin((lpy-OffsetLY)/Dist_arm) and math.asin((lpy-OffsetLY)/Dist_arm)<=math.asin((331-OffsetLY)/Dist_arm)):
            #0 correspond to max point when x=Dist_arm)
            #-67 is the min y pose and 331 is the max (with offset)
            execution= False
            #print("2")
        elif not(math.sqrt(lpx*lpx+(lpz-OffsetLZ)*(lpz-OffsetLZ))<= third_Dist_arm):
            execution= False
            #print("3")
        elif not( math.acos((305.9-OffsetLZ)/Dist_arm)<= math.acos((lpz-OffsetLZ)/Dist_arm) and
        math.acos((lpz-OffsetLZ)/Dist_arm)<=math.acos((-80-OffsetLZ)/Dist_arm) and math.asin(0/Dist_arm)<=math.asin(lpx/Dist_arm)):
            #We don't let x go behind its back, 306 and -80 are the max and min z with offset.
            execution= False
            #print("4")

    except ValueError: #if values are too high
        #print("test")
        execution= False

    if execution == False:
        #print("Object not interesting")
        stimulation=0.0012
    else:
        #print("Object interesting")
        stimulation=0.0035
    pub_stimulation.publish(stimulation)
def main():
    rospy.init_node("stimulation_decision")
    #Final code
    """
    rospy.Subscriber('/point',PointStamped, callback)
    rospy.spin()
    """
    #Test unit (without subscriber)
    
    while not rospy.is_shutdown():
        main_func()
    

if __name__ == '__main__':
    main()


