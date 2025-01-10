## <b>BootROM Related Scripts Description</b>

This section provides the necessary scripts to generate and program BootROM keys.


To ease the configuration process, enable\_secure\_boot, keys\_generation, and keys\_programming scripts are used.
The enable\_secure\_boot script allows the switching of the device to CLOSED\_UNLOCKED state. After this step the BootROM only accepts signed images.
The keys\_generation script generate new keys in the folder \Keys\.
The keys\_programming script program necessary keys that allows BootROM to run signed/encrypted images.


### **Keywords**


BootROM, Root Of Trust, Security


### **Directory contents**


* BootROM/enable\_secure\_boot.bat/.sh Change device state to CLOSED\_UNLOCKED.
* BootROM/keys\_generation.bat/.sh Generate new BootROM keys.
* BootROM/keys\_programming.bat/.sh Program BootROM keys into OTP.
* OEMuROT/Keys/OEM\_SECRET.bat/.sh Encryption key.
* OEMuROT/Keys/privateKey00.pem Private authentication key 0.
* OEMuROT/Keys/privateKey01.pem Private authentication key 1.
* OEMuROT/Keys/privateKey02.pem Private authentication key 2.
* OEMuROT/Keys/privateKey03.pem Private authentication key 3.
* OEMuROT/Keys/privateKey04.pem Private authentication key 4.
* OEMuROT/Keys/privateKey05.pem Private authentication key 5.
* OEMuROT/Keys/privateKey06.pem Private authentication key 6.
* OEMuROT/Keys/privateKey07.pem Private authentication key 7.
* OEMuROT/Keys/publicKey00.pem Public authentication key 0.
* OEMuROT/Keys/publicKey01.pem Public authentication key 1.
* OEMuROT/Keys/publicKey02.pem Public authentication key 2.
* OEMuROT/Keys/publicKey03.pem Public authentication key 3.
* OEMuROT/Keys/publicKey04.pem Public authentication key 4.
* OEMuROT/Keys/publicKey05.pem Public authentication key 5.
* OEMuROT/Keys/publicKey06.pem Public authentication key 6.
* OEMuROT/Keys/publicKey07.pem Public authentication key 7.
* OEMuROT/Keys/publicKeyHash00.pem Public authentication key hash 0.
* OEMuROT/Keys/publicKeyHash01.pem Public authentication key hash 1.
* OEMuROT/Keys/publicKeyHash02.pem Public authentication key hash 2.
* OEMuROT/Keys/publicKeyHash03.pem Public authentication key hash 3.
* OEMuROT/Keys/publicKeyHash04.pem Public authentication key hash 4.
* OEMuROT/Keys/publicKeyHash05.pem Public authentication key hash 5.
* OEMuROT/Keys/publicKeyHash06.pem Public authentication key hash 6.
* OEMuROT/Keys/publicKeyHash07.pem Public authentication key hash 7.
* OEMuROT/Keys/publicKeysHashHashes.pem The hash of authentication keys hashes.


### **Hardware and Software
environment**


* This example runs on STM32N657xx devices.
* This example has been tested with STMicroelectronics STM32N6570-DK (MB1939)
board and can be easily tailored to any other supported device and development board.


Environment Setup
-----------------


All scripts rely on env.bat and env.sh for setting the necessary tools path and application path.


* File path : ROT\_Provisioning/env.bat/.sh.
* Purpose : Sets the necessary tools path and application path for Windows, Linux and Mac operating system.
* Usage : Exporting the Environment Variables, Path Configuration, Dependency Setup, Perform initial setup tasks.


Additional Resources
--------------------


[OEMuROT\_STM32N657](https://wiki.st.com/stm32mcu)




