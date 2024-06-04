module LORA
  puts "LoRa Version....03 GIT"

  include ESP32::STANDARD
  include ESP32WIFI
  include ESP32MQTT
  #include ESP32UART

  class LoRa
    def initialize uartnum,rx,tx,baudrate
      # o = LoRa.new NUM_2,25,26,115200
      @uartnum = uartnum
      init_lora_896 uartnum,rx,tx,baudrate
      #data_r = lora_read @uartnum
      #puts data_r
      puts "init_lora_896 completed!"
    end

    def set_node address,netid
      #o.set_node 3,10
      lora_set_node @uartnum,address,netid
    end

    def set_band band
      lora_set_band @uartnum,band
    end

    def set_CPIN cpin
      lora_set_cpin @uartnum,cpin
    end

    def send to_address,data
      # o.send 2,"100|000|000"
      # o.send 2,"255|000|000"
      lora_send @uartnum,to_address,data
    end

    def read 
      lora_read @uartnum
      data = lora_read @uartnum
      return data
    end

    def set_cmode cmode
      #o.set_cmode MODE_1
      lora_set_cmode @uartnum,cmode
    end

    def set_param param
      lora_set_param @uartnum,param
    end

    def read_d
      #for i in 0..2
      puts "#######################"
      puts "Start test System"
      puts "#######################"
      sleep 5
      
      lora_send @uartnum,1,"ON"
      sleep 2
      lora_send @uartnum,2,"ON"
      sleep 1
      for i in 0..100
        data_r = lora_read @uartnum
        puts data_r
        sleep 1
        if data_r == "TEST 9"
          lora_send @uartnum,1,"OFF"
          sleep 2
          lora_send @uartnum,2,"OFF"
          sleep 1
          break
  
        elsif data_r == "TEST 5"
          lora_send @uartnum,2,"OFF"
          sleep 2
          lora_send @uartnum,2,"ON"
          sleep 1

        end
      end
      #end

    end

    def init_system_wifi
      w = WiFi.new
      w.wifi_connect "n302mesh","n302pw8879"
      sleep 3
    end

    def init_system_mqtt
      m = MQTT.new "192.168.1.14",1883
      m.mqtt_connect
      sleep 1
    end

    def observe_mode pin
      puts "test observe_mode_wifi_mqtt"
      sleep 1
      #ESP32WIFI::init_wifi
      #ESP32WIFI::connect "TP-Link_6EA8_5G","22429735"

      #w = WiFi.new
      #w.wifi_connect "TP-Link_6EA8_5G","22429735"
      #sleep 1
      m = MQTT.new "192.168.0.210",1883
      m.mqtt_connect
      sleep 1



      x = GPIO.high_at? pin
      while x == false
        x = GPIO.high_at? pin
        data_r = lora_read @uartnum
        #data_r = data_r.to_s
        if data_r != "Emtry data"
          begin
            puts "LoRa read : #{data_r}"
            Sun,Temp,Sl,Sr = data_r.split("|",-1) 
            
            _,sun_num = Sun.split(":",-1) 
            sun_p = ((sun_num.to_f / 3.2) * 100).round(2)
            puts "light intensity : #{sun_p} %"
            #puts sun_p

            _,temp_num = Temp.split(':', -1) 
            temp_num = (temp_num.to_f - 0.4)/0.0195 # Temperature Coefficient mV/°C old -> 0.01295 0.01195
            temp_num  = temp_num.round(2)
            puts "Temperature : #{temp_num}°C"
            #puts temp_num

            _,sl_num = Sl.split(':', -1)
            #sl_p = (100-((sl_num.to_f / 2.75) * 100)).round(2)
            sl_p = (100-(((sl_num.to_f-0.9)/(2.75-0.9))*100)).round(2)
            #puts sl_num
            puts "Soil moisture value sensor 1: #{sl_p} %"
            #puts sl_p

            _,sr_num = Sr.split(':', -1)
            #sr_p = (100-((sr_num.to_f / 3) * 100)).round(2)
            sr_p = (100-(((sr_num.to_f-0.9)/(2.75-0.9))*100)).round(2)
            puts "Soil moisture value sensor 2 : #{sr_p} %"
            #puts sr_p

            m.mqtt_publish "node3/sun",sun_p.to_s
            m.mqtt_publish "node3/temp",temp_num.to_s
            m.mqtt_publish "node3/soilL",sl_p.to_s
            m.mqtt_publish "node3/soilR",sr_p.to_s
            puts "########################################################################"

          rescue # optionally: `rescue StandardError => ex`
            puts 'Msg error'
        
          end
          
        elsif x == true
          puts "Stop !!!"
          puts x
          #sleep 1
          break
        end

        sleep 2
      end
    end

    def observe_mode2 
      puts "test observe_mode2"
      while true
        data_r = lora_read @uartnum
        puts data_r
        sleep 1
      end
    end

    def send_temp pin,to_address
      puts "Send Temp"
      while true
        T = ADC.read_temp pin
        T = T.to_s
        temp = "Temp:"+T
        puts temp
        lora_send @uartnum,to_address,temp
        sleep 2
      end
    end

  end
end
