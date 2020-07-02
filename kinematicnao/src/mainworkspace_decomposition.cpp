#include <iostream>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include "NAOKinematics.h"
#include "KMat.hpp"
#include <fstream>
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <geometry_msgs/PointStamped.h>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float64.h"
#include "KinematicsDefines.h"
#include "KinematicsDefines.h"
#include <rat_model/list_pop.h>
using namespace std;
using namespace KMath::KMat;
using namespace KDeviceLists;
string color("blue");
float lpx=218.65; //be wary, if you change here you also have to change something down, check it out
float lpy=113;
float lpz=65;
double execution= 0;
float error=0;

//MAIN TAKING OBJECT POSE FROM A TOPIC ("point" from image_processing launch.launch)
//taken from noobswestand github : NAaoPythonIK and modified for ROS/my situation.
//CAN BE ADAPTED FOR LEFT AND RIGHT ARM

void chatterCallback(const geometry_msgs::PointStamped& msg)
{

  ::lpx= (msg.point.x); //in mm
  ::lpy= msg.point.y; //in mm
  ::lpz= msg.point.z; //cheating here because z obtained with my caemra is not good here considering nao size
  ::color= msg.header.frame_id;
  cout <<"kinematic point received: "<< lpx << " , " << lpy << ", " << lpz<< ",color " << color << "\n";
  //ROS_INFO("I heard: [%s]", msg->data.c_str());
}

void stimuCallback(const std_msgs::Float64& msg)
{
//First stimulation reception, when no brain model. Used to test prototype 2, the motion according to a stimulation (and synchonise files)
 //TEST UNIT
/*
 if (msg.data==0.0012){
    ::execution=0;}
 else{
    ::execution=1;
    }*/

  //cout <<"kinematic stimulation received: "<<  execution << "\n";
  //ROS_INFO("I heard: [%s]", msg->data.c_str());
}
void stimulationcallback(const rat_model::list_pop &msg){
    //cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXIIIIIIIMMMMTHHHHEEEEEERRRREEEEEEEE CAAAN YOUUU SEEEEEEEE MMMMEEEEEEEE?";
    ::execution=msg.popaveragerate[0];
    ::error=msg.popaveragerate[1];
    cout<<"stimulation: "<< execution <<", error: "<<error;


}
float clamp(float x, float min, float max){
    if (x<min){x=min;}
    if (x>max){x=max;}
    return x;
}

void convert(float x,float y, float z, float *result){
    float ch=cos(x);
    float sh=sin(x);
    float ca=cos(y);
    float sa=sin(y);
    float cb=cos(z);
    float sb=sin(z);

    result[0] = ch * ca;
    result[1] = sh*sb - ch*sa*cb;
    result[2] = ch*sa*sb + sh*cb;
    result[3] = sa;
    result[4] = ca*cb;
    result[5] = -ca*sb;
    result[6] = -sh*ca;
    result[7] = sh*sa*cb + ch*sb;
    result[8] = -sh*sa*sb + ch*cb;
}



