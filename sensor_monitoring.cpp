// Sensor Monitoring System - proiect OOP
// simulez o placa cu mai multi senzori si detectez defecte

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <algorithm>

// clasa abstracta pt orice senzor
class Sensor {
protected:
    std::string name;
    double currentValue;
    double minThreshold;
    double maxThreshold;

public:
    Sensor(const std::string& sensorName, double minVal, double maxVal)
        : name(sensorName), currentValue(0.0),
          minThreshold(minVal), maxThreshold(maxVal) {}

    virtual ~Sensor() = default;

    // fiecare tip de senzor simuleaza citirea altfel
    virtual double read() = 0;
    virtual std::string getUnit() const = 0;

    bool isFaulty() const {
        return (currentValue < minThreshold || currentValue > maxThreshold);
    }

    std::string getName() const { return name; }
    double getValue() const { return currentValue; }
    double getMin() const { return minThreshold; }
    double getMax() const { return maxThreshold; }
};

// senzor temp - ex NTC pe un MCU
class TemperatureSensor : public Sensor {
private:
    double baseTemp;

public:
    TemperatureSensor(const std::string& name, double base, double minC, double maxC)
        : Sensor(name, minC, maxC), baseTemp(base) {}

    double read() override {
        // temp de baza + zgomot random ca in realitate
        double noise = ((std::rand() % 201) - 100) / 100.0 * 5.0;
        currentValue = baseTemp + noise;
        return currentValue;
    }

    std::string getUnit() const override { return "C"; }
};

class PressureSensor : public Sensor {
private:
    double basePressure;

public:
    PressureSensor(const std::string& name, double base, double minP, double maxP)
        : Sensor(name, minP, maxP), basePressure(base) {}

    double read() override {
        double noise = ((std::rand() % 201) - 100) / 100.0 * 15.0;
        currentValue = basePressure + noise;
        return currentValue;
    }

    std::string getUnit() const override { return "hPa"; }
};

class VoltageMonitor : public Sensor {
private:
    double nominalVoltage;

public:
    VoltageMonitor(const std::string& name, double nominal, double minV, double maxV)
        : Sensor(name, minV, maxV), nominalVoltage(nominal) {}

    double read() override {
        // tensiunea variaza mult mai putin decat temp
        double noise = ((std::rand() % 201) - 100) / 100.0 * 0.3;
        currentValue = nominalVoltage + noise;
        return currentValue;
    }

    std::string getUnit() const override { return "V"; }
};

// tine istoricul citirilor pt un senzor
class DataLogger {
private:
    std::string sensorName;
    std::vector<double> history;
    int maxEntries;

public:
    DataLogger(const std::string& name, int maxSize = 100)
        : sensorName(name), maxEntries(maxSize) {}

    void log(double value) {
        if ((int)history.size() >= maxEntries) {
            history.erase(history.begin());
        }
        history.push_back(value);
    }

    double getAverage() const {
        if (history.empty()) return 0.0;
        double sum = 0.0;
        for (double v : history) sum += v;
        return sum / history.size();
    }

    double getMax() const {
        if (history.empty()) return 0.0;
        return *std::max_element(history.begin(), history.end());
    }

    double getMin() const {
        if (history.empty()) return 0.0;
        return *std::min_element(history.begin(), history.end());
    }

    int getCount() const { return (int)history.size(); }
    std::string getSensorName() const { return sensorName; }
};

class AlertSystem {
private:
    struct Alert {
        std::string sensorName;
        double value;
        std::string message;
        int timestep;
    };

    std::vector<Alert> alertLog;
    int totalAlerts;

public:
    AlertSystem() : totalAlerts(0) {}

    void trigger(const std::string& sensor, double value, const std::string& msg, int step) {
        alertLog.push_back({sensor, value, msg, step});
        totalAlerts++;
        std::cout << "  [ALERT] Step " << std::setw(3) << step
                  << " | " << std::left << std::setw(20) << sensor
                  << " | " << std::fixed << std::setprecision(2)
                  << std::setw(8) << value
                  << " | " << msg << "\n";
    }

