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
#define DEBOUNCE_TIME 300
#define LOGIC_LOW 0
#define LOGIC_HIGH 1

void queue_access_callback(void)
{
	// do software debouncing, we are waiting for 
	unsigned long start = millis();
	unsigned long end = millis() + (unsigned long)DEBOUNCE_TIME;
	unsigned long current = 0;
	while (current <= end)
	{
		current = millis();
	}

	// Query the pin again
 	if (digitalRead(GPIO_4_AKA_BUTTON1) == LOGIC_LOW)
	{
		printf("We are accessing the queue now.\n");
	} else {
		printf("Debounce test failed.\n");
	}
}

int main(void)
{
	int pin_state = 0;
	// Initialize wiringPi
	wiringPiSetup();

	// Initialize the testing button.
	pinMode(GPIO_4_AKA_BUTTON1, INPUT);
	pullUpDnControl(GPIO_4_AKA_BUTTON1, PULL_UP);

	// Initialize the interrupt for the pin.
	wiringPiISR(GPIO_4_AKA_BUTTON1, INT_EDGE_FALLING, &queue_access_callback);


	// whi;e loop to poll the button.
	while(1)
	{
//		pin_state = digitalRead(GPIO_4_AKA_BUTTON1);
//		if (pin_state == 0)
//			printf("Button 1 is pressed!!\n");

		// put a 250ms delay to give the processor some breathing room
		delay (250);
	}

	return 0;
}

