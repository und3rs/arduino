#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
#include <SoftwareSerial.h>

/*
  # USAGE
  1. Connect PIN_RX to TX of remote device.
  2. Connect PIN_TX to RX of remote device.
  3. Connect PIN_NEXT_BAUDRATE with GND by physical button.
*/

#define PIN_NEXT_BAUDRATE 2
#define PIN_RX 3
#define PIN_TX 4
#define BAUDRATE_OF_LOCAL 115200

unsigned long boudarates[] = {2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 76800, 115200, 230400, 250000};
int boudrateIndex = 0;
int countOfBoudrates = sizeof(boudarates) / sizeof(boudarates[0]);


// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only one Task is accessing this resource at any time.
SemaphoreHandle_t xSerialSemaphore;
SoftwareSerial remoteSerial(PIN_RX, PIN_TX); // RX, TX

// define two Tasks for SerialRead & SerialWrite
void TaskSerialRead( void *pvParameters );
void TaskSerialWrite( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(BAUDRATE_OF_LOCAL);

  pinMode(PIN_NEXT_BAUDRATE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_NEXT_BAUDRATE), changeBaudrateToNext, LOW);
  
  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  // }
  
  // Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
  // because it is sharing a resource, such as the Serial port.
  // Semaphores should only be used whilst the scheduler is running, but we can set it up here.
  if ( xSerialSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }


  changeBaudrate(boudarates[boudrateIndex]);

  // Now set up two Tasks to run independently.
  xTaskCreate(
    TaskSerialRead
    ,  "SerialRead"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL //Parameters for the task
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL ); //Task Handle

  xTaskCreate(
    TaskSerialWrite
    ,  "SerialWrite" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle

  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}



void changeBaudrateToNext() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 100) {
    boudrateIndex = (boudrateIndex + 1) % countOfBoudrates;
    changeBaudrate(boudarates[boudrateIndex]);
    last_interrupt_time = interrupt_time;
  }
}



void changeBaudrate(unsigned long rate) {
  if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
    remoteSerial.begin(rate);
    Serial.print("+[SYSTEM] Boudrate is set: ");
    Serial.println(rate);

    xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
  }
}



void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/


void TaskSerialRead( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  /*
    TaskSerialRead
  */

  Serial.write("Start Read");
  for (;;) // A Task shall never return or exit.
  {
    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      while (remoteSerial.available()) {
        Serial.write(remoteSerial.read());
      }

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}



void TaskSerialWrite( void *pvParameters __attribute__((unused)) )  // This is a Task.
{

  for (;;)
  {
    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      while (Serial.available()) {
        remoteSerial.write(Serial.read());
      }

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}
