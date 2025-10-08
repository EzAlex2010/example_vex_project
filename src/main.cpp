#include "main.h"
#include "robot.h"
#include "lemlib/api.hpp" // IWYU pragma: keep

// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// motor groups
pros::MotorGroup leftMotors({-5, 4, -3},pros::MotorGearset::blue);
pros::MotorGroup rightMotors({6, -9, 7}, pros::MotorGearset::blue);
// Inertial Sensor on port 10
pros::Imu imu(10);

// tracking wheels
// horizontal tracking wheel encoder. Rotation sensor, port 20, not reversed
pros::Rotation horizontalEnc(20);
// vertical tracking wheel encoder. Rotation sensor, port 11, reversed
pros::Rotation verticalEnc(-11);
// horizontal tracking wheel. 2.75" diameter, 5.75" offset, back of the robot (negative)
lemlib::TrackingWheel horizontal(&horizontalEnc, lemlib::Omniwheel::NEW_275, -5.75);
// vertical tracking wheel. 2.75" diameter, 2.5" offset, left of the robot (negative)
lemlib::TrackingWheel vertical(&verticalEnc, lemlib::Omniwheel::NEW_275, -2.5);

// drivetrain settings
lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              10, // 10 inch track width
                              lemlib::Omniwheel::NEW_4, // using new 4" omnis
                              360, // drivetrain rpm is 360
                              2 // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// lateral motion controller
lemlib::ControllerSettings linearController(0.5, // proportional gain (kP)
                                            0, // integral gain (kI)
                                            0.1, // derivative gain (kD)
                                            0, // anti windup
                                            1, // small error range, in inches
                                            0, // small error range timeout, in milliseconds
                                            0, // large error range, in inches
                                            0, // large error range timeout, in milliseconds
                                            0 // maximum acceleration (slew)
);

// angular motion controller
lemlib::ControllerSettings angularController(0.5, // proportional gain (kP)
                                             0, // integral gain (kI)
                                             0.1, // derivative gain (kD)
                                             0, // anti windup
                                             1, // small error range, in degrees
                                             0, // small error range timeout, in milliseconds
                                             0, // large error range, in degrees
                                             0, // large error range timeout, in milliseconds
                                             0 // maximum acceleration (slew)
);

// sensors for odometry (set to nullptr if not present)
lemlib::OdomSensors sensors(&vertical, // vertical tracking wheel
                            nullptr, // vertical tracking wheel 2, set to nullptr as we don't have a second one
                            &horizontal, // horizontal tracking wheel
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu // inertial sensor
);

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttleCurve(3, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steerCurve(3, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);

// create the chassis
lemlib::Chassis chassis(drivetrain, linearController, angularController, sensors, &throttleCurve, &steerCurve);

//Add motors and sensors here
pros::Motor intake1(21, pros::MotorGearset::blue);

void screenTask() {
    while (true) {
        // print robot location to the brain screen
        pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
        pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
        pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
        // log position telemetry
        lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
        // delay to save resources
        pros::delay(50);
    }
}

void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    pros::Task screenTask();
    chassis.calibrate(); // calibrate sensors
}

int auton = 0; //what auton you want to run

void autonomous() {
	chassis.setPose(0,0,0);
	
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

void opcontrol() {
    while (true) {
        // get joystick positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
        // move the chassis with curvature drive
        chassis.tank(leftY, rightY);

		// example intake control
		if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
			intake1.move_velocity(200);
		} else if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
			intake1.move_velocity(-200);
		} else {
			intake1.move_velocity(0);
		}
		
        // delay to save resources
        pros::delay(10);
    }
}
