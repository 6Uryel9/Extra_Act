#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <utility>
//By: Marco Uriel Alfaro Hernandez 
// PART 5: Exception Safety & RAII
class DockingException : public std::runtime_error {
public:
    DockingException(const std::string& message) : std::runtime_error(message) {}
};

// PART 1 & 2: Base Components and Relationships
class Maneuver {
public:
    void execute() const { std::cout << ">>> Executing flight maneuver...\n"; }
};

class FuelTank {
public:
    int currentFuel = 100;
    FuelTank() { std::cout << "  -> FuelTank initialized\n"; }
    ~FuelTank() { std::cout << "  -> FuelTank destroyed\n"; }
};

class Telemetry {
public:
    Telemetry() { std::cout << "  -> Telemetry online\n"; }
    ~Telemetry() { std::cout << "  -> Telemetry offline\n"; }
};

class Entity {
public:
    Entity() { std::cout << "[Entity] Base instantiated\n"; }
    virtual ~Entity() { std::cout << "[Entity] Base destroyed\n"; }
};

// PART 2: Module Hierarchy 
class Module {
protected:
    std::string identifier;
    int energyCost;
public:
    Module(std::string id, int energy) : identifier(std::move(id)), energyCost(energy) {
        std::cout << "  [Module] " << identifier << " assembled\n";
    }
    virtual ~Module() { 
        std::cout << "  [Module] " << identifier << " dismantled\n"; 
    }
    virtual void activate() = 0;
};

class Engine final : public Module {
public:
    Engine(std::string id, int energy) : Module(std::move(id), energy) {
        std::cout << "    [Engine] specific components ready\n";
    }
    ~Engine() override { 
        std::cout << "    [Engine] specific components scrapped\n"; 
    }
    void activate() override { 
        std::cout << "*** ENGINE FIRING *** Consuming " << energyCost << " units.\n"; 
    }
};

class Shield final : public Module {
public:
    Shield(std::string id, int energy) : Module(std::move(id), energy) {}
    void activate() override { 
        std::cout << "*** SHIELD DEPLOYED *** Deflecting impacts.\n"; 
    }
};

// PART 3: Rule of 5 
class CargoHold {
private:
    int* items;
    size_t capacity;
public:
    CargoHold(size_t cap) : capacity(cap), items(new int[cap]) { 
        std::cout << "CargoHold: Raw memory allocated\n"; 
    }
    
    ~CargoHold() { 
        delete[] items; 
        std::cout << "CargoHold: Raw memory freed\n"; 
    }

    CargoHold(const CargoHold& src) : capacity(src.capacity), items(new int[src.capacity]) {
        for(size_t i = 0; i < capacity; ++i) items[i] = src.items[i];
    }

    CargoHold& operator=(const CargoHold& src) {
        if (this != &src) {
            delete[] items;
            capacity = src.capacity;
            items = new int[capacity];
            for(size_t i = 0; i < capacity; ++i) items[i] = src.items[i];
        }
        return *this;
    }

    CargoHold(CargoHold&& src) noexcept : items(src.items), capacity(src.capacity) {
        src.items = nullptr;
        src.capacity = 0;
    }

    CargoHold& operator=(CargoHold&& src) noexcept {
        if (this != &src) {
            delete[] items;
            items = src.items;
            capacity = src.capacity;
            src.items = nullptr;
            src.capacity = 0;
        }
        return *this;
    }

    bool checkEmpty() const { return items == nullptr; }
};

// PART 4: Rule of 0 
class ModernCargoHold {
private:
    std::vector<int> items; 
};

// Forward declaration
class Fleet;

class Spacecraft : public Entity {
private:
    FuelTank propellant;         
    Telemetry flightData;        
    Fleet* assignedFleet;        
    Module* hardwareSlot;       
    ModernCargoHold storage;    

public:
    Spacecraft() : assignedFleet(nullptr), hardwareSlot(nullptr) {
        std::cout << "[Spacecraft] Hull fully constructed\n";
    }
    
    ~Spacecraft() override {
        std::cout << "[Spacecraft] Hull breached and destroyed\n";
    }

    void joinFleet(Fleet* targetFleet) { assignedFleet = targetFleet; }
    void installHardware(Module* mod) { hardwareSlot = mod; }
    
    void runManeuver(Maneuver m) { 
        m.execute();
    }

    void processTick() {
        if (hardwareSlot) hardwareSlot->activate(); 
    }

    void dock() {
        if (propellant.currentFuel < 25) {
            throw DockingException("CRITICAL: Not enough fuel to safely dock!");
        }
        std::cout << "Docking sequence completed successfully.\n";
    }
    
    void consumeAllFuel() { propellant.currentFuel = 0; }
};

class Fleet {
private:
    std::vector<std::unique_ptr<Spacecraft>> vessels;
public:
    void registerShip(std::unique_ptr<Spacecraft> s) {
        s->joinFleet(this);
        vessels.push_back(std::move(s));
    }
    
    void scrapShip() {
        if (!vessels.empty()) {
            std::cout << "\n>>> Fleet Command: Scrapping one vessel...\n";
            vessels.pop_back(); // Safely triggers RAII destruction
        }
    }
    
    size_t countVessels() const { return vessels.size(); }
};

// MAIN SIMULATION
int main() {
    std::cout << "PART 3: RULE OF 5 TESTS\n";
    CargoHold originalHold(5);
    CargoHold copiedHold = originalHold;
    CargoHold movedHold = std::move(originalHold);
    
    std::cout << "Is the original hold safely nulled after move? " 
              << (originalHold.checkEmpty() ? "Yes" : "No") << "\n\n";

    std::cout << "PART 5: MISSION SIMULATION\n";
    Fleet republicFleet;
    Engine hyperDrive("HyperDrive_V2", 850);

    try {
        auto vanguardShip = std::make_unique<Spacecraft>();
        vanguardShip->installHardware(&hyperDrive);
        
        Maneuver standardOrbit;
        vanguardShip->runManeuver(standardOrbit);
        vanguardShip->processTick();
        
        vanguardShip->consumeAllFuel();
        vanguardShip->dock(); 

    } catch (const DockingException& error) {
        std::cerr << "EXCEPTION INTERCEPTED: " << error.what() << "\n";
    }

    std::cout << "\nPART 4: FLEET LIFECYCLE\n";
    republicFleet.registerShip(std::make_unique<Spacecraft>());
    republicFleet.registerShip(std::make_unique<Spacecraft>());
    
    std::cout << "Total active vessels: " << republicFleet.countVessels() << "\n";
    republicFleet.scrapShip();
    std::cout << "Remaining active vessels: " << republicFleet.countVessels() << "\n";
    
    std::cout << "\nTerminating simulation. Commencing full memory sweep...\n";
    return 0;
}