# F1_SDCARD_BOOTLOADER


Hi Everyone. 
I maked a bootloader using a .bin file from sdcard with stm32f103 bluepill board.

bootloader_init function is makes load software from sdcard, clears the flash memory, runs the current software in flash memory.
I used FATFS for reading data from sdcard.
I used spi sdcard module for reading data from sdcard. You can use sdio protocol or other boards.
I used 2 buttons for bootloading.
First button makes load, second button makes clear.

After restarting;

if press the load button quickly, it loads .bin file from sdcard.
if press the clear button quickly, it clears flash memory for writing software.

otherwise;

bootloader function if have a software in flash memory, it runs the software in flash memory.
bootloader function if haven't a software in flash memory, it waits until take load command from user.

app-demo is making sliding between three leds.
