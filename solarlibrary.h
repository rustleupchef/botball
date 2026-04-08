#include <kipr/wombat.h>
#include <stdio.h>

struct Robot
{
    int leftWheel, rightWheel;
    int leftSpeed, rightSpeed;
    int armPort, clawPort;
    int servoSpeed;
    int grey;
    int buttonPort1, buttonPort2;
    int waitForLightSensor, blackLightSensor;
    int side;
    double startTime, timeout;
} prime;

void init()
{
    prime.leftWheel = 3;
    prime.rightWheel = 0;

    prime.leftSpeed = 1000;
    prime.rightSpeed = 1000;

    prime.armPort = 0;
    prime.clawPort = 3;

    prime.servoSpeed = 5;

    prime.grey = 3000;

    prime.buttonPort1 = 0;
    prime.buttonPort2 = 1;

    prime.waitForLightSensor = 0;
    prime.blackLightSensor = 1;
    prime.side = 1;

    prime.startTime = seconds();
    prime.timeout = 10;
}

void setup()
{
    wait_for_light(prime.waitForLightSensor);
    shut_down_in(118);
}

void logPrime()
{
    for (int i = 0; i < 20; i++)
    {
        printf("-");
    }
    printf("\n");

    printf("leftWheel=%d\n", prime.leftWheel);
    printf("rightWheel=%d\n", prime.rightWheel);

    printf("leftSpeed=%d\n", prime.leftSpeed);
    printf("rightSpeed=%d\n", prime.rightSpeed);

    printf("armPort=%d\n", prime.armPort);
    printf("clawPort=%d\n", prime.clawPort);

    printf("grey=%d\n", prime.grey);

    printf("buttonPort1=%d\n", prime.buttonPort1);
    printf("buttonPort2=%d\n", prime.buttonPort2);

    printf("waitForLightSensor=%d\n", prime.waitForLightSensor);
    printf("blackLightSensor=%d\n", prime.blackLightSensor);
    printf("side=%d\n", prime.side);

    printf("startTime=%lf\n", prime.startTime);

    for (int i = 0; i < 20; i++)
    {
        printf("-");
    }
    printf("\n");
}

int clamp(int var, int min, int max)
{
    if (var > max)
    {
        return max;
    }

    if (var < min)
    {
        return min;
    }
    return var;
}

void f(int distance)
{
    forward(distance);
}

void forward(int distance)
{
    v(prime.rightSpeed, prime.leftSpeed, 0, distance);
}

void b(int distance)
{
    backward(distance);
}

void backward(int distance)
{
    v(-prime.rightSpeed, -prime.leftSpeed, 0, distance);
}

void l(int degree)
{
    turnL(degree);
}

void turnL(int degree)
{
    v(prime.rightSpeed, -prime.leftSpeed, 0, degree * 12);
}

void r(int degree)
{
    turnR(degree);
}

void turnR(int degree)
{
    v(-prime.rightSpeed, prime.leftSpeed, 3, degree * 12);
}

void lP(int degree)
{
    turnLPivot(degree);
}

void turnLPivot(int degree)
{
    v(prime.rightSpeed, 0, 0, degree * 24);
}

void rP(int degree)
{
    turnRPivot(degree);
}

void turnRPivot(int degree)
{
    v(0, prime.leftSpeed, 3, degree * 24);
}

void v(int right, int left, int driver, int distance)
{
    driver = clamp(driver, 0, 3);
    right = clamp(right, -1500, 1500);
    left = clamp(left, -1500, 1500);

    cmpc(driver);
    int majority = (distance * 4) / 5;
    while (abs(gmpc(driver)) < majority)
    {
        mav(prime.rightWheel, right);
        mav(prime.leftWheel, left);
    }

    while (abs(gmpc(driver)) < distance)
    {
        mav(prime.rightWheel, right / 2);
        mav(prime.leftWheel, left / 2);
    }
    ao();
}

void p(double time)
{
    pause(time);
}

void pause(double time)
{
    double sTime = seconds();
    while (seconds() - sTime < time)
    {
        ao();
    }
}

void lc(char *name, int logitech)
{
    load_cam(name, logitech);
}

void load_cam(char *name, int logitech)
{
    logitech = clamp(logitech, 0, 1);
    camera_load_config(name);
    if (logitech)
    {
        camera_open_black();
    }
    else
    {
        camera_open();
    }

    for (int i = 0; i < 10; i++)
    {
        camera_update();
        p(0.01);
    }
}

