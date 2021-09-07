# i2c_pwm
This program helps to communicate with the PCA9685 PWM controller.

Communication is possible via the i2c bus and can be run on all Linux machines that have an i2c-dev driver in the kernel.

When I was working with a PWM controller, I ran into the problem that there was not a single project implemented for this controller in C ++, which motivated me to start porting the program from Python. 
The program has a stable release, has #NO depends from WiringPi or from other libraries and its development will continue. 
