# Simon & Simon 🎮

A memory game implementation for ESP32-based microcontrollers with NeoPixel LEDs, OLED display, buttons, and buzzer. Challenge your memory by repeating increasingly complex sequences of colors and sounds!

## ✨ Features

### 🎯 Game Features
- **Classic Simon & Simon gameplay** with increasing difficulty
- **Sequence length protection** - Maximum 100 rounds prevents memory overflow
- **High score persistence** - Scores saved to flash memory
- **Visual celebrations** - Synchronized light shows with display fireworks for new records
- **Rainbow idle effects** - Periodic animations show system is active
- **Accurate scoring** - Precise round tracking

### 🔧 Hardware Features
- **Multi-board support** - Arduino Nano ESP32 and Seeed Xiao ESP32-C6
- **Smart reset button**:
  - Short press (< 7s): System restart
  - Long press (≥ 7s): High score reset (initial state only)
- **Auto-calibrated buttons** - Board-specific analog ranges for reliable detection
- **Memory optimized** - PROGMEM strings reduce RAM usage
- **Error recovery** - Graceful handling of hardware initialization failures

### 🎵 Audio/Visual Features
- **Smooth audio** - Fixed buzzer timing for clear melodies
- **Color-coded buttons** - Each button has unique color and sound
- **Animated displays** - Fireworks and celebrations on OLED screen
- **NeoPixel effects** - Rainbow animations, color wipes, sparkles

## 🛠 Hardware Requirements

### Supported Microcontrollers
- Arduino Nano ESP32
- Seeed Xiao ESP32-C6

### Components
- **24x WS2812B NeoPixel LED Strip** - Game sequence display
- **4x Analog Buttons** - Game input (connected via voltage divider)
- **Passive Buzzer** - Audio feedback
- **128x64 SSD1306 OLED Display** - Score and status display
- **Reset Button** - System control

### Connections

#### Arduino Nano ESP32
| Component | Pin | Notes |
|-----------|-----|-------|
| NeoPixel Data | D6 | 24 LEDs |
| Buzzer | D2 | Passive buzzer |
| Buttons | A7 | Analog input |
| Reset Button | D7 | Pullup to 3.3V |
| OLED SDA | SDA | I2C |
| OLED SCL | SCL | I2C |

#### Seeed Xiao ESP32-C6
| Component | Pin | Notes |
|-----------|-----|-------|
| NeoPixel Data | D10 | 24 LEDs |
| Buzzer | D8 | Passive buzzer |
| Buttons | A1 | Analog input |
| Reset Button | D7 | Pullup to 3.3V |
| OLED SDA | SDA | I2C |
| OLED SCL | SCL | I2C |

