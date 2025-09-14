# Simon Says Arduino Game

A classic Simon Says memory game implemented on ESP32 microcontrollers with NeoPixel LEDs, OLED display, and tactile buttons.

## Features

- **Memory Challenge**: Follow increasingly complex color sequences
- **Visual Feedback**: 24 NeoPixel LED ring with color animations and rainbow effects
- **Audio Cues**: Buzzer with musical tones for each color and special sound effects
- **Display**: 128x64 OLED screen with game status, scores, and animated fireworks
- **High Score Tracking**: Persistent storage of best performance
- **Reset Functionality**:
  - Short press: Restart system
  - Long press (7+ seconds): Reset high score with visual confirmation
- **Celebration Effects**: Synchronized lights, sounds, and display fireworks for new records
- **Power Management**: Optimized for battery operation

## Hardware Requirements

### Supported Boards
- **Arduino Nano ESP32** (primary)
- **Seeed Xiao ESP32-C6** (alternative)

### Components
- 24x NeoPixel LED Ring (WS2812B)
- 128x64 OLED Display (SSD1306, I2C)
- Passive Buzzer
- 4x Tactile Push Buttons (Red, Green, Blue, Yellow)
- 1x Reset Button
- Power supply (4×AA batteries recommended, or USB)

### Pin Configuration

#### Arduino Nano ESP32
```
LED Ring:     Pin 6
Buzzer:       Pin D2
Reset Button: Pin D7
Red Button:   Pin D3
Green Button: Pin D4
Blue Button:  Pin D5
Yellow Button: Pin A7
I2C (OLED):   SDA/SCL default pins
```

#### Seeed Xiao ESP32-C6
```
LED Ring:     Pin D10
Buzzer:       Pin D8
Reset Button: Pin D7
Red Button:   Pin D0
Green Button: Pin D1
Blue Button:  Pin D2
Yellow Button: Pin D3
I2C (OLED):   SDA/SCL default pins
```

## Wiring Guide

### Button Connections
Each button uses **digital GPIO pins with internal pullup resistors**:
- Connect one side of each button to its designated GPIO pin
- Connect the other side to **GND**
- No external resistors needed (uses `INPUT_PULLUP`)

### LED Ring
- VCC → 5V (or 3.3V for lower power)
- GND → GND
- Data In → LED_PIN

### OLED Display (I2C)
- VCC → 3.3V
- GND → GND
- SDA → SDA pin
- SCL → SCL pin

### Buzzer
- Positive → BUZZER_PIN
- Negative → GND

## Build Instructions

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- ESP32 board support

### Setup
1. Clone the repository
2. Open in PlatformIO IDE or use CLI
3. Select your target board in `platformio.ini`:
   - `arduino_nano_esp32` (default)
   - `seeed_xiao_esp32_c6`
4. Build and upload:
   ```bash
   pio run -t upload
   ```

### Configuration
All hardware settings are in `include/config.h`:
- Pin assignments
- LED count and type
- Timing parameters
- Display settings

## Gameplay

1. **Start**: Press any button to begin
2. **Watch**: Observe the sequence of colors and sounds
3. **Repeat**: Press buttons in the same order
4. **Progress**: Each round adds one more color to the sequence
5. **Challenge**: Reach the maximum sequence length for total victory!

### Controls
- **Any Button**: Start game / Input sequence
- **Reset Button**:
  - Short press: Restart system
  - Long press (7s): Reset high score (only in initial state)

## Power Consumption

### Battery Life Estimates
- **4×AA Alkaline (6V, 2500mAh)**: ~15-20 hours active gameplay
- **9V Battery (500mAh)**: ~3-4 hours active gameplay

### Power Optimization
- LEDs dim automatically when not in active use
- Display optimized for low power consumption
- Efficient state machine reduces CPU usage

## Development

### Architecture
- **Finite State Machine**: Clean state transitions using TinyFSM
- **Component-based**: Separate classes for LEDs, buttons, buzzer, display
- **Memory Optimized**: PROGMEM strings, efficient data structures
- **Modular Design**: Easy to extend and modify

### Key Classes
- `Game`: Main controller and FSM event handler
- `Buttons`: Digital button input with debouncing
- `Leds`: NeoPixel animations and effects
- `Buzzer`: Audio feedback and melodies
- `Board`: Hardware abstraction layer

### Dependencies
- Adafruit NeoPixel
- Adafruit SSD1306
- Adafruit GFX Library
- TinyFSM
- ESP32 Preferences (for persistent storage)

## Troubleshooting

### Common Issues
1. **Buttons not responding**: Check wiring to GND, verify pin definitions
2. **LEDs not lighting**: Verify power supply capacity, check data pin connection
3. **Display blank**: Check I2C connections and address (0x3C)
4. **No sound**: Verify buzzer polarity and pin connection
5. **High score not saving**: Check Preferences initialization in serial monitor

### Debug Features
- Serial output at 115200 baud
- State transition logging
- Button press/release events
- Setup progress indicators

### Power Issues
- Use adequate power supply for LED ring (up to 1.4A at full brightness)
- Consider external power for LEDs if using USB power
- Monitor battery voltage for optimal performance

## License

Open source project - feel free to modify and distribute.

## Contributing

Contributions welcome! Please test thoroughly on both supported board types.