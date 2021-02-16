#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

static const int led_pin = LED_BUILTIN;
static const int BUFFER_SIZE = 5;

SemaphoreHandle_t semHandle;

void blinkAtRateTask(void *argument) {
  int receivedLedRateMS = *((int *) argument);

  xSemaphoreGive(semHandle);

  Serial.print("Received LED rate:  ");
  Serial.println(receivedLedRateMS);

  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(receivedLedRateMS / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(receivedLedRateMS / portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  semHandle = xSemaphoreCreateMutex();
  pinMode(led_pin, OUTPUT);

  int index;
  char inputArray[BUFFER_SIZE];
  int ledRateMS = 500;

  Serial.println("Enter a blink rate (1--9999ms):");

  for (index = 0; index < BUFFER_SIZE - 1; ) {
    if (Serial.available() > 0) {
      char c = Serial.read();

      if (isdigit(c)) {
        inputArray[index++] = c;
      } else if (c == '\n') {
        break;
      }
    }
  }

  if (index > 0) {
    inputArray[index++] = '\0';
    ledRateMS = atoi(inputArray);

    Serial.print("Sending LED rate:  ");
    Serial.println(ledRateMS);
  }

  xSemaphoreTake(semHandle, portMAX_DELAY);

  xTaskCreatePinnedToCore(
    blinkAtRateTask
    , "Read Input"
    , 1024
    , (void *) &ledRateMS
    , 1
    , NULL
    , app_cpu
  );

  xSemaphoreTake(semHandle, portMAX_DELAY);

  Serial.println("Done!!!");
//  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}
