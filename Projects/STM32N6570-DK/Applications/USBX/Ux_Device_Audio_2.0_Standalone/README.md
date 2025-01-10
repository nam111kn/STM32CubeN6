
## <b>Ux_Device_Audio_2.0_Standalone Application Description</b>

This application provides an example of standalone USBX stack usage on STM32N6570-DK board, it shows how to develop USB Device "AUDIO" Class based application.

The application is designed to emulate an USB (Out: Speaker/Headset), the code provides all required device descriptors framework
and associated Class descriptor report to build a compliant USB AUDIO device.

This is a typical application on how to use the STM32N6xx USB OTG Device peripheral and I2S peripheral to stream audio data from USB Host to the audio codec implemented on the STM32N6570-DK board.

The device supports the following audio features:
  - Pulse Coded Modulation (PCM) format
  - sampling rate: 48KHz.
  - Bit resolution: 16
  - Number of channels: 2
  - No volume control
  - Mute capability
  - Asynchronous Endpoints

#### <b>Expected success behavior</b>

When plugged to PC host, the STM32N6570-DK should enumerate as an USB Speaker Device.
During the enumeration phase, device must provide host with the requested descriptors (device descriptor, configuration descriptor, string descriptors).
Those descriptors are used by host driver to identify the device capabilities. Once STM32N6570 USB device successfully completed the enumeration phase,
Start streaming Audio.

#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (Audio Device enumeration failed).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" specification and AUDIO 2.0 class specification.

#### <b>Known limitations</b>

 - This application does not support synchronization.
 - Footprint are not optimized.

### <b>Notes</b>

None

#### <b>USBX usage hints</b>

- None

### <b>Keywords</b>

USBXDevice, USB_OTG, High speed, SAI, Audio, Streaming, PCM

### <b>Hardware and Software environment</b>

  - This application runs on STM32N657X0H3QU devices.
  - This application has been tested with STMicroelectronics STM32N6570-DK boards revision MB1939-N6570-C01 and can be easily tailored to any other supported device and development board.

  - **EWARM** : To monitor a variable in the live watch window, you must proceed as follow :
    - Start a debugging session.
    - Open the View > Images.
    - Double-click to deselect the second instance of project.out. 

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.
 - Run the application

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Ux_Device_Audio_2.0_Standalone.bin with the following command
   - *STM32_SigningTool_CLI.exe -bin Ux_Device_Audio_2.0_Standalone.bin -nk -of 0x80000000 -t fsbl -o Ux_Device_Audio_2.0_Standalone-trusted.bin -hv 2.3 -dump Ux_Device_Audio_2.0_Standalone-trusted.bin*
       - The resulting binary is Ux_Device_Audio_2.0_Standalone-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Ux_Device_Audio_2.0_Standalone-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.