int rotateTill(int channel, int size, int resolution, int direction)
{
    double startT = seconds();

    cmpc(prime.rightWheel);
    direction = clamp(direction, -1, 1);
    while (1)
    {
        if (seconds() - startT > prime.timeout)
        {
            break;
        }

        rectangle o = get_object_bbox(channel, 0);
        if (o.width * o.height > size)
        {
            int x = o.ulx + o.width / 2;
            if (abs((x - resolution / 2)) < resolution / 10)
            {
                return gmpc(prime.rightWheel);
            }
        }

        double sTime = seconds();
        while (seconds() - sTime <= 0.2)
        {
            mav(prime.rightWheel, -prime.rightSpeed / 2 * direction);
            mav(prime.leftWheel, prime.leftSpeed / 2 * direction);
        }
        p(0.01);
        camera_update();
    }
}

void ssc(int position)
{
    slowServoClaw(position);
}

void slowServoClaw(int position)
{
    slowServo(prime.clawPort, position);
}

void ssa(int position)
{
    slowServoArm(position);
}

void slowServoArm(int position)
{
    slowServo(prime.armPort, position);
}

void ss(int port, int position)
{
    slowServo(port, position);
}

void slowServo(int port, int position)
{
    if (get_servo_position(port) == position)
    {
        return;
    }

    int starting = get_servo_position(port);
    int counter = 0;

    while ((starting + counter) / prime.servoSpeed != position / prime.servoSpeed)
    {
        if (position - starting > 0)
        {
            counter += prime.servoSpeed;
        }
        if (position - starting < 0)
        {
            counter -= prime.servoSpeed;
        }

        set_servo_position(port, starting + counter);
        msleep(10);
    }
    set_servo_position(port, position);
    msleep(100);
}

int detect_color(int size)
{
    for (int i = 0; i < get_channel_count(); i++)
    {
        for (int j = 0; j < get_object_count(i); j++)
        {
            rectangle o = get_object_bbox(i, j);
            if (o.width * o.height > size)
            {
                return i;
            }
        }
    }
    return -1;
}

void lineup()
{
    double startT = seconds();
    while (digital(prime.buttonPort1) == 0 || digital(prime.buttonPort2) == 0)
    {
        if (seconds() - startT > prime.timeout)
        {
            break;
        }
        mav(prime.rightWheel, digital(prime.buttonPort1) == 0 ? -prime.leftSpeed / 2 : 0);
        mav(prime.leftWheel, digital(prime.buttonPort2) == 0 ? -prime.rightSpeed / 2 : 0);
    }
}

void rrtc(int greater)
{
    rotateRightTillColor(greater);
}

void rotateRightTillColor(int greater)
{
    rotateTillColor(greater, 0);
}

void rltc(int greater)
{
    rotateLeftTillColor(greater);
}

void rotateLeftTillColor(int greater)
{
    rotateTillColor(greater, 1);
}

void rtc(int greater, int dir)
{
    rotateTillColor(greater, dir);
}

void rotateTillColor(int greater, int dir)
{
    int right = dir ? prime.rightSpeed / 2 : -prime.rightSpeed / 2;
    int left = dir ? -prime.leftSpeed / 2 : prime.leftSpeed / 2;
    mtc(right, left, greater);
}

void ftc(int greater)
{
    forwardTillColor(greater);
}

void forwardTillColor(int greater)
{
    mtc(prime.rightSpeed, prime.leftSpeed, greater);
}

void btc(int greater)
{
    backwardTillColor(greater);
}

void backwardTillColor(int greater)
{
    mtc(-prime.rightSpeed, -prime.leftSpeed, greater);
}

void mtc(int right, int left, int greater)
{
    moveTillColor(right, left, greater);
}

void moveTillColor(int right, int left, int greater)
{
    double startT = seconds();
    while (1)
    {
        if (seconds() - startT > prime.timeout)
        {
            break;
        }

        if (greater && analog(prime.blackLightSensor) > prime.grey)
        {
            break;
        }
        else if (!greater && analog(prime.blackLightSensor) < prime.grey)
        {
            break;
        }
        mav(prime.rightWheel, right);
        mav(prime.leftWheel, left);
    }
    ao();
}

void ftbl(int distance)
{
    followTheBlackLine(distance);
}

void followTheBlackLine(int distance)
{
    double startT = seconds();

    cmpc(prime.rightWheel);
    while (abs(gmpc(prime.rightWheel)) < distance)
    {
        if (prime.side)
        {
            mav(prime.rightWheel, analog(prime.blackLightSensor) > prime.grey ? prime.rightSpeed : prime.leftSpeed / 2);
            mav(prime.leftWheel, analog(prime.blackLightSensor) > prime.grey ? prime.leftSpeed / 2 : prime.rightSpeed);
        }
        else
        {
            mav(prime.rightWheel, analog(prime.blackLightSensor) > prime.grey ? prime.leftSpeed / 2 : prime.rightSpeed);
            mav(prime.leftWheel, analog(prime.blackLightSensor) > prime.grey ? prime.rightSpeed : prime.leftSpeed / 2);
        }
    }
    ao();
}