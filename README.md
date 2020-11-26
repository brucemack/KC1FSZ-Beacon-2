RTTY/WSPR Beacon

Source Location, Link Folder for KC1FSZ-STM32-lib and ssd1306 

Please make sure to #define one of these:
* STM32F1
* STM32F4

Download the KC1FSZ-STM32-lib library and adjust this project's include/source
path to reference this project.

Download the SSD1306 library and adjust this project's include/source path to 
reference it: https://github.com/afiskon/stm32-ssd1306

Make sure to define these two symbols inside of the SSD1306_config.h file
* STM32F1
* SSD1306_HEIGHT=32
* SSD1306_USE_I2C