int main(int argc, char **argv){
    ros::init(argc, argv, "kinematic");
    ros::NodeHandle n;
    ros::Publisher chatter_pub = n.advertise<std_msgs::String>("jointangles", 1000);
    ros::Subscriber sub = n.subscribe("point", 1000, chatterCallback);
    ros::Subscriber sub2 = n.subscribe("/stimulation", 1000, stimuCallback);
    ros::Subscriber sub3 = n.subscribe("/brainoutstimu", 1000,stimulationcallback);
    ros::Rate loop_rate(10);

	NAOKinematics nkin;
	NAOKinematics::kmatTable output1, output2;
    string writedatapath="/home/cata/nao_ws/src/kinematicnao/data/joints_decompo.txt";
	std::vector<float> joints(NUMOFJOINTS);
    string prev("");
    string prev2("");
    float prevlpx=0;
    float prevlpy=0;
    float prevlpz=0;



    //float prevangle[4] ={0,0,0,0};
    //std::vector<float> prevangle(4,0);

    unsigned int starting_point=0; //just to send a motion at the beginning, else trouble
	//double pi = KMath::KMat::transformations::PI;

	//while (true){
    while (ros::ok())
     {
        std_msgs::String msg;
        //position (x,y,z)
        //float lpx,lpy,lpz; //rpx,rpy,rpz; //pos
        float lrx,lry,lrz;//,rrx,rry,rrz; //rot
        //float lrx,lry,lrz,rrx,rry,rrz; //rot
/*
        lpx=180;
        lpy=30;
        lpz=0.0;
        */
        lrx=0;//empirical observation
        lry=0;//lry;//lrz;
        lrz=0;//temp;

        /*
        //min/max left
        lpz=clamp(lpz,-.95,.95);//up down
        lpy=clamp(lpy,-.05,.85);//left right
        lpx=clamp(lpx,0,.67);//foward back
        */
        if (lpx!= 218.65 and lpy!=113 and lpz!=65 ){
            starting_point=1;
        }
        //to get a random float
        boost::random::mt19937 gen;
        boost::random::uniform_real_distribution<float> dist(-abs(error/3),abs(error/3+0.0001));//-0.1001, 0.1001);
        if (execution==1 or starting_point==0){
            if (not(prevlpx==lpx and prevlpy==lpy and prevlpz==lpz)or starting_point==0){
                stringstream fullstream;
		    
		unsigned int div_number=16    
		float lx=(lpx-prevlpx)/div_number;
                float ly=(lpy-prevlpy)/div_number;
                float lz=(lpz-prevlpz)/div_number;
                for (unsigned int div=1; div<=div_number; div++){
                    //+0.0001 because it can't take 0

                    if (starting_point==0){ //don't want to divide the first goal
                        div=div_number;
                    }
                    cout<<"angle sent";
                    
                    output1(0,3)=(prevlpx+lx*div);//+(std::rand() % 5) ;//350;  //forward/backward
                    output1(1,3)=prevlpy+ly*div;//+std::rand() % 5;//320; //side to side
                    output1(2,3)=prevlpz+lz*div;//+std::rand() % 5;//200+100; //up/down
                    if (div==div_number){
                        prevlpx=lpx;
                        prevlpy=lpy;
                        prevlpz=lpz;
                    }
                    //rotation
                    float mat[9];
                    convert(lrx,lry,lrz,mat);

                    output1(0,0)=mat[0];
                    output1(0,1)=mat[1];
                    output1(0,2)=mat[2];

                    output1(1,0)=mat[3];
                    output1(1,1)=mat[4];
                    output1(1,2)=mat[5];

                    output1(2,0)=mat[6];
                    output1(2,1)=mat[7];
                    output1(2,2)=mat[8];

                    vector<vector<float> > result;
                    /**
                    * This is a message object. You stuff it with data, and then publish it.
                    */


                    std::cout << "x = " << output1(0,3) << " y = " << output1(1,3) << " z = " << output1(2,3) <<  std::endl;
                    //output1.prettyPrint();

                    joints[L_ARM+SHOULDER_PITCH]=0;
                    joints[L_ARM+SHOULDER_ROLL]=0;
                    joints[L_ARM+ELBOW_YAW]=0;
                    joints[L_ARM+ELBOW_ROLL]=0;
                    nkin.setJoints(joints);
                    result = nkin.jacobianInverseLeftHand(output1);

                    if(!result.empty()){
                        ofstream read;
                        read.open(writedatapath, std::ios::app); //In this txt we write all the data of a stimulation
                        stringstream littlestream ;
                        littlestream<<"\n color:"<<color<<" x:"<<output1(0,3)<<" y:"<<output1(1,3)<<" z:"<<output1(2,3)<<" angles: ";
                        string s1 = littlestream.str();
                        if (prev.compare(s1)!=0){
                            read << s1 << ",";
                            prev=s1;
                        }//prev.compare

                        for(unsigned int j=0; j<result[0].size(); j++){
                            stringstream stream;
                            /*if (error>0){
                                float merror=dist(gen);
                            }*/
                            float merror=dist(gen);
                            cout<<"ERRRRROOOOOOOOOORRRRRRR :"<< merror<<"\n";
                            //cout << "angle" << j << " = " << result[0][j]/div << " ";
                            stream << fixed << setprecision(4) << result[0][j]+merror;
                            fullstream<<stream.str()<<",";

                            //if (div==1){ //if the ball pose is updated it will move from the prev pose to the next
                            //    prevangle[j]=result[0][j];
                            //}
                            //cout << s << " , ";
                        }//j
                        string s2 = fullstream.str();
                        if (prev2.compare(s2)!=0){
                            read << "  "<< div << " :  ";
                            read << s2 << ",";
                            prev2=s2;
                        }//prev compare
                        //read<<"\n";
                        read.close();
                    }//if not empty
                msg.data = fullstream.str();
                 }//for div
                chatter_pub.publish(msg);
                ros::Duration(0.05).sleep(); // sleep in second
                }//if not = prev
        }//end if execution

        /*
        result = nkin.inverseLeftHand(output1);
	    //cout<<"test2"<<endl;
	    if(!result.empty()){
		    cout << "--Solution exists 1" << endl;
		    for(unsigned int j=0; j<result[0].size(); j++){
			    cout << "angle" << j << " = " << result[0][j] << " ";
		    }
	    }
	    else{
	        cout<<"no solution"<<endl;
	    }
		cout << endl;
		*/

/*
        //right
        rrx=0;//rrx+3.1415;//lrx;
        rry=0;//rry;//lrz;
        rrz=0;//temp;


        //min/max right
        rpz=clamp(rpz,-.95,.95);//up down
        rpy=clamp(rpy,-.85,.05);//left right
        rpx=clamp(rpx,0,.67);//foward back


        output1(0,3)=rpx*304.8;  //forward/backward
        //output1(1,3)=rpy*304.8; //side to side
        output1(1,3)=rpy*360;//
        output1(2,3)=rpz*204.8+100; //up/down

        //rotation
        //float mat[9];

        convert(rrx,rry,rrz,mat);

        output1(0,0)=mat[0];
        output1(0,1)=mat[1];
        output1(0,2)=mat[2];

        output1(1,0)=mat[3];
        output1(1,1)=mat[4];
        output1(1,2)=mat[5];

        output1(2,0)=mat[6];
        output1(2,1)=mat[7];
        output1(2,2)=mat[8];

        //vector<vector<float> > result;
        std::cout << "x = " << output1(0,3) << " y = " << output1(1,3) << " z = " << output1(2,3) <<  std::endl;
        //output1.prettyPrint();
        joints[R_ARM+SHOULDER_PITCH]=0;
        joints[R_ARM+SHOULDER_ROLL]=0;
        joints[R_ARM+ELBOW_YAW]=0;
        joints[R_ARM+ELBOW_ROLL]=0;
        nkin.setJoints(joints);
        result = nkin.jacobianInverseRightHand(output1);
        if(!result.empty()){
            std::ofstream read;
            read.open("read.txt", std::ios_base::app);
            for(unsigned int j=0; j<result[0].size(); j++){
                //cout << "angle" << j << " = " << result[0][j] << " ";
                stringstream stream;
                stream << fixed << setprecision(4) << result[0][j];
                string s = stream.str();
                read << s << "\n";
                //cout << s << " , ";
            }
            read.close();
        }
        */

	//}

    else{
        stringstream fullstream;
        //cout <<"POSITION NOT REACHABLE";
        fullstream<<"POSITION NOT REACHABLE";
        msg.data = fullstream.str();
        chatter_pub.publish(msg);

    }
    //ROS_INFO("%s", msg.data.c_str());

    ros::spinOnce();

    loop_rate.sleep();

  }

	return 0;
}

