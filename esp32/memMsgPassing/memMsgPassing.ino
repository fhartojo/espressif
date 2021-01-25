#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

char *msgPtr = NULL;
volatile bool newMsg = false;

void readInputTask(void *argument) {
  int index;
  char inputArray[BUFSIZ];

  while(1) {
    for (index = 0; index < BUFSIZ - 1; ) {
      if (Serial.available() > 0) {
        char c = Serial.read();
  
        if (isprint(c)) {
          inputArray[index++] = c;
        } else if (c == '\n') {
          break;
        }
      }
    }

    if (index > 0) {
      inputArray[index++] = '\0';

      Serial.print("Typed input message:  ");
      Serial.println(inputArray);

      msgPtr = (char *) pvPortMalloc(index * sizeof(char));
      strcpy(msgPtr, inputArray);

      newMsg = true;
    }
  }
}

void writeOutputTask(void *argument) {
  while(1) {
    if (newMsg) {
      Serial.print("Received input message:  ");
      Serial.println(msgPtr);

      vPortFree(msgPtr);
      msgPtr = NULL;

      newMsg = false;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  xTaskCreatePinnedToCore(
    readInputTask
    , "Read Input"
    , 1024
    , NULL
    , 1
    , NULL
    , app_cpu
  );

  xTaskCreatePinnedToCore(
    writeOutputTask
    , "Write Output"
    , 1024
    , NULL
    , 1
    , NULL
    , app_cpu
  );

  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}
