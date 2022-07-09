#include <iostream>
#include <vector>
#include <unordered_set>
using namespace std;

// Models
class Location {
private:
    int x;
    int y;
public:
    explicit Location() {}

    Location(int x, int y) {
        this->x = x;
        this->y = y;
    }

    int getX() {
        return x;
    }

    int getY() {
        return y;
    }
};

class Car {
private:
    int regNo;
    string type;
public:
    explicit Car() {}

    Car(int regNo, string type) {
        this->regNo = regNo;
        this->type = type;
    }

    int getRegNo() {
        return regNo;
    }

    string getType() {
        return type;
    }
};

class Ride {
private:
    int userId;
    int driverId;
    string date;
    Location *source;
    Location *destination;
    int cost;
public:
    explicit Ride() {}

    Ride(Location *source, Location *destination) {
        this->source = source;
        this->destination = destination;
    }

    void setUserId(int id) {
        this->userId = id;
    }

    void setDriverId(int id) {
        this->driverId = id;
    }

    int getUserId() {
        return userId;
    }

    int getDriverId() {
        return driverId;
    }

    void setCost(int cost) {
        this->cost = cost;
    }

    int getCost() {
        return cost;
    }
};

class Person {
private:
    int id;
    string name;
    vector<Ride*> rideHistory;
    Location *location;

public:
    explicit Person() {}

    Person (int id, string name) {
        this->id = id;
        this->name = name;
        rideHistory.clear();
    }

    void setId(int id) {
        this->id = id;
    }

    void setName(string name) {
        this->name = name;
    }

    void setLocation(Location *location) {
        this->location = location;
    }

    int getId() {
        return this->id;
    }

    string getName() {
        return this->name;
    }

    vector<Ride*> getRideHistory()
    {
        return this->rideHistory;
    }

    Location* getLocation() {
        return this->location;
    }

    void addToRideHistory(Ride *ride) {
        rideHistory.push_back(ride);
    }
};

class User : public Person {
private:
public:
    explicit User() { }
    User(int id, string name) : Person(id, name) {}
};

class Driver : public Person {
private:
    Car *car;
    bool available;
public:
    explicit Driver() {}

    Driver(int id, string name, Car* car) : Person(id, name)
    {
        this->car = car;
        this->available = true;
    }

    Car* getCar() {
        return car;
    }

    bool isAvailable() {
        return this->available;
    }

    void makeAvailable() {
        this->available = true;
    }

    void makeUnAvailable() {
        this->available = false;
    }

    void endRide() {
        makeAvailable();
    }
};

class PricingStrategy {
public:
    int basePrice;
    int firstTwoKms;
    int nextThreeKms;
    int remaining;
    explicit PricingStrategy() { }

    PricingStrategy(int basePrice, int firstTwoKms, int nextThreeKms, int remaining) {
        this->basePrice = basePrice;
        this->firstTwoKms = firstTwoKms;
        this->nextThreeKms = nextThreeKms;
        this->remaining = remaining;
    }
};

class RideHailingSystem {
private:
    unordered_set <Driver *> driverList;
    unordered_set <User *> userList;
    PricingStrategy *pricingStrategy;
    int radius;
public:
    RideHailingSystem(PricingStrategy *pricingStrategy, int radius) {
        this->pricingStrategy = pricingStrategy;
        this->radius = radius;
    }

    bool registerDriver(Driver *driver) {
        if(driverList.find(driver) != driverList.end()) {
            cout << "Driver Already Registered\n";
            return false;
        }
        driverList.insert(driver);
        cout << "Driver successfully registered\n";
        return true;
    }

    bool registerUser(User *user) {
        if(userList.find(user) != userList.end()) {
            cout << "User Already Registered\n";
            return false;
        }
        userList.insert(user);
        cout << "User successfully registered\n";
        return true;
    }

    Driver* findFirstAvailableDriver(int userX, int userY) {
        for (Driver *driver : driverList) {
            if(driver->isAvailable())
            {
                int distance = abs(userX - driver->getLocation()->getX()) + abs(userY - driver->getLocation()->getY());
                if (distance <= radius) {
                    return driver;
                }
            }
        }
        return NULL;
    }