    int getTotalAlerts() const { return totalAlerts; }

    void printSummary() const {
        std::cout << "\n=== ALERT SUMMARY ===\n";
        std::cout << "Total alerts: " << totalAlerts << "\n";
        if (alertLog.empty()) {
            std::cout << "  Niciun defect detectat.\n";
            return;
        }
        for (const auto& a : alertLog) {
            std::cout << "  Step " << std::setw(3) << a.timestep
                      << " | " << std::left << std::setw(20) << a.sensorName
                      << " | " << std::fixed << std::setprecision(2)
                      << a.value << " | " << a.message << "\n";
        }
    }
};

// reteaua de senzori - contine senzori + loggere + sistem alerte
class SensorNetwork {
private:
    std::vector<Sensor*> sensors;
    std::vector<DataLogger> loggers;
    AlertSystem alertSystem;
    int currentStep;

public:
    SensorNetwork() : currentStep(0) {}

    ~SensorNetwork() {
        for (Sensor* s : sensors) delete s;
    }

    void addSensor(Sensor* sensor) {
        sensors.push_back(sensor);
        loggers.emplace_back(sensor->getName());
    }

    void tick() {
        currentStep++;
        for (size_t i = 0; i < sensors.size(); i++) {
            double val = sensors[i]->read();
            loggers[i].log(val);

            if (sensors[i]->isFaulty()) {
                std::string direction = (val > sensors[i]->getMax())
                    ? "OVER MAX (" + std::to_string((int)sensors[i]->getMax()) + sensors[i]->getUnit() + ")"
                    : "UNDER MIN (" + std::to_string((int)sensors[i]->getMin()) + sensors[i]->getUnit() + ")";
                alertSystem.trigger(sensors[i]->getName(), val, direction, currentStep);
            }
        }
    }

    void run(int steps) {
        std::cout << "=== SENSOR NETWORK SIMULATION ===\n";
        std::cout << "Run " << steps << " timesteps...\n\n";

        for (int i = 0; i < steps; i++) {
            tick();
        }
    }

    void printStatistics() const {
        std::cout << "\n=== STATISTICI ===\n";
        std::cout << std::left
                  << std::setw(22) << "Senzor"
                  << std::setw(10) << "Citiri"
                  << std::setw(10) << "Medie"
                  << std::setw(10) << "Min"
                  << std::setw(10) << "Max"
                  << "Limite\n";
        std::cout << std::string(72, '-') << "\n";

        for (size_t i = 0; i < sensors.size(); i++) {
            const DataLogger& log = loggers[i];
            const Sensor* s = sensors[i];
            std::cout << std::left
                      << std::setw(22) << s->getName()
                      << std::setw(10) << log.getCount()
                      << std::setw(10) << std::fixed << std::setprecision(2) << log.getAverage()
                      << std::setw(10) << log.getMin()
                      << std::setw(10) << log.getMax()
                      << "[" << s->getMin() << " - " << s->getMax()
                      << " " << s->getUnit() << "]\n";
        }
    }

    void printAlertSummary() const {
        alertSystem.printSummary();
    }

    int getAlertCount() const {
        return alertSystem.getTotalAlerts();
    }
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    SensorNetwork network;

    // adaug senzorii - placa de control invertor
    network.addSensor(new TemperatureSensor("MCU_Temp",     70.0,  20.0, 95.0));
    network.addSensor(new TemperatureSensor("PowerStage",   85.0,  25.0, 110.0));
    network.addSensor(new PressureSensor("CoolantPressure", 1013.0, 950.0, 1080.0));
    network.addSensor(new VoltageMonitor("VCC_3V3",         3.3,   3.0,  3.6));

    // ruleaza 50 de pasi (ex: 50 esantioane la 100ms = 5s)
    network.run(50);

    network.printStatistics();
    network.printAlertSummary();

    std::cout << "\nTotal defecte detectate: " << network.getAlertCount() << "\n";

    return 0;
}
