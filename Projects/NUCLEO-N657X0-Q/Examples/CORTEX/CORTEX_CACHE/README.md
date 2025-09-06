## <b>CORTEX_CACHE Example Description</b>

This project provides a CORTEXM cache example based on the CMSIS API that can be used
to build any firmware application to execute from internal Flash.

This project is targeted to run on STM32N6xx device on NUCLEO-N657X0 board from STMicroelectronics.

The system clock runs at 600 MHz and external memory interface at the highest speed.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The example shows the consequence of the cache on data located in the cacheable area when
the memory is updated by a DMA operation and how to counteract this effect.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

The green LED turns on when the example executes as expected.
The red LED turns on in case of an error.

### <b>Keywords</b>

CORTEXM, CACHE, DMA, SRAM

### <b>Directory contents</b>

File | Description
 --- | ---
  CORTEX/CORTEX_CACHE/FSBL/Inc/main.h                   | Header for main.c module
  CORTEX/CORTEX_CACHE/FSBL/Inc/stm32n6xx_hal_conf.h     | HAL Configuration file
  CORTEX/CORTEX_CACHE/FSBL/Inc/stm32n6xx_it.h           | Interrupt handlers header file
  CORTEX/CORTEX_CACHE/FSBL/Inc/stm32n6xx_nucleo_conf.h  | BSP Configuration file
  CORTEX/CORTEX_CACHE/FSBL/Src/main.c                   | Main program
  CORTEX/CORTEX_CACHE/FSBL/Src/stm32n6xx_hal_msp.c      | HAL MSP module
  CORTEX/CORTEX_CACHE/FSBL/Src/stm32n6xx_it.c           | Interrupt handlers
  CORTEX/CORTEX_CACHE/FSBL/Src/system_stm32n6xx.c       | STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This example has been tested with STMicroelectronics NUCLEO-N657X0
    board and can be easily tailored to any other supported device
    and development board.

  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out.

  - **MDK-ARM** : To monitor a variable in the live watch window, you must comment out SCB_EnableDCache() in main() function.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.

