# Sensor Monitoring & Fault Detection System

Embedded-style simulation of a sensor network on a control board. Reads multiple sensors (temperature, pressure, voltage), logs the data, and triggers fault alerts when readings exceed safety thresholds.

Built in **C++17** to practice core OOP concepts: abstraction, polymorphism, encapsulation and composition.

## What it simulates

A control board (think automotive inverter or industrial controller) with 4 sensors:

| Sensor | Normal | Faulty if |
|---|---|---|
| MCU temperature | 70 °C | < 20 or > 95 |
| Power stage temperature | 85 °C | < 25 or > 110 |
| Coolant pressure | 1013 hPa | < 950 or > 1080 |
| VCC 3.3V rail | 3.3 V | < 3.0 or > 3.6 |

Each sensor produces a realistic reading (base value + random noise). The system runs for 50 timesteps, logs everything, and reports any fault detected during the run.

## OOP concepts demonstrated

- **Abstraction** — `Sensor` is an abstract base class with pure virtual `read()` and `getUnit()`
- **Polymorphism** — `SensorNetwork` holds `Sensor*` pointers and calls `read()` uniformly across derived types
- **Inheritance** — `TemperatureSensor`, `PressureSensor`, `VoltageMonitor` extend `Sensor`
- **Encapsulation** — `DataLogger` and `AlertSystem` hide internal state behind methods
- **Composition** — `SensorNetwork` contains `DataLogger`s and an `AlertSystem` (has-a, not is-a)

## Build & run

```bash
mkdir build && cd build
cmake ..
cmake --build .
./SensorMonitoring
```

Or open in CLion — it reads `CMakeLists.txt` automatically.

## Example output

```
=== SENSOR NETWORK SIMULATION ===
Run 50 timesteps...

  [ALERT] Step   7 | MCU_Temp             | 96.84    | OVER MAX (95C)
  [ALERT] Step  19 | CoolantPressure      | 947.12   | UNDER MIN (950hPa)
  [ALERT] Step  34 | PowerStage           | 111.20   | OVER MAX (110C)

=== STATISTICI ===
Senzor                Citiri    Medie     Min       Max       Limite
------------------------------------------------------------------------
MCU_Temp              50        70.34     62.11     97.23     [20 - 95 C]
PowerStage            50        85.12     76.45     111.20    [25 - 110 C]
CoolantPressure       50        1013.45   947.12    1078.90   [950 - 1080 hPa]
VCC_3V3               50        3.30      3.05      3.58      [3 - 3.6 V]

Total defecte detectate: 3
```
