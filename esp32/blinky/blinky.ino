#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;

static const int led_rate1 = 500;
static const int led_rate2 = 300;

void toggle_led_rate(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(*((int *) parameter) / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(*((int *) parameter) / portTICK_PERIOD_MS);
  }
}

void setup() {
  pinMode(led_pin, OUTPUT);

  xTaskCreatePinnedToCore(
    toggle_led_rate
    , "Toggle LED Rate 1"
    , 1024
    , (void *) &led_rate1
    , 1
    , NULL
    , app_cpu
  );

  xTaskCreatePinnedToCore(
    toggle_led_rate
    , "Toggle LED Rate 2"
    , 1024
    , (void *) &led_rate2
    , 1
    , NULL
    , app_cpu
  );
}

void loop() {
  // put your main code here, to run repeatedly:

}
