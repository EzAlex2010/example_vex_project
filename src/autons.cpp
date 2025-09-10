#include "robot.h"

int auton = 0; //what auton you want to run

void run_auton()
{
    switch(auton)
    {
        case 0:
            auton_example();
            break;
        case 1:
            //another auton function
            break;

        //add more cases for more autons
    }
}

//Make more of these functions for each auton
void auton_example()
{
    chasis.setPose(0, 0, 0); // set starting position (x, y, theta)

    pros::delay(1000); // wait example
    
    chasis.moveToPoint(24, 0, 90); // move to point (x, y, theta)
}