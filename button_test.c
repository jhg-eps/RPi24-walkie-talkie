#include <stdio.h>
#include <wiringPi.h>

// Compilation Instructions
//gcc -Wall -o button button_test.c -lwiringPi

// Run Instructions
//sudo ./button

#define GPIO_4_AKA_BUTTON1   7
#define PULL_UP 2
#define PULL_DOWN 1
#define RESISTOR_GONE 0

int main(void)
{
	int pin_state = 0;
	// Initialize wiringPi
	wiringPiSetup();
	
	// Initialize the testing button.
	pinMode(GPIO_4_AKA_BUTTON1, INPUT);
	pullUpDnControl(GPIO_4_AKA_BUTTON1, PULL_UP);

	// whi;e loop to poll the button.
	while(1)
	{
		pin_state = digitalRead(GPIO_4_AKA_BUTTON1);
		if (pin_state == 0)
			printf("Button 1 is pressed!!\n");

		// put a 250ms delay to give the processor some breathing room
		delay (250);
	}

	return 0;
}

