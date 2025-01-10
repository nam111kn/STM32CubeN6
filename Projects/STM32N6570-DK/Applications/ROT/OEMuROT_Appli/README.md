## <b>OEMuROT_Appli Application Description</b>

This project provides a OEMuROT boot path application example. Boot is performed through OEMuROT boot path after authenticity and the integrity checks of the project firmware and project data images.


This project is composed of two sub-projects:


* One for the secure application part (Project\_s)
* One for the non-secure application part (Project\_ns).


Please remember that the system always boots in secure and the secure application is responsible for launching the non-secure application. When the secure application is started the MPU is already configured (by OEMuROT) to limit the execution area to the project firmware execution slot. This is done in order to avoid any malicious code execution from an unauthorised area (RAM, out of execution slot in user flash …). Once started, it is up to the secure application to adapt the security configuration to its needs.


This project mainly shows how to switch from secure application to non-secure application thanks to the system isolation performed to split the external flash and SRAM memories. The split of the external flash depends on OEMuROT configuration done through the provisioning steps. The split of the SRAM memories is independent from provisioning process and can be updated at any time before compilation steps.


The non-secure application will display a menu on the console allowing to download a new version of the application firmware and the application data images. At next reset, the OEMuROT will decrypt and install these new versions after successful check of the authenticity and the integrity of each image.


### <b>Keywords</b>

OEMuROT, TrustZone, boot path, Root Of Trust, MPU

### **Directory contents**







| File | Description |
| --- | --- |
| ROT/OEMuROT\_Appli/Secure/Src/low\_level\_device.c | Secure Low level device configuration |
| ROT/OEMuROT\_Appli/Secure/Src/low\_level\_ext\_flash.c | Secure Low level external flash driver |
| ROT/OEMuROT\_Appli/Secure/Src/main.c | Secure Main program |
| ROT/OEMuROT\_Appli/Secure/Src/secure\_nsc.c | Secure Non-Secure Callable (NSC) module |
| ROT/OEMuROT\_Appli/Secure/Src/stm32n6xx\_hal\_msp.c | Secure HAL MSP module |
| ROT/OEMuROT\_Appli/Secure/Src/stm32n6xx\_it.c | Secure Interrupt handlers |
| ROT/OEMuROT\_Appli/Secure/Src/system\_stm32n6xx\_s.c | Secure STM32N6xx system clock configuration file |
| ROT/OEMuROT\_Appli/Secure/Inc/low\_level\_ext\_flash.h | Secure Header file for low\_level\_ext\_flash.c |
| ROT/OEMuROT\_Appli/Secure/Inc/main.h | Secure Main program header file |
| ROT/OEMuROT\_Appli/Secure/Inc/stm32\_extmem\_conf.h | Secure Header configuration for extmem module |
| ROT/OEMuROT\_Appli/Secure/Inc/stm32n6xx\_hal\_conf.h | Secure HAL Configuration file |
| ROT/OEMuROT\_Appli/Secure/Inc/stm32n6xx\_it.h | Secure Interrupt handlers header file |
| ROT/OEMuROT\_Appli/Secure/nsclib/appli\_flash\_layout.h | Flash layout header file |
| ROT/OEMuROT\_Appli/Secure/nsclib/secure\_nsc.h | Secure Non-Secure Callable (NSC) module header file |
| ROT/OEMuROT\_Appli/NonSecure/Src/com.c | Non-secure Communication |
| ROT/OEMuROT\_Appli/NonSecure/Src/common.c | Non-secure common |
| ROT/OEMuROT\_Appli/NonSecure/Src/fw\_update\_app.c | Non-Secure firmware update service |
| ROT/OEMuROT\_Appli/NonSecure/Src/main.c | Non-secure Main program |
| ROT/OEMuROT\_Appli/NonSecure/Src/ns\_data.c | Non-secure data display module |
| ROT/OEMuROT\_Appli/NonSecure/Src/stm32n6xx\_it.c | Non-secure Interrupt handlers |
| ROT/OEMuROT\_Appli/NonSecure/Src/system/stm32n6xx\_ns.c | Non-secure STM32N6xx system clock configuration file |
| ROT/OEMuROT\_Appli/NonSecure/Src/ymodem.c | Non-secure YModem module |
| ROT/OEMuROT\_Appli/NonSecure/Src/com.h | Non-secure Communication header file |
| ROT/OEMuROT\_Appli/NonSecure/Src/common.h | Non-secure common header file |
| ROT/OEMuROT\_Appli/NonSecure/Src/low\_level\_flash.h | Non-Secure Low level flash driver header file |
| ROT/OEMuROT\_Appli/NonSecure/Inc/main.h | Non-secure Main program header file |
| ROT/OEMuROT\_Appli/NonSecure/Inc/stm32n6xx\_hal\_conf.h | Non-secure HAL Configuration file |
| ROT/OEMuROT\_Appli/NonSecure/Inc/stm32n6xx\_it.h | Non-secure Interrupt handlers header file |
| ROT/OEMuROT\_Appli/Binary | Application firmware image (application binary + MCUBoot header and metadata) |


### **Hardware and Software environment**


* This example runs on STM32N657xx devices.
* This example has been tested with STMicroelectronics STM32N6570-DK (MB1939) board and can be easily tailored to any other supported device and development board.
* To print the application menu in your UART console you have to configure it using these parameters: Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.


### **How to use it ?**


This project is targeted to boot through **OEMuROT boot path**.


Before compiling the project, you should first start the provisioning process. During the provisioning process, the linker files of Project\_s and Project\_ns as well as the postbuild commands will be automatically updated.


Before starting the provisioning process, select the application project to use (application example or template), through oemurot\_appli\_path\_project variable in ROT\_Provisioning/env.bat or env.sh. Then start provisioning process by running ROT\_Provisioning/OEMuROT/provisioning.bat or .sh, and follow its instructions.


For more details, refer to Wiki article available here : [Link to Security Wiki](https://wiki.st.com/stm32mcu/wiki/Category:Security)


After application startup, check in your "UART console" the menu is well displayed:



```
=================== Main Menu ============================
 Non-Secure Data --------------------------------------- 1
 New Fw Image ------------------------------------------ 2
 Selection :

```

To update the application firmware and/or the application data image version, you must:


* Select the function "New Fw Image"
* Select the type of image to update
* Send the signed binary by using the menu : File > Transfer > YMODEM > Send…
* Reset the board
* After authenticity and intergrity checks the new images are decrypted and installed.


#### **Notes:**


1. Two versions of ROT\_AppliConfig are available: windows executable and python version. By default, the windows executable is selected. It is possible to switch to python version by:


	* installing python (Python 3.10 or newer) with the required modules listed in requirements.txt.
```
pip install -r requirements.txt
```

	* having python in execution path variable
	* deleting main.exe in Utilities\_Software\_AppliConfig






