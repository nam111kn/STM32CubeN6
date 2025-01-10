## <b>OEMuROT_Appli Template Description</b>

This project provides a OEMuROT boot path application example. Boot is performed through OEMuROT boot path after authenticity and integrity checks of the project firmware and project data
images.


At the beginning of the main program, the HAL\_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.


The SystemClock\_Config() function is used to configure the system clock (SYSCLK)


This project is composed of two sub-projects: 


* One for the secure application part (Project\_s)
* One for the non-secure application part (Project\_ns).


Please remember that the system always boots in secure and the secure application is responsible for launching the non-secure application. When the secure application is started the MPU is already configured (by OEMuROT) to limit the execution area to the project firmware execution slot. This is done in order to avoid any malicious code execution from an unauthorised area (RAM, out of execution slot in user flash …). Once started, it is up to the secure application to adapt the security configuration to its needs.


This project mainly shows how to switch from secure application to non-secure application thanks to the system isolation performed to split the external flash and SRAM memories. The split of the external flash depends on OEMuROT configuration done through the provisioning steps. The split of the SRAM memories is independent from provisioning process and can be updated at any time before compilation steps.


#### **Notes**


1. Care must be taken when using HAL\_Delay(), this function provides accurate delay (in milliseconds)
based on variable incremented in SysTick ISR. This implies that if HAL\_Delay() is called from
a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
To change the SysTick interrupt priority you have to use HAL\_NVIC\_SetPriority() function.
2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
to have correct HAL operation.


### **Keywords**


TrustZone, OEMuROT, Boot path, Root Of Trust, MPU


### **Directory contents**


* Templates\_ROT/AppliSecure/Inc/main.h Header file for main.c
* Templates\_ROT/AppliSecure/Inc/stm32n6xx\_hal\_conf.h HAL configuration file
* Templates\_ROT/AppliSecure/Inc/stm32n6xx\_it.h Header file for stm32n6xx\_it.h
* Templates\_ROT/AppliSecure/Src/main.c Secure Main program
* Templates\_ROT/AppliSecure/Src/secure\_nsc.c Secure nonsecure callable functions
* Templates\_ROT/AppliSecure/Src/stm32n6xx\_it.c Secure Interrupt handlers
* Templates\_ROT/AppliSecure/Src/system\_stm32n6xx\_s.c Secure STM32n6xx system clock configuration file
* Templates\_ROT/Secure\_nsclib/secure\_nsc.h Header file for nonsecure callable functions
* Templates\_ROT/AppliNonSecure/Inc/main.h Header file for main.c
* Templates\_ROT/AppliNonSecure/Inc/stm32n6xx\_hal\_conf.h HAL configuration file
* Templates\_ROT/AppliNonSecure/Inc/stm32n6xx\_it.h Header file for stm32n6xx\_it.c
* Templates\_ROT/AppliNonSecure/Inc/stm32n6570\_discovery\_conf.h Header file for stm32n6570\_discovery\_conf.c
* Templates\_ROT/AppliNonSecure/Src/main.c Non secure Main application file
* Templates\_ROT/AppliNonSecure/Src/stm32n6xx\_hal\_msp.c Non secure HAL MSP file
* Templates\_ROT/AppliNonSecure/Src/stm32n6xx\_it.c Non secure Interrupt handlers
* Templates\_ROT/AppliNonSecure/Src/system\_stm32n6xx.c Non secure System init file
* Templates\_ROT/EWARM/startup\_stm32n6xx.s Startup file
* Templates\_ROT/Binary Application firmware image (application binary + MCUBoot header and metadata)


### **Hardware and Software environment**


* This example runs on STM32N657xx devices.
* This example has been tested with STMicroelectronics STM32N6570-DK (MB1939) board and can be easily tailored to any other supported device and development board.


### **How to use it ?**


This project is targeted to boot through **OEMuROT boot path**.


Before compiling the project, you should first start the provisioning process. During the provisioning process, the linker files of Project\_s and Project\_ns as well as the postbuild commands will be automatically updated.


Before starting the provisioning process, select the application project to use (application example or template), through oemurot\_appli\_path\_project variable in ROT\_Provisioning/env.bat or env.sh. Then start provisioning process by running ROT\_Provisioning/OEMuROT/provisioning.bat or .sh, and follow its instructions.


For more details, refer to Wiki article available here : [Link to Security Wiki](https://wiki.st.com/stm32mcu/wiki/Category:Security)


#### **Notes:**


1. Two versions of ROT\_AppliConfig are available: windows executable and python version. By default, the windows executable is selected. It
is possible to switch to python version by: 


	* installing python (Python 3.10 or newer) with the required modules listed in requirements.txt. 
	```
	pip install -r requirements.txt
	
	```
	* having python in execution path variable
	* deleting main.exe in Utilities\PC\_Software\ROT\_AppliConfig\dist






