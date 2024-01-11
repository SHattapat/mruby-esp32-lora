module LORA
  puts "LoRa Version....02 GIT"
  include ESP32::STANDARD
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
      #o.send 1,"ON"
      #o.send 2,"ON"
      lora_read @uartnum
    end

    def set_cmode cmode
      #o.set_cmode MODE_1
      lora_set_cmode @uartnum,cmode
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

    def observe_mode pin
      puts "test observe_mode"
      x = GPIO.high_at? pin
      while x == false
        x = GPIO.high_at? pin
        data_r = lora_read @uartnum
        puts data_r
        sleep 1
        if x == true
          puts x
          sleep 1
          break
         end
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
