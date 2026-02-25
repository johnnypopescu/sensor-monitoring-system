# Sensor Monitoring System

Simulare in C++17 a unui sistem cu mai multi senzori. Genereaza citiri cu zgomot aleator si detecteaza cand valorile ies din limite.

## Senzorii simulati

| Senzor | Valoare normala | Alerta daca |
|---|---|---|
| MCU temperature | 70 C | < 20 sau > 95 |
| Power stage temp | 85 C | < 25 sau > 110 |
| Coolant pressure | 1013 hPa | < 950 sau > 1080 |
| VCC 3.3V | 3.3 V | < 3.0 sau > 3.6 |

Programul ruleaza 50 timesteps, logheaza fiecare citire si raporteaza alertele.

## Design OOP

- `Sensor` clasa abstracta cu `read()` si `getUnit()`
- `TemperatureSensor`, `PressureSensor`, `VoltageMonitor` derivate
- `DataLogger` tine istoricul citirilor
- `AlertSystem` tine evidenta defectelor
- `SensorNetwork` contine totul, ruleaza simularea

## Build & run

```
mkdir build
cd build
cmake ..
cmake --build .
./SensorMonitoring
```

Sau direct in CLion.

## Output

![Output](screenshots/sensor_output.png)

Statistici dupa 50 timesteps si lista alertelor detectate.
