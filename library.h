#include <kipr/wombat.h>
#include <stdio.h>

int clamp(int var, int min, int max) {
    if (var > max) {
        return max;
    }
    
    if (var < min) {
        return min;
    }
    return var;
}


void f(int distance) {
        forward(distance);
}


void forward(int distance) {
        v(1000, 1000, 0, distance);
}


void b(int distance) {
        backward(distance);
}


void backward(int distance) {
        v(-1000, -1000, 0, distance);
}


void l(int degree) {
        turnL(degree);
}


void turnL(int degree) {
        v(1000, -1000, 0, degree * 12);
}


void r(int degree) {
        turnR(degree);
}


void turnR(int degree) {
        v(-1000, 1000, 3, degree * 12);
}


void lP(int degree) {
	turnLPivot(degree);
}

void turnLPivot(int degree) {
	v(1000, 0, 0, degree * 24);
}

void rP(int degree) {
	turnRPivot(degree);
}

void turnRPivot(int degree) {
	v(0, 1000, 3, degree * 24);
}

void v(int right, int left, int driver, int distance) {
    driver = clamp(driver, 0, 3);
    right = clamp(right, -1000, 1000);
    left = clamp(left, -1000, 1000);
    
        cmpc(driver);
    int majority = (distance * 4) / 5;
    while (abs(gmpc(driver)) < majority) {
        mav(0, right);
        mav(3, left);
    }
    
    while (abs(gmpc(driver)) < distance) {
        mav(0, right/2);
        mav(3, left/2);
    }
    ao();
}

void p(double time) {
	pause(time);
}

void pause(double time) {
	double sTime = seconds();
    while (seconds() - sTime < time) {
    	ao();
    }
}

void lc(char* name, int logitech) {
	load_cam(name, logitech);
}

void load_cam(char* name, int logitech) {
    logitech = clamp(logitech, 0, 1);
	camera_load_config(name);
    if (logitech) {
    	camera_open_black();
    } else {
    	camera_open();
    }
    
    for (int i = 0; i < 10; i++) {
    	camera_update();
        p(0.01);
    }
}

int rotate_till(int channel, int size, int resolution, int direction) {
    cmpc(0);
    direction = clamp(direction, -1, 1);
	while (1) {
        rectangle o = get_object_bbox(channel, 0);
        if (o.width * o.height > size) {
            int x = o.ulx + o.width/2;
            if (abs((x - resolution/2)) < resolution/10){
                return gmpc(0);
            }
        }
        
        double sTime = seconds();
        while (seconds() - sTime <= 0.2) {
        	mav(0, -500 * direction);
            mav(3, 500 * direction);
        }
        p(0.01);
        camera_update();
    }
}

void ss(int port, int position, int speed) {
	slow_servo(port, position, speed);
}

void slow_servo(int port, int position, int speed) {
	if (get_servo_position(port)  == position) {
    	return;
    }
    
    int starting = get_servo_position(port);
    int counter = 0;
    
    while((starting + counter)/speed != position/speed) {
    	if (position - starting > 0) {
    		counter  += speed;
    	}
        if (position - starting < 0) {
        	counter -= speed;
        }
        
        set_servo_position(port, starting + counter);
        msleep(10);
    }
    set_servo_position(port, position);
    msleep(100);
}

int detect_color(int size) {
	for (int i = 0; i < get_channel_count(); i++) {
    	for (int j = 0; j < get_object_count(i); j++) {
        	rectangle o = get_object_bbox(i, j);
            if (o.width * o.height > size) {
            	return i;
            }
        }
    }
    return -1;
}

void lineup(int port1, int port2) {
    double startT = seconds();
	while (digital(port1) == 0 || digital(port2) == 0 ) {
        if (seconds() - startT > 10) {
        	break;
        }
    	mav(0, -500);
        mav(3, -500);
    }
}

void rtc(int port, int thresh, int greater, int dir) {
	rotateTillColor(port, thresh, greater, dir);
}

void rotateTillColor(int port, int thresh, int greater, int dir) {
	greater = clamp(greater, 0, 1);
    dir = clamp(dir, 0, 1);
    
    while (1) {
    	if (greater && analog(port) > thresh) {
            break;
        } else if (!greater && analog(port) < thresh) {
        	break;
        }
        mav(0, dir ? 500 : -500);
        mav(3, dir ? -500 : 500);
    }
}

void ftc(int port, int thresh, int greater) {
	forwardTillColor(port, thresh, greater);
}

void forwardTillColor(int port, int thresh, int greater) {
	while (1) {
    	if (greater && analog(port) > thresh) {
            break;
        } else if (!greater && analog(port) < thresh) {
        	break;
        }
        mav(0, 1000);
        mav(3, 1000);
    } 
}

void btc(int port, int thresh, int greater) {
	backwardTillColor(port, thresh, greater);
}

void backwardTillColor(int port, int thresh, int greater) {
	while (1) {
    	if (greater && analog(port) > thresh) {
            break;
        } else if (!greater && analog(port) < thresh) {
        	break;
        }
        mav(0, -1000);
        mav(3, -1000);
    }
}

void ftbl(int port, int thresh, int distance) {
	followTheBlackLine(port, thresh, distance);
}

void followTheBlackLine(int port, int thresh, int distance) {
	cmpc(0);
    while (abs(gmpc(0)) < distance) {
    	mav(0, analog(port) > thresh ? 1000 : 500);
        mav(3, analog(port) > thresh ? 500 : 1000);
    }
    ao();
}