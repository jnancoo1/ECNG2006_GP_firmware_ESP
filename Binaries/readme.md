# Binary Files

## Sample command to flash in txt file
Modify the command to flash binaries to the ESP8266 based on your file paths

python /home/jonat/ESP8266_RTOS_SDK/components/esptool_py/esptool/esptool.py --chip esp8266 --port COM5 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dout --flash_freq 40m --flash_size 1MB 0x0 /home/jonat/ESP8266_RTOS_SDK/examples/get-started/hello_world/build/bootloader/bootloader.bin 0x10000 /home/jonat/ESP8266_RTOS_SDK/examples/get-started/hello_world/build/hello-world.bin 0x8000 /home/jonat/ESP8266_RTOS_SDK/examples/get-started/hello_world/build/partitions_singleapp.bin
