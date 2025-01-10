## <b>OEMuROT_Boot Application Description</b>

This project provides an OEMuROT example. OEMuROT boot path performs authenticity and the integrity checks of the project firmware and data images.


The core function of this application relies on the mcuboot middleware and the mbed-crypto middleware.


It relies on cryptography hardware peripherals for optimal performances (PKA, SAE, HASH, RNG).


This project is full secure.


This project shows how to implement an OEMuROT.


To ease the development process, a prebuild command and postbuild command are integrated in the project. The prebuild command is in charge of preparing the OEMuROT\_Boot scatter file according to flash layout description. The postbuild command is in charge of preparing the provisioning scripts and the application image (firmware application managed by OEMuROT) configuration files in ROT\_Provisioning/OEMuROT folder, according to the flash layout and OEMuROT configuration.


The maximum system clock frequency at 600Mhz is configured.


### <b>Keywords</b>

OEMuROT, TrustZone, Root Of Trust, Security, mcuboot


### **Directory contents**







| File | Description |
| --- | --- |
| OEMuROT\_Boot/Inc/mcuboot\_config/mcuboot\_config.h | Mcuboot configuration file |
| OEMuROT\_Boot/Inc/boot\_hal\_cfg.h | Platform configuration file for OEMuROT\_Boot |
| OEMuROT\_Boot/Inc/boot\_hal\_flowcontrol.h | Header file for flow control code in boot\_hal.c |
| OEMuROT\_Boot/Inc/boot\_hal\_hash\_ref.h | Header file for hash reference code in boot\_hal.c |
| OEMuROT\_Boot/Inc/boot\_hal\_imagevalid.h | Header file for image validation code in boot\_hal.c |
| OEMuROT\_Boot/Inc/boot\_hal\_mce.h | Header file for boot\_hal\_mce.c |
| OEMuROT\_Boot/Inc/cmsis.h | Header file for CMSIS |
| OEMuROT\_Boot/Inc/config-boot.h | Mbed-crypto configuration file |
| OEMuROT\_Boot/Inc/debug\_authentication.h | Header file for debug\_authentication.c |
| OEMuROT\_Boot/Inc/flash\_layout.h | Flash mapping |
| OEMuROT\_Boot/Inc/low\_level\_ext\_flash.h | Header file for low\_level\_ext\_flash.c |
| OEMuROT\_Boot/Inc/low\_level\_ext\_ram.h | Header file for low\_level\_ext\_ram.c |
| OEMuROT\_Boot/Inc/low\_level\_mce.h | Header file for low\_level\_mce.c |
| OEMuROT\_Boot/Inc/low\_level\_otp.h | Header file for low\_level\_otp.c |
| OEMuROT\_Boot/Inc/low\_level\_rng.h | Header file for low\_level\_rng.c |
| OEMuROT\_Boot/Inc/low\_level\_security.h | Header file for low\_level\_security.c |
| OEMuROT\_Boot/Inc/region\_defs.h | RAM and FLASH regions definitions |
| OEMuROT\_Boot/Inc/stm32\_extmem\_conf.h | Header configuration for extmem module |
| OEMuROT\_Boot/Inc/stm32n6xx\_hal\_conf.h | HAL driver configuration file |
| OEMuROT\_Boot/Inc/target\_cfg.h | Header file for target start up |
| OEMuROT\_Boot/Src/bl2\_nv\_services.c | Non Volatile services for OEMuROT\_Boot |
| OEMuROT\_Boot/Src/boot\_hal.c | Platform initialization |
| OEMuROT\_Boot/Src/debug\_authentication.c | Debug authentication module |
| OEMuROT\_Boot/Src/image\_macros\_to\_preprocess\_bl2.c | Images definitions to preprocess for bl2 |
| OEMuROT\_Boot/Src/keys\_map.c | Keys interface |
| OEMuROT\_Boot/Src/low\_level\_com.c | UART low level interface |
| OEMuROT\_Boot/Src/low\_level\_device.c | Flash low level device configuration |
| OEMuROT\_Boot/Src/low\_level\_ext\_flash.c | External flash low level interface |
| OEMuROT\_Boot/Inc/low\_level\_ext\_ram.c | External ram low level interface |
| OEMuROT\_Boot/Inc/low\_level\_extmem\_device.c | External memory module interface |
| OEMuROT\_Boot/Inc/low\_level\_mce.c | MCE interface |
| OEMuROT\_Boot/Inc/low\_level\_otp.c | OTP interface |
| OEMuROT\_Boot/Src/low\_level\_rng.c | Random generator interface |
| OEMuROT\_Boot/Src/low\_level\_security.c | Security Low level services |
| OEMuROT\_Boot/Src/mpu\_armv8m\_drv.c | Mpu low level interface |
| OEMuROT\_Boot/Src/stm32n6xx\_hal\_msp.c | HAL MSP module |
| OEMuROT\_Boot/Src/system\_stm32n6xx.c | System Init file |
| OEMuROT\_Boot/Src/tick.c | HAL Tick implementation |


### **Hardware and Software environment**


* This example runs on STM32N657xx devices.
* This example has been tested with STMicroelectronics STM32N6570-DK (MB1939) board and can be easily tailored to any other supported device and development board.
* To get debug print in your UART console you have to configure it using these parameters: Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.


### **How to use it ?**


In order to build the OEMuROT\_Boot project, you must do the following:


* Open your preferred toolchain
* Rebuild the project


Then refer to OEMuROT\_Appli readme for example of application booted through OEMuROT boot path.






