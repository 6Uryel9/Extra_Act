# Extra Activity

## What is this activity?
This is an extra activity for Object-Oriented Programming. The goal of this program is to demonstrate core C++ concepts, including memory management, object lifecycles, and relationships (Inheritance, Composition, Aggregation, and Dependency), ensuring that memory is managed perfectly and nothing leaks when an object is destroyed.

## How to build and run
To compile the code using C++14 and check for all warnings, open the terminal in the same folder as the code and run exactly this:

`g++ -std=c++14 main.cpp -o activity`

To run the program, execute the following command:
* **Windows:** `.\activity.exe`
* **Mac/Linux:** `./activity`

## Part 1: Design & Relationship Matrix
The class relationships were designed following strict C++ lifecycles:

* **Inheritance (IS-A):** `class Spacecraft : public Entity`. A spacecraft is a specific type of trackable entity.
* **Composition (HAS-A):** `FuelTank` and `Telemetry` are embedded directly as values inside the `Spacecraft` class. They are strictly owned by the ship and die exactly when the ship is destroyed.
* **Aggregation (HAS-A but not owned):** The `Spacecraft` has a non-owning raw pointer to the `Fleet` (`Fleet* assignedFleet`) and a `Module* hardwareSlot`. Pointers are used here because the fleet and the modules live independently of the ship.
* **Dependency (USES-A):** The `Maneuver` object is only passed as a parameter to the `runManeuver()` method. The ship just uses it for that specific action and doesn't store it.

## Part 2: Construction Order, Destruction & Slicing

**What is the exact print order?**
When the program creates a `Spacecraft`, it builds the object from the inside out. The base class is constructed first, then the member variables, and finally the derived class. Destruction happens in the exact reverse order. 
1. `[Entity] Base instantiated`
2. `-> FuelTank initialized`
3. `-> Telemetry online`
4. `CargoHold: Raw memory allocated`
5. `[Spacecraft] Hull fully constructed`

**Object Slicing:**
Slicing happens if a derived object (like an `Engine`) is saved directly inside a base class variable (`Module`). It "slices" off the engine parts. To avoid this, objects are exclusively stored and passed using pointers (`Module*` and `std::unique_ptr`). 
Also, if the `Module` class didn't have a `virtual` destructor, deleting a `Module*` would only destroy the base part and leak the `Engine` part. The virtual destructor prevents this memory leak.

## Part 4: The Rule of Zero & Ownership

**Why ModernCargoHold has zero special members:**
The old cargo hold was updated to `ModernCargoHold` using a `std::vector<int>`. Because the standard vector already knows how to allocate, copy, move, and free its own memory, there is no need to write any custom destructors or copy constructors. The compiler handles it safely for free, which is known as the Rule of Zero.

**Ownership Strategy:**
The `Fleet` class holds the objects using a `std::vector<std::unique_ptr<Spacecraft>>`. 
* **Why unique_ptr instead of raw pointers?** It provides automatic RAII. If an object gets destroyed (popped from the vector), the `unique_ptr` automatically deletes it and frees the RAM. No manual `delete` is needed, so there are no leaks.
* **Why unique_ptr instead of shared_ptr?** A `Spacecraft` in this design is strictly owned by one `Fleet` at a time. A `shared_ptr` adds extra processing overhead (reference counting) that isn't needed since the ownership isn't being shared with other systems.