    int findCost(Location *source, Location *destination) {
        int distance = abs(source->getX() - destination->getX()) + abs(source->getY() - destination->getY());
        int cost = 0;
        if(distance > 6) {
            cost += (pricingStrategy->remaining)*(distance - 6);
            distance = 6;
        }

        if(distance > 2) {
            cost += (pricingStrategy->nextThreeKms)*(distance - 2);
            distance = 2;
        }

        cost += (pricingStrategy->firstTwoKms)*(distance);
        return cost;
    }

    Ride* bookCab(User *user, Location *destination) {
        int distance = 0;
        Location *userLocation = user->getLocation();
        int userX = userLocation->getX(), userY = userLocation->getY();
        // Find the first available driver
        Driver* assignedDriver = findFirstAvailableDriver(userX, userY);
        if(assignedDriver != NULL) {
            Ride *ride = new Ride(userLocation, destination);
            ride->setUserId(user->getId());
            ride->setDriverId(assignedDriver->getId());
            int rideCost = pricingStrategy->basePrice + findCost(userLocation, destination);
            ride->setCost(rideCost);
            assignedDriver->makeUnAvailable();
            assignedDriver->addToRideHistory(ride);
            user->addToRideHistory(ride);
            return ride;
        }
        return NULL;
    }

    int getRadius() {
        return radius;
    }

    void completeRide(Driver *driver) {
        driver->makeAvailable();
    }
};

int main() {
    // Create a pricing strategy
    PricingStrategy *pricingStrategy = new PricingStrategy(50, 10, 8, 5);

    // Create a ride hailing system
    RideHailingSystem *uberApp = new RideHailingSystem(pricingStrategy, 3);

    // Add few cars
    Car *car1 = new Car(1000, "MINI");
    Car *car2 = new Car(2000, "MINI");
    Car *car3 = new Car(3000, "MINI");

    // Create few drivers
    Driver *driver1 = new Driver(1, "Shyam", car1);
    Driver *driver2 = new Driver(2, "Raju", car2);
    Driver *driver3 = new Driver(3, "Ajay", car3);

    //Create one user
    User *user1 = new User(4, "Amit");

    // Add drivers to uberApp
    uberApp->registerDriver(driver1);
    uberApp->registerDriver(driver2);
    uberApp->registerDriver(driver3);

    //Add user to uberApp
    uberApp->registerUser(user1);

    // Update driver locations
    driver1->setLocation(new Location(3,3));
    driver2->setLocation(new Location(4,4));
    driver3->setLocation(new Location(5,5));

    // Update user location
    user1->setLocation(new Location(0, 0));

    // Book cab for user1
    Ride *ride1 = uberApp->bookCab(user1, new Location(10, 10));

    if(ride1 != NULL) {
        cout << "Ride successfully booked" << endl;
        //cout << "Cost = " << ride1->getCost() << endl;
    }
    else
    {
        cout << "No available drivers within " << uberApp->getRadius() << endl;
    }

    uberApp->completeRide(driver1);

    user1->setLocation(new Location(5, 5));

    uberApp->bookCab(user1, new Location(5, 5));

    // List rides for user1
    vector<Ride*> list1 = user1->getRideHistory();
    for (Ride* rideHistory1 : list1) {
        cout << "user_id" << rideHistory1->getUserId() << rideHistory1->getDriverId() << endl; // We can print other informations
    }

    // List rides for driver3
    vector<Ride*> list2 = driver3->getRideHistory();
    for (Ride* rideHistory2 : list2) {
        cout << "driver_id" << rideHistory2->getDriverId() << " " << rideHistory2->getCost() << endl;
    }

    //List rides for driver2
    vector<Ride*> list3 = driver2->getRideHistory();
    for (Ride* rideHistory3 : list3) {
        cout << "driver_id" << rideHistory3->getDriverId()<< " " << rideHistory3->getCost() << endl;
    }


    return 0;
}