## 🚀 Quick Start

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- ESP32 development board
- Required components wired according to pinout

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/sephiroth74/Simon-Simon.git
   cd Simon-Simon
   ```

2. **Build and upload**
   ```bash
   cd platformio
   pio run -e arduino_nano_esp32 --target upload    # For Arduino Nano ESP32
   # OR
   pio run -e seeed_xiao_esp32_c6 --target upload   # For Seeed Xiao ESP32-C6
   ```

3. **Monitor serial output**
   ```bash
   pio device monitor
   ```

### Button Calibration

If buttons aren't working correctly, calibrate them for your hardware:

1. **Enable calibration mode**
   ```cpp
   // In platformio/include/config.h, uncomment:
   #define BUTTON_CALIBRATION_MODE
   ```

2. **Upload and monitor**
   ```bash
   pio run --target upload
   pio device monitor
   ```

3. **Press each button** and note the analog values displayed

4. **Update button ranges** in `config.h`:
   ```cpp
   #define YELLOW_BUTTON_MIN 1630
   #define YELLOW_BUTTON_MAX 1790
   // ... etc for other colors
   ```

5. **Disable calibration mode** and re-upload

## 🎮 How to Play

1. **Power on** - System initializes with welcome screen and rainbow animation
2. **Press any button** to start a new game
3. **Watch the sequence** - LEDs and sounds show the pattern to memorize
4. **Repeat the sequence** - Press buttons in the same order
5. **Success** - Sequence gets longer, continue to next round
6. **Mistake** - Game over, displays final score
7. **New high score** - Spectacular celebration with fireworks!

### Controls
- **Any game button** - Start game / Input sequence
- **Reset button (short press)** - Restart system
- **Reset button (long press)** - Clear high score (main menu only)

## 🏗 Architecture

### Software Architecture
The project uses a **finite state machine (FSM)** architecture powered by the `tinyfsm` library:

- **INITIAL_STATE** - Main menu, waiting for input
- **GAME_START_STATE** - Countdown and initialization
- **PLAYING_SEQUENCE_STATE** - Displaying sequence to memorize
- **PLAYING_USER_STATE** - Waiting for user input
- **PLAYING_WIN_STATE** - Round completed successfully
- **PLAYING_LOSE_STATE** - Game over, score handling

### Key Components
- **`game.cpp/h`** - Main game controller with celebration effects
- **`fsm.cpp/h`** - State machine implementation
- **`buttons.cpp/h`** - Multi-button analog input with debouncing
- **`leds.cpp/h`** - NeoPixel animations and effects
- **`buzzer.cpp/h`** - Audio feedback and melodies
- **`main.cpp`** - Entry point and reset button handling

## 🔧 Development

### Build Commands
```bash
cd platformio
pio run                           # Build default environment
pio run -e arduino_nano_esp32     # Build for Arduino Nano ESP32
pio run -e seeed_xiao_esp32_c6    # Build for Seeed Xiao ESP32-C6
pio run --target upload           # Upload to device
pio run --target monitor          # Serial monitor
```

### Code Formatting
Code is formatted using clang-format:
```bash
clang-format -i src/*.cpp include/*.h include/melodies/*.h
```

### Key Features
- **C++17 standard** with ESP32 Arduino framework
- **Memory optimized** - PROGMEM strings, efficient data structures
- **Non-blocking animations** - Responsive gameplay
- **Comprehensive error handling** - Graceful failure recovery
- **Extensive logging** - Serial debug output

## 🐛 Troubleshooting

### Button Issues
- **Not responding** - Run calibration mode to verify analog ranges
- **Wrong detection** - Check wiring and update ranges in `config.h`
- **Intermittent** - Verify pulldown resistors and debounce settings

### Audio Issues
- **No sound** - Check buzzer wiring and pin configuration
- **Choppy audio** - Ensure no blocking delays interrupt generation
- **Wrong pitch** - Verify passive (not active) buzzer type

### Display Issues
- **Blank screen** - Check I2C wiring (SDA/SCL) and address (0x3C)
- **Garbled text** - Verify 128x64 resolution and SSD1306 compatibility
- **Init failed** - System continues without display functionality

### Reset Button
- **Not working** - Check D7 connection and pullup configuration
- **Long press ignored** - Must be in initial state (main menu)
- **Accidental resets** - Long press safety only works in initial state

## 📚 Dependencies

- **Adafruit NeoPixel** - LED strip control
- **Adafruit SSD1306** - OLED display driver
- **Adafruit GFX Library** - Graphics primitives
- **tinyfsm** - Lightweight state machine framework

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🎯 Future Enhancements

- [ ] Multiple difficulty levels
- [ ] Sound themes and customization
- [ ] Bluetooth connectivity for remote play
- [ ] Tournament mode with multiple players
- [ ] Web interface for configuration
- [ ] Additional LED patterns and effects

## 🏆 Acknowledgments

- Inspired by the classic Simon electronic game by Milton Bradley
- Built with love for retro gaming and modern maker culture
- Thanks to the Arduino and ESP32 communities for excellent libraries and support

---

**Have fun playing Simon!** 🎉
