# SputterMachineSimulator

**Compiled using MinGW64 on Qt 5.12.2 (Windows).**

This software is used strictly with the SputterAutomation project.

## Prerequisites
1. A virtual COM port emulator. Null-modem emulator (com0com) is highly recommended.
  - Links:
    - http://com0com.sourceforge.net/
    - https://sourceforge.net/projects/com0com/      
2. Ensure that the following COM ports are unused:
  * COM1
  * COM2
  * COM3
  * COM4
  * COM6
3. Microsoft Loopback Adapter.
  - Instructions: https://www.schneider-electric.com/en/faqs/FA240071/
  - When filling in the IP address field, enter 192.168.0.2.
  - Allow any connections regarding the Loopback Adapter in your firewall.
  
## Using the Null-modem emulator (com0com)
1. Open (Installation Directory)\com0com\setupg.exe
2. Click "Add Pair".
3. In the newly created pair, check boxes "Use Ports class" and "emulate baud rate".
4. Change one of the port names to one of the ports listed above in **Prerequisites Step 2**. The other port in the pair can be named as you wish. Example: COM5, COM8, COM9...etc.
5. Click "Apply".
6. Repeat step 2 to step 5 until all the ports in **Prerequisites Step 2** have been assigned a pair.
7. An example of a completed setup would look like the following:
     * COM5 <-----> COM1
     * COM7 <-----> COM2
     * COM8 <-----> COM3
     * COM9 <-----> COM4
     * COM10 <-----> COM6
