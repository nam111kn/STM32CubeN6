## <b>DMA2D_MemToMemWithPFC DMA2D Memory to Memory with PFC example Description</b>

This example provides a description of how to configure the DMA2D peripheral in Memory-to-memory transfer mode with
pixel format conversion (PFC) mode.

- This project is targeted to run on STM32N6xx device on Discovery STM32N6570-DK board from STMicroelectronics.

- This project runs from internal RAM. CPU clock is set at 600 MHz.

- This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

- In this example, the code is executed from SRAM memory.

- At the beginning of the main program the HAL_Init() function is called to set the SysTick.

- The SystemClock_Config() function is used to configure the CPU clock for STM32N6xx Devices :
  The CPU at 600MHz

  In this basic example the goal is to explain the different fields of the DMA2D 
  structure in the case of Memory-to-memory with pixel format conversion (PFC) transfer mode
  and the difference between pixel coded on 16bits (RGB565 format) and coded on 24bits (RGB888 format).
  
  The transformed picture is shown on an LCD screen, which is set up to present an RGB888 format image
  from the DMA2D's destination address.
  
- How to convert pixel format from **RGB565** to **RGB888** ?
 => transparency information is discarded 
 Extract Color Channels from RGB565:

 Red (R): Extract the 5-bit red value.
 Green (G): Extract the 6-bit green value.
 Blue (B): Extract the 5-bit blue value.
 Expand Color Channels to 8 Bits:

 Red (R): Scale the 5-bit value to 8 bits by replicating the MSBs.
 From R4..R0, expand to R7..R0.
 Green (G): Scale the 6-bit value to 8 bits by replicating the MSBs.
 From G5..G0, expand to G7..G0.
 Blue (B): Scale the 5-bit value to 8 bits by replicating the MSBs.
 From B4..B0, expand to B7..B0.
</pre>

- After DMA2D configuration, the data transfer is performed and then the transferred
data is checked by byte to byte memory comparing of the source and destination buffers aBufferInput[] and aBufferResult[].
\
 - [**LD1**]{style="color: green"} is ON for 100ms then OFF when the DMA2D transfer is complete.
\
 - [**LD1**]{style="color: green"} is ON when data correctly transferred
\
 - [**LD2**]{style="color: red"} TOGGLES when there is an error occurs.


#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.
 

### <b>Keywords</b>

Graphics, DMA2D, Display ,SRAM, RGB565, Pixel Format Conversion (PFC), Memory to memory, Foreground,
Background, pixel

### <b>Directory contents</b>
### <b>Directory contents</b>

File | Description
 --- | ---
  DMA2D/DMA2D_MemToMemWithPFC/FSBL/Inc/main.h                       | Header for main.c module
  DMA2D/DMA2D_MemToMemWithPFC/FSBL/Inc/stm32n6xx_hal_conf.h         | HAL Configuration file
  DMA2D/DMA2D_MemToMemWithPFC/FSBL/Inc/stm32n6xx_it.h               | Interrupt handlers header file
  DMA2D/DMA2D_MemToMemWithPFC/FSBL/Inc/stm32n6570_discovery_conf.h  | DMA2D Configuration file
  DMA2D/DMA2D_MemToMemWithPFC/FSBL/Src/main.c                       | Main program
  DMA2D/DMA2D_MemToMemWithPFC/FSBL/Src/stm32n6xx_it.c               | Interrupt handlers
  DMA2D/DMA2D_MemToMemWithPFC/FSBL/Src/system_stm32n6xx.c           | STM32N6xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This example has been tested with STMicroelectronics STM32N6570-DK
    board.

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

