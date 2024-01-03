#include <stdlib.h>
#include <string.h>

#include "mruby.h"
#include "mruby/value.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

#include "driver/gpio.h"
#include "driver/uart.h"


//#define BUF_SIZE 1024
#define MAX_EXPECTED_DATA_SIZE 256
#define BUF_SIZE (MAX_EXPECTED_DATA_SIZE + 1)  // Add 1 for null-termination

typedef struct LoraData
{
    /*char Address;
    char DataLength;
    char Data;
    char SignalStrength;
    char SignalNoise;*/
    char Address[BUF_SIZE];
    char DataLength[BUF_SIZE];
    char Data[BUF_SIZE];
    char SignalStrength[BUF_SIZE];
    char SignalNoise[BUF_SIZE];
} Lora_Data_t;

Lora_Data_t Lora_data;

static mrb_value
mrb_esp32_lora_init(mrb_state *mrb, mrb_value self) {
  mrb_value uartnum,rx,tx,baudrate;
  mrb_get_args(mrb, "oooo",&uartnum,&rx,&tx, &baudrate);
  //mrb_fixnum(duty)
  printf("INITIAL :  %d %d %d %d\n",mrb_fixnum(uartnum),mrb_fixnum(rx),mrb_fixnum(tx),mrb_fixnum(baudrate));
  static const char *TAG = "LORA MASTER";
  const uart_config_t uart_config = {
        .baud_rate = mrb_fixnum(baudrate),
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
  uart_param_config(mrb_fixnum(uartnum), &uart_config);

  uart_set_pin(mrb_fixnum(uartnum),mrb_fixnum(rx),mrb_fixnum(tx), UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  //uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 5, &uart_queue, 0);
  uart_driver_install(mrb_fixnum(uartnum), BUF_SIZE , BUF_SIZE , 5, NULL, 0);
  //xTaskCreate(uart_task, "uart_task", TASK_MEMORY, NULL, 7, NULL);

  char *data = "AT\r\n";
  uart_write_bytes(mrb_fixnum(uartnum), (const char *)data, strlen(data));
  vTaskDelay(pdMS_TO_TICKS(500));

  data = "AT+ADDRESS=1\r\n";
  uart_write_bytes(mrb_fixnum(uartnum), (const char *)data, strlen(data));
  vTaskDelay(pdMS_TO_TICKS(500));

  data = "AT+NETWORKID=10\r\n";
  uart_write_bytes(mrb_fixnum(uartnum), (const char *)data, strlen(data));
  vTaskDelay(pdMS_TO_TICKS(500));

  data = "AT+CPIN=AABCF002EEDCFA90FABC0002EEDCAAF0\r\n";
  uart_write_bytes(mrb_fixnum(uartnum), (const char *)data, strlen(data));
  vTaskDelay(pdMS_TO_TICKS(500));

  data = "AT+BAND=920000000\r\n";
  uart_write_bytes(mrb_fixnum(uartnum), (const char *)data, strlen(data));
  vTaskDelay(pdMS_TO_TICKS(500));

  //ESP_LOGI(TAG, "init uart completed!");

  return mrb_nil_value();
}

static mrb_value
mrb_esp32_lora_set_node(mrb_state *mrb, mrb_value self) {
  mrb_int address,netid;
  mrb_value uartnum;
  mrb_get_args(mrb, "oii",&uartnum,&address,&netid);

  //data = "AT+ADDRESS=2\r\n"
  char command_data_add[300] = "";
  char command_add[] = "AT+ADDRESS=";
  //"AT+NETWORKID=10\r\n"
  char command_data_id[300] = "";
  char command_id[] = "AT+NETWORKID=";
  char end_of_line[] = "\r\n";
  
  snprintf(command_data_add,200,"%s%d%s",command_add,address,end_of_line);
  
  snprintf(command_data_id,200,"%s%d%s",command_id,netid,end_of_line);
  

  uart_write_bytes(mrb_fixnum(uartnum), (const char *)command_data_add, strlen(command_data_add));
  vTaskDelay(pdMS_TO_TICKS(500 * 2));
  uart_write_bytes(mrb_fixnum(uartnum), (const char *)command_data_id, strlen(command_data_id));
  vTaskDelay(pdMS_TO_TICKS(500 * 2));

  printf("Set Address : %s",command_data_add);
  printf("Set Network : %s",command_data_id);
  
  return mrb_nil_value();
}

static mrb_value
mrb_esp32_lora_set_band(mrb_state *mrb, mrb_value self) {
  mrb_int band;
  mrb_value uartnum;
  mrb_get_args(mrb, "oi",&uartnum,&band);
  //"AT+BAND=920000000\r\n"
  char command_data_band[300] = "";
  char command_band[] = "AT+BAND=";
  char end_of_line[] = "\r\n";
  
  snprintf(command_data_band,200,"%s%d%s",command_band,band,end_of_line);

  uart_write_bytes(mrb_fixnum(uartnum), (const char *)command_data_band, strlen(command_data_band));
  vTaskDelay(pdMS_TO_TICKS(500 * 2));

  printf("Set Band : %s",command_data_band);

  return mrb_nil_value();
}

static mrb_value
mrb_esp32_lora_set_cpin(mrb_state *mrb, mrb_value self) {
  char* cpin;
  mrb_value uartnum;
  mrb_get_args(mrb, "oz" ,&uartnum,&cpin);
  //"AT+CPIN=AABCF002EEDCFA90FABC0002EEDCAAF0\r\n";
  char command_data_cpin[300] = "";
  char command_cpin[] = "AT+CPIN=";
  char end_of_line[] = "\r\n";
  
  snprintf(command_data_cpin,200,"%s%s%s",command_cpin,cpin,end_of_line);

  uart_write_bytes(mrb_fixnum(uartnum), (const char *)command_data_cpin, strlen(command_data_cpin));
  vTaskDelay(pdMS_TO_TICKS(500 * 2));

  printf("Set CPIN: %s",command_data_cpin);
  
  return mrb_nil_value();
}

static mrb_value
mrb_esp32_lora_send(mrb_state *mrb, mrb_value self) {
  mrb_value uartnum,to_add;
  char* data;
  mrb_get_args(mrb, "oiz", &uartnum,&to_add,&data);
  
  char command_data[300] = "";
  char command[] = "AT+SEND=";
  char separator[] = ",";
  char end_of_line[] = "\r\n";
  
  snprintf(command_data, 200, "%s%d%s%d%s%s%s",command,mrb_fixnum(to_add),separator,strlen(data),separator,data,end_of_line);
  uart_write_bytes(mrb_fixnum(uartnum), (const char *)command_data, strlen(command_data));
  vTaskDelay(pdMS_TO_TICKS(500 * 2));

  printf("LoRa send : %s",command_data);

  return  mrb_nil_value();
}

static mrb_value
mrb_esp32_lora_set_parameter(mrb_state *mrb, mrb_value self) {
  mrb_value uartnum,cmode;
  char *mode = "";
  mrb_get_args(mrb, "oo" ,&uartnum,&cmode);

  if( mrb_fixnum(cmode) == 0 ){
    //within 3 km
    mode = "10,7,1,7";
  }
  else if( mrb_fixnum(cmode) == 1 ){
    //More than 3 km
    mode = "12,4,1,7";
  }  
  //AT+PARAMETER=10,7,1,7
  char command_data_cmode[300] = "";
  char command_cmode[] = "AT+PARAMETER=";
  char end_of_line[] = "\r\n";
  
  snprintf(command_data_cmode,200,"%s%s%s",command_cmode,mode,end_of_line);
  uart_write_bytes(mrb_fixnum(uartnum), (const char *)command_data_cmode, strlen(command_data_cmode));
  vTaskDelay(pdMS_TO_TICKS(500 * 2));

  printf("Set CMODE: %s",command_data_cmode);
  return mrb_nil_value();
}

static mrb_value
mrb_esp32_lora_read(mrb_state *mrb, mrb_value self) {
    mrb_value uartnum;
    mrb_get_args(mrb, "o", &uartnum);
    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);

    if (data != NULL) {
      int data_r = 0;
      char* data_s = "";
      //printf("Data S : %s \n",data_s);
   
      int rxBytes = uart_read_bytes(mrb_fixnum(uartnum), data,BUF_SIZE, 500/ portTICK_PERIOD_MS);
      if (rxBytes > 0) {
        data[rxBytes] = '\0';
          //printf("data read = %s \n",data);
          if (strncmp((const char *)data, "+RCV=", 5) == 0){
                  char *token = strtok((char *)data, "=");
                  // loop through the string to extract all other tokens
                  uint8_t count_token = 0;
                  while (token != NULL){
                      token = strtok(NULL, ",");
                      count_token++;
                      switch (count_token)
                      {
                      case 1:
                          //Lora_data.Address = token;
                          strncpy(Lora_data.Address, token, BUF_SIZE - 1);
                          Lora_data.Address[BUF_SIZE - 1] = '\0';  // Ensure null termination
                          break;
                      case 2:
                          //Lora_data.DataLength = token;
                          strncpy(Lora_data.DataLength, token, BUF_SIZE - 1);
                          Lora_data.DataLength[BUF_SIZE - 1] = '\0';  // Ensure null termination
                          break;
                      case 3:
                          //Lora_data.Data = token;
                          strncpy(Lora_data.Data, token, BUF_SIZE - 1);
                          Lora_data.Data[BUF_SIZE - 1] = '\0';  // Ensure null termination
                          break;
                      case 4:
                          //Lora_data.SignalStrength = token;
                          strncpy(Lora_data.SignalStrength, token, BUF_SIZE - 1);
                          Lora_data.SignalStrength[BUF_SIZE - 1] = '\0';  // Ensure null termination
                          break;
                      case 5:
                          //Lora_data.SignalNoise = token;
                          strncpy(Lora_data.SignalNoise, token, BUF_SIZE - 1);
                          Lora_data.SignalNoise[BUF_SIZE - 1] = '\0';  // Ensure null termination
                          break;

                      default:
                          break;
                      }
                  }
                  uint8_t r_value;
                  uint8_t g_value;
                  uint8_t b_value;

                  //r_value = atoi(strtok((char *)Lora_data.Data, "|")); //000|255|000
                  //g_value = atoi(strtok(NULL, "|"));
                  //b_value = atoi(strtok(NULL, "|"));
                  //data_r = atoi(strtok((char *)Lora_data.Data, "|"));

                  data_s = Lora_data.Data; //000|255|000
            }else{
              data_s = "Emtry data";
            }
            mrb_value mruby_str = mrb_str_new_cstr(mrb,data_s); 
      }else{
        data_s = "Emtry data";
        mrb_value mruby_str = mrb_str_new_cstr(mrb,data_s); 
      }
        //printf("Data S : %s \n",data_s);
        mrb_value mruby_str = mrb_str_new_cstr(mrb,data_s);
        free(data);
        return mruby_str;

      
    
  }else{
   mrb_raise(mrb, mrb->eStandardError_class, "Memory allocation failed");
   return mrb_nil_value();
  }
}





void
mrb_esp32_lora_gem_init(mrb_state* mrb)
{
  struct RClass *c;
  c = mrb_define_module(mrb, "LORA");

  // Comunication mode
  mrb_define_const(mrb, c, "MODE_1", mrb_fixnum_value(0));
  mrb_define_const(mrb, c, "MODE_2", mrb_fixnum_value(1));
  

  // UART_NUM 
  mrb_define_const(mrb, c, "NUM_0", mrb_fixnum_value(UART_NUM_0));
  mrb_define_const(mrb, c, "NUM_1", mrb_fixnum_value(UART_NUM_1));
  mrb_define_const(mrb, c, "NUM_2", mrb_fixnum_value(UART_NUM_2));
 


  // lora fucntion requirment
  mrb_define_module_function(mrb, c, "init_lora_896",mrb_esp32_lora_init, MRB_ARGS_REQ(4));
  mrb_define_module_function(mrb, c, "lora_set_node",mrb_esp32_lora_set_node, MRB_ARGS_REQ(3));
  
  // lora funtion other setup
  mrb_define_module_function(mrb, c, "lora_set_band",mrb_esp32_lora_set_band, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, c, "lora_set_cpin",mrb_esp32_lora_set_cpin, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, c, "lora_set_cmode",mrb_esp32_lora_set_parameter, MRB_ARGS_REQ(2));

  // lora comunication funtion setup
  mrb_define_module_function(mrb, c, "lora_send", mrb_esp32_lora_send, MRB_ARGS_REQ(3));
  mrb_define_module_function(mrb, c, "lora_read", mrb_esp32_lora_read, MRB_ARGS_REQ(1));
}

void
mrb_esp32_lora_gem_final(mrb_state* mrb)
{
}


