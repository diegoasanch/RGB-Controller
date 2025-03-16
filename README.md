# RGB LED Strip controller

RGB LED strip controller + mini Weather station, built using the Arduino platform.

## File Structure

- `cli/`: Python scripts for building and uploading the project
- `src/`
  - `src.ino` : Entry point 
  - `config.h`:  Pin mappings, refresh rates, device version
  - `env.h`:  WiFi config
  - `components/`: Hardware components
  - `domain/`: System logic

## Use the device

### Toggle light

```bash
curl -X POST '<controller_ip>/light/toggleAnimated'
```

### Set light color

```bash 
curl -X POST '<controller_ip>/light/hexColor/<hex_color>'
```

### Set light brightness

```bash
curl -X POST '<controller_ip>/light/brightnessAnimated/<brightness>'
```



## Project setup

The project is configured to be used with the VSCode editor for editing comfort, but can definitely be 
built with just the `arduino-cli` and `python 3`. 

### VSCode configuration

1. Create a copy of the `.vscode/c_cpp_properties.example.json` file and rename it to `.vscode/c_cpp_properties.json`

2. Install the [Arduino extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino)

3. Install the required libraries using the "Arduino: Library Manager" command from the command palette.
    - DHT sensor library by Adafruit
    - AsyncTimer by Aasim-A
    - ArduinoJson by Benoit Blanchon  


4. Run the "Arduino: Rebuild IntelliSense Configuration" command from the command palette. 

5. Create a copy of the `src/env.example.h` file and rename it to `src/env.h`. 
    - Fill in the required values for the WiFi SSID and password.

### Building the project

1. Install the `arduino-cli` tool from [here](https://arduino.github.io/arduino-cli/latest/installation/)

2. Install the Python dependencies:
```bash
pip install -r cli/requirements.txt
```

3. Run the `compile.py` script to build the project:
```bash
python3 cli/compile.py <version_number>
```

The script will create a `build/bin` directory and place the binary in it.
The binary will be named `<version_number.bin>`.

### Uploading the binary

There are two ways to upload the binary to the micro controller:

1. USB upload: Required for first-time upload or when the board is not connected to WiFi
2. OTA update (Over The Air): Can be used for subsequent updates when the board is connected to your local network

The `run.py` script handles both methods and includes compilation, uploading, and monitoring in a single command:

```bash
# USB upload (will also open serial monitor after upload)
python3 cli/run.py <version_number>

# OTA update
python3 cli/run.py <version_number> --ota <controller_ip>

# OTA update with custom timeout (default is 60 seconds)
python3 cli/run.py <version_number> --ota <controller_ip> --timeout 90
```

Examples:
```bash
# Upload v1.3.2 via USB
python3 cli/run.py v1.3.2

# Update to v1.3.2 over WiFi (controller at 192.168.1.100)
python3 cli/run.py v1.3.2 --ota 192.168.1.100
```

The script will:
1. Compile the code for your board
2. Upload it using the selected method (USB or OTA)
3. For USB uploads: Open the serial monitor automatically
4. For OTA updates: Wait for the update to complete and verify the board is back online

#### Manual Upload Methods

If you prefer to upload manually, you can use these commands:

##### 1. USB Upload
```bash
arduino-cli upload -i ./build/bin/<version_number>.bin -b esp8266:esp8266:d1_mini_clone -p /dev/cu.usbserial-120
```

##### 2. OTA Update

1. Start the update server:
```bash
python3 cli/server.py
```

2. Trigger the update on the controller:
```bash
curl -X POST '<controller_ip>/settings/version/update/<update_server_ip>/<update_server_port>/update'
```

## Hardware

### Components

- WeMos D1 mini (micro controller)
- DHT22 (temperature & humidity sensor)
- LM2596S Step-down voltage regulator
- IRLZ44N (N-channel, logic-level MOSFET Transistor) x 3
- 10kΩ Resistor x 4
- 5.5X2.1mm DC Power jack
- SMD 5050 RGB Strip

### Circuit diagram

![Circuit](./Diagrams/Diagram_bb.png)
