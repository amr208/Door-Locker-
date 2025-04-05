# Dual-Microcontroller Door Locker Security System
# Password-Based Access Control with Motion Detection and Alarm

## Project Overview
This system implements a secure door locker using two ATmega32 microcontrollers (HMI_ECU and Control_ECU) communicating via UART. The system features password authentication stored in external EEPROM, motorized door control, motion detection, and alarm functionality.

## Hardware Components

### HMI_ECU (Human-Machine Interface)
- LCD (8-bit mode):
  - RS pin connected to PC0
  - E (Enable) pin connected to PC1
  - Data Pins (D0-D7) connected to Port A (PA0 to PA7)
- Keypad (4×4):
  - Rows connected to PB0-PB3
  - Columns connected to PB4-PB7
- UART Communication:
  - TXD connected to Control_ECU RXD
  - RXD connected to Control_ECU TXD

### Control_ECU (Main Controller)
- External EEPROM (I2C Communication):
  - SCL connected to PC0
  - SDA connected to PC1
- Buzzer connected to PC7
- H-bridge Motor Driver:
  - Input 1 connected to PD6
  - Input 2 connected to PD7
  - Enable1 connected to PB3/OC0
- Motor for Door Control connected to H-bridge
- PIR Motion Sensor connected to PC2

## System Features

### Password Security
- First-time password setup stored in external EEPROM
- Password verification:
  - Correct password unlocks door
  - Three wrong attempts trigger 1-minute lockout and buzzer alarm
- Password change option after authentication

### Door Control
- Motorized lock/unlock via H-bridge and DC motor
- PIR motion sensor for automatic locking

### User Interface
- LCD displays system status and prompts
- 4×4 Keypad for password input

### Alarm System
- Buzzer activates on:
  - Three failed password attempts
  - Unauthorized access attempts

## Communication Protocol
- UART between HMI_ECU and Control_ECU
- Baud Rate: 9600
- Data Format: 8-bit, No Parity, 1 Stop Bit
- Commands:
  - 'A' - Password correct (unlock door)
  - 'B' - Password wrong
  - 'C' - Change password request

## Software Architecture

### HMI_ECU (Keypad and LCD)
1. Keypad Scanning - Detects user input
2. Password Entry - Sends to Control_ECU via UART
3. LCD Feedback - Displays system status

### Control_ECU (Security and Motor Control)
1. EEPROM Access - Stores/retrieves password
2. Password Verification - Compares input with stored password
3. Motor Control - Unlocks/locks door via H-bridge
4. PIR Sensor - Detects motion for auto-locking
5. Buzzer Control - Alerts on security breaches

## Security Measures
- EEPROM Storage - Passwords persist after power-off
- Three-Attempt Lockout - Prevents brute-force attacks
- Motion-Based Locking - PIR ensures door locks when idle
- Encrypted UART - Optional protection against eavesdropping

## Potential Upgrades
- Bluetooth/Wi-Fi for remote unlocking
- Fingerprint Sensor for biometric authentication
- Real-Time Clock for time-based access control
- Camera Module for intrer detection and logging

## Project Files
- HMI_ECU.c - Handles LCD, Keypad, UART TX/RX
- Control_ECU.c - Manages EEPROM, Motor, Buzzer, PIR
- I2C_EEPROM.c - External EEPROM read/write functions
- UART_Protocol.c - UART communication between MCUs
- Motor_Driver.c - H-bridge PWM control for door motor


