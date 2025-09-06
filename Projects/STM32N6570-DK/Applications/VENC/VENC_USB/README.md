
## <b>VENC_USB Application Description</b>

This application provides an example of VENC usage on STM32N6570-DK board,
it shows how to develop a USB video device using the camera pipeline and VENC IP.
It is also designed to be easily configurable:

  - Hardware handshake or frame mode
  - 1080p15, 720p30, 480p30

The application is designed to emulate a USB video device, the code provides all required device descriptors framework
and associated class descriptor report to build a compliant USB video device.

At the beginning ThreadX calls the entry function tx_application_define(), at this stage, all USBx resources
are initialized, the video class driver is registered and the application creates one thread:

  - app_ux_device_thread_entry (Prio : 10; PreemptionPrio : 10) used to initialize USB_OTG HAL PCD driver and start the device.

#### <b>Expected success behavior</b>

When plugged to PC host, the STM32N6570-DK must be properly enumerated as a USB video device.
During the enumeration phase, device provides host with the requested descriptors (device, configuration, string).
Those descriptors are used by host driver to identify the device capabilities.

Once the STM32N6570-DK USB device successfully completed the enumeration phase:
Use a camera tool supporting USB h264 (ffplay for example with the command  ffplay -f dshow -i video="STM32 Video Device" )

    - stream1 FORMAT : H264 and Width = 960, Height = 720.


#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (video device enumeration failed).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" specification and video class specification.

#### <b>Known limitations</b>

None.

### <b>Notes</b>

None

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it, by updating the "TX_TIMER_TICKS_PER_SECOND" define in the "tx_user.h" file. The update should be reflected in "tx_initialize_low_level.S" file too.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
 - ThreadX offers the "tx_application_define()" function, that is automatically called by the tx_kernel_enter() API.
   It is highly recommended to use it to create all applications ThreadX related resources (threads, semaphores, memory pools...)  but it should not in any way contain a system API call (HAL or BSP).
 - Using dynamic memory allocation requires to apply some changes to the linker file.
   ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
   using the "first_unused_memory" argument.
   This requires changes in the linker files to expose this memory location.
    + For EWARM add the following section into the .icf file:
     ```
     place in RAM_region    { last section FREE_MEM };
     ```

    + The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.

### <b>Keywords</b>

RTOS, ThreadX, USBX Device, USB_OTG, Full Speed, High Speed, Video, MJPEG.

### <b>Hardware and Software environment</b>

  - This application runs on STM32N657X0H3QU devices.
  - This application has been tested with STMicroelectronics SSTM32N6570-DK boards revision MB1939-N6570-C01 and can be easily tailored to any other supported device and development board.

  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out.

  - **MDK-ARM** : To monitor a variable in the live watch window, you must comment out SCB_EnableDCache() in main() function.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Select first the FSBL workspace
 - Rebuild all files from sub-project FSBL (if no modification is done on FSBL project, this step can be done only once)
 - Select the Firmware workspace (VENC_USB_Appli or VENC_USB_LRUN)
 - Rebuild all files from sub-project VENC_USB_XXX
 - Use CubeProgrammer to add a header to the generated VENC_USB_XXX binary Project.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Firmware-trusted.bin -hv 2.3 -dump Firmware-trusted.bin*
       - The resulting binary is Ux_Device_Video-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the Firmware binary and its header (Firmware-trusted.bin) in DK board external Flash at address 0x7010'0000.
 - Load the FSBL binary in internal RAM using the IDE
 - Run the example

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Use  CubeProgrammer to add a header to the generated binary FSBL.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin FSBL.bin -nk -of 0x80000000 -t fsbl -o FSBL-trusted.bin -hv 2.3 -dump FSBL-trusted.bin*
       - The resulting binary is FSBL-trusted.bin.
 - In resorting again to CubeProgrammer, load the FSBL binary and its header (FSBL-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.

 Note : when using CubeIDE, make sure to use the Debug configuration for development mode and the Release configuration for boot from flash.