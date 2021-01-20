#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

#include <stdlib.h>

static const int led_pin = LED_BUILTIN;

int *led_rate1;

static TaskHandle_t taskBlink = NULL;

void toggle_led_rate(void *parameter) {
  Serial.print("Blinking LED at ");
  Serial.print(*led_rate1 / portTICK_PERIOD_MS);
  Serial.println("ms");

  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(*led_rate1 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(*led_rate1 / portTICK_PERIOD_MS);
  }
}

void read_input(void *parameter) {
  char inputArray[5];

  while (true) {
    int index;
    bool eoi;
    int desired_led_rate;

    for (index = 0, eoi = false; Serial.available() > 0 && index < 4 && !eoi; ) {
      char c = Serial.read();

      switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          inputArray[index++] = c;
          break;

        case '\n':
          eoi = true;
          break;

        default:
          break;
      }
    }

    inputArray[index] = NULL;
    desired_led_rate = atoi(inputArray);
  
    if (desired_led_rate >= 100 && desired_led_rate <= 2000) {
      *led_rate1 = desired_led_rate;

      //Absolutely don't need to do this, but I just want to play with deleting and creating tasks from another task.
      if (taskBlink != NULL) {
        Serial.println("Removing existing task");
        vTaskDelete(taskBlink);
        taskBlink = NULL;
      }

      xTaskCreatePinnedToCore(
        toggle_led_rate
        , "Toggle LED Rate 1"
        , 1024
        , NULL
        , 1
        , &taskBlink
        , app_cpu
      );
    }
  }
}

void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  pinMode(led_pin, OUTPUT);

  led_rate1 = (int *) malloc(sizeof(int));

  Serial.println("Enter a LED rate in millisecond");

  xTaskCreatePinnedToCore(
    read_input
    , "Read Input"
    , 1024
    , NULL
    , 1
    , NULL
    , app_cpu
  );

  //Delete the setup() and loop() task.
  vTaskDelete(NULL);
}

void loop() {
}
