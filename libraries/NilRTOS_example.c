// source: http://forum.arduino.cc/index.php?topic=142494.15 fat16lib's post from 2013-07-17 16:35:00
// Connect a scope to pin 13.
// Measure difference in time between first pulse with no context switch
// and second pulse started in thread 2 and ended in thread 1.
// Difference should be about 10 usec on a 16 MHz 328 Arduino.
#include <NilRTOS.h>

const uint8_t LED_PIN = 13;

// Semaphore used to trigger a context switch.
Semaphore sem = {0};
//------------------------------------------------------------------------------
/*
* Thread 1 - high priority thread to set pin low.
*/
NIL_WORKING_AREA(waThread1, 128);
NIL_THREAD(Thread1, arg) {

  while (TRUE) {
    // wait for semaphore signal
    nilSemWait(&sem);
    // set pin low
    digitalWrite(LED_PIN, LOW);
  }
}
//------------------------------------------------------------------------------
/*
* Thread 2 - lower priority thread to toggle LED and trigger thread 1.
*/
NIL_WORKING_AREA(waThread2, 128);
NIL_THREAD(Thread2, arg) {

  pinMode(LED_PIN, OUTPUT);
  while (TRUE) {
    // first pulse to get time with no context switch
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
    // start second pulse
    digitalWrite(LED_PIN, HIGH);
    // trigger context switch for task that ends pulse
    nilSemSignal(&sem);
    // sleep until next tick (1024 microseconds tick on Arduino)
    nilThdSleep(1);
  }
}
//------------------------------------------------------------------------------
/*
* Threads static table, one entry per thread. Thread priority is determined
* by position in table.
*/
NIL_THREADS_TABLE_BEGIN()
NIL_THREADS_TABLE_ENTRY("thread1", Thread1, NULL, waThread1, sizeof(waThread1))
NIL_THREADS_TABLE_ENTRY("thread2", Thread2, NULL, waThread2, sizeof(waThread2))
NIL_THREADS_TABLE_END()
//------------------------------------------------------------------------------
void setup() {
  // Start nil.
  nilBegin();
}
//------------------------------------------------------------------------------
void loop() {
  // Not used.
}
