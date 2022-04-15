# ECE 3375 Final Project
Cameron Bourdeau, Braeden Wolf, Austin Zhang, Varun Noah Chauhan

## Home thermostat supporting multiple temperature sensors
Supports the following functionalities:
- Reading temperature data from up to 8 analog temperature sensors (e.g. thermistors; represented by the ADC peripheral in the prototype)
- Controlling an HVAC system with support for heat, cooling, and circulation fan (represented by the red LEDs in the prototype)
- Displaying temperature data and system status on the VGA display for optimal readability

### Compilation instructions
- `final.c` is the only file required.
- Use the compiler flag `-std=c99` at the end of USERCCFLAGS in makefile to ensure the program is run on C99
