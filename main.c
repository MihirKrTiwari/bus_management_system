#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 50
#define RATE_PER_KM 5.0  // Fare rate per kilometer

// --- Structures ---
struct Driver {
    char name[50];
    char licence_no[20];
    char phone[15];
    char bus_licence_no[20];
    float feedback;
};

struct Conductor {
    char name[50];
    char phone[15];
    char bus_licence_no[20];
    float feedback;
};

struct Bus {
    char licence_no[20];
    char stops[10][50];
    float distances[10];  // distance between stops[i] and stops[i+1]
    int stop_count;
    int seats;
};

struct Passenger {
    char name[50];
    char bus_licence_no[20];
    char start[50];
    char end[50];
    float distance;
    float fare;
};

// --- Function Declarations ---
void addDriver();
void addConductor();
void addBus();
void bookTicket();
void giveFeedback();
void viewDrivers();
void viewConductors();
void viewBuses();

// --- Main Menu ---
int main() {
    int choice;
    do {
        printf("\n===== BUS MANAGEMENT SYSTEM =====\n");
        printf("1. Add Driver\n");
        printf("2. Add Conductor\n");
        printf("3. Add Bus\n");
        printf("4. Book Passenger Ticket\n");
        printf("5. Give Feedback\n");
        printf("6. View Drivers\n");
        printf("7. View Conductors\n");
        printf("8. View Buses\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch(choice) {
            case 1: addDriver(); break;
            case 2: addConductor(); break;
            case 3: addBus(); break;
            case 4: bookTicket(); break;
            case 5: giveFeedback(); break;
            case 6: viewDrivers(); break;
            case 7: viewConductors(); break;
            case 8: viewBuses(); break;
            case 9: printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while(choice != 9);
    return 0;
}

// --- Function Definitions ---

void addDriver() {
    struct Driver d;
    FILE *fp = fopen("drivers.txt", "a");
    if(!fp) { printf("File error!\n"); return; }

    printf("Enter driver name: ");
    fgets(d.name, sizeof(d.name), stdin); d.name[strcspn(d.name, "\n")] = 0;
    printf("Enter driving licence number: ");
    fgets(d.licence_no, sizeof(d.licence_no), stdin); d.licence_no[strcspn(d.licence_no, "\n")] = 0;
    printf("Enter phone number: ");
    fgets(d.phone, sizeof(d.phone), stdin); d.phone[strcspn(d.phone, "\n")] = 0;
    printf("Enter bus licence number: ");
    fgets(d.bus_licence_no, sizeof(d.bus_licence_no), stdin); d.bus_licence_no[strcspn(d.bus_licence_no, "\n")] = 0;
    d.feedback = 0;

    fwrite(&d, sizeof(d), 1, fp);
    fclose(fp);
    printf("Driver added successfully!\n");
}

void addConductor() {
    struct Conductor c;
    FILE *fp = fopen("conductors.txt", "a");
    if(!fp) { printf("File error!\n"); return; }

    printf("Enter conductor name: ");
    fgets(c.name, sizeof(c.name), stdin); c.name[strcspn(c.name, "\n")] = 0;
    printf("Enter phone number: ");
    fgets(c.phone, sizeof(c.phone), stdin); c.phone[strcspn(c.phone, "\n")] = 0;
    printf("Enter bus licence number: ");
    fgets(c.bus_licence_no, sizeof(c.bus_licence_no), stdin); c.bus_licence_no[strcspn(c.bus_licence_no, "\n")] = 0;
    c.feedback = 0;

    fwrite(&c, sizeof(c), 1, fp);
    fclose(fp);
    printf("Conductor added successfully!\n");
}

void addBus() {
    struct Bus b;
    FILE *fp = fopen("buses.txt", "a");
    if(!fp) { printf("File error!\n"); return; }

    printf("Enter bus licence number: ");
    fgets(b.licence_no, sizeof(b.licence_no), stdin); b.licence_no[strcspn(b.licence_no, "\n")] = 0;

    printf("Enter number of stops: ");
    scanf("%d", &b.stop_count);
    getchar();

    for(int i=0; i<b.stop_count; i++) {
        printf("Enter stop %d name: ", i+1);
        fgets(b.stops[i], sizeof(b.stops[i]), stdin);
        b.stops[i][strcspn(b.stops[i], "\n")] = 0;
        if(i > 0) {
            printf("Enter distance from stop %d to stop %d (in km): ", i, i+1);
            scanf("%f", &b.distances[i-1]);
            getchar();
        }
    }

    printf("Enter number of seats: ");
    scanf("%d", &b.seats);
    getchar();

    fwrite(&b, sizeof(b), 1, fp);
    fclose(fp);
    printf("Bus added successfully with distance info!\n");
}

void bookTicket() {
    struct Passenger p;
    struct Bus b;
    FILE *fp, *temp;
    int foundBus = 0;
    int choice = 0;

    printf("Enter passenger name: ");
    fgets(p.name, sizeof(p.name), stdin);
    p.name[strcspn(p.name, "\n")] = 0;

    printf("Enter starting stop name: ");
    fgets(p.start, sizeof(p.start), stdin);
    p.start[strcspn(p.start, "\n")] = 0;

    printf("Enter ending stop name: ");
    fgets(p.end, sizeof(p.end), stdin);
    p.end[strcspn(p.end, "\n")] = 0;

    fp = fopen("buses.txt", "r");
    if (!fp) {
        printf("No bus data found!\n");
        return;
    }

    struct Bus buses[MAX];
    float journeyDistance[MAX];
    int busCount = 0;

    // --- Find buses that match route ---
    while (fread(&b, sizeof(b), 1, fp)) {
        int start_index = -1, end_index = -1;
        for (int i = 0; i < b.stop_count; i++) {
            if (strcmp(b.stops[i], p.start) == 0) start_index = i;
            if (strcmp(b.stops[i], p.end) == 0) end_index = i;
        }

        if (start_index != -1 && end_index != -1 && start_index < end_index && b.seats > 0) {
            float total_distance = 0;
            for (int i = start_index; i < end_index; i++) {
                total_distance += b.distances[i];
            }
            journeyDistance[busCount] = total_distance;
            buses[busCount++] = b;
            foundBus = 1;
        }
    }
    fclose(fp);

    if (!foundBus) {
        printf("No available buses found between %s and %s.\n", p.start, p.end);
        return;
    }

    printf("\nAvailable Buses between %s and %s:\n", p.start, p.end);
    printf("-----------------------------------------------------------------------------\n");
    for (int i = 0; i < busCount; i++) {
        printf("%d. Licence No: %s | Seats: %d | Distance: %.2f km\n",
               i + 1, buses[i].licence_no, buses[i].seats, journeyDistance[i]);

        printf("   Route: ");
        int start_index = -1, end_index = -1;
        for (int j = 0; j < buses[i].stop_count; j++) {
            if (strcmp(buses[i].stops[j], p.start) == 0) start_index = j;
            if (strcmp(buses[i].stops[j], p.end) == 0) end_index = j;
        }
        for (int j = start_index; j <= end_index; j++) {
            printf("%s", buses[i].stops[j]);
            if (j < end_index) printf(" -> ");
        }
        printf("\n-----------------------------------------------------------------------------\n");
    }

    printf("Enter bus number to book: ");
    scanf("%d", &choice);
    getchar();

    if (choice < 1 || choice > busCount) {
        printf("Invalid bus choice!\n");
        return;
    }

    struct Bus selectedBus = buses[choice - 1];
    float total_distance = journeyDistance[choice - 1];

    p.distance = total_distance;
    p.fare = total_distance * RATE_PER_KM;
    strcpy(p.bus_licence_no, selectedBus.licence_no);

    // --- Save passenger info ---
    FILE *passenger_fp = fopen("passengers.txt", "a");
    fwrite(&p, sizeof(p), 1, passenger_fp);
    fclose(passenger_fp);

    // --- Update seat count in bus file ---
    fp = fopen("buses.txt", "r");
    temp = fopen("temp.txt", "w");
    while (fread(&b, sizeof(b), 1, fp)) {
        if (strcmp(b.licence_no, selectedBus.licence_no) == 0) {
            b.seats -= 1; // reduce seat count
        }
        fwrite(&b, sizeof(b), 1, temp);
    }
    fclose(fp);
    fclose(temp);
    remove("buses.txt");
    rename("temp.txt", "buses.txt");

    // --- Print ticket ---
    printf("\n--- Passenger Bill ---\n");
    printf("Passenger Name : %s\n", p.name);
    printf("Bus Licence    : %s\n", p.bus_licence_no);
    printf("From           : %s\n", p.start);
    printf("To             : %s\n", p.end);
    printf("Distance       : %.2f km\n", p.distance);
    printf("Fare (₹%.2f/km): ₹%.2f\n", RATE_PER_KM, p.fare);
    printf("Seat booked successfully!\n");
    printf("-----------------------\n");
}

void giveFeedback() {
    char emp_type[20];
    char name[50];
    float rating;
    printf("Enter employee type (driver/conductor): ");
    fgets(emp_type, sizeof(emp_type), stdin); emp_type[strcspn(emp_type, "\n")] = 0;
    printf("Enter employee name: ");
    fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
    printf("Enter feedback rating (0-5): ");
    scanf("%f", &rating);
    getchar();

    if(strcmp(emp_type, "driver") == 0) {
        FILE *fp = fopen("drivers.txt", "r+");
        struct Driver d;
        while(fread(&d, sizeof(d), 1, fp)) {
            if(strcmp(d.name, name) == 0) {
                d.feedback = (d.feedback + rating) / 2;
                fseek(fp, -sizeof(d), SEEK_CUR);
                fwrite(&d, sizeof(d), 1, fp);
                printf("Feedback updated for driver %s!\n", name);
                fclose(fp);
                return;
            }
        }
        fclose(fp);
        printf("Driver not found!\n");
    } else if(strcmp(emp_type, "conductor") == 0) {
        FILE *fp = fopen("conductors.txt", "r+");
        struct Conductor c;
        while(fread(&c, sizeof(c), 1, fp)) {
            if(strcmp(c.name, name) == 0) {
                c.feedback = (c.feedback + rating) / 2;
                fseek(fp, -sizeof(c), SEEK_CUR);
                fwrite(&c, sizeof(c), 1, fp);
                printf("Feedback updated for conductor %s!\n", name);
                fclose(fp);
                return;
            }
        }
        fclose(fp);
        printf("Conductor not found!\n");
    } else {
        printf("Invalid employee type!\n");
    }
}

void viewDrivers() {
    FILE *fp = fopen("drivers.txt", "r");
    struct Driver d;
    printf("\n--- DRIVER DETAILS ---\n");
    while(fread(&d, sizeof(d), 1, fp))
        printf("Name: %s | Licence: %s | Phone: %s | Bus: %s | Feedback: %.2f\n", 
            d.name, d.licence_no, d.phone, d.bus_licence_no, d.feedback);
    fclose(fp);
}

void viewConductors() {
    FILE *fp = fopen("conductors.txt", "r");
    struct Conductor c;
    printf("\n--- CONDUCTOR DETAILS ---\n");
    while(fread(&c, sizeof(c), 1, fp))
        printf("Name: %s | Phone: %s | Bus: %s | Feedback: %.2f\n", 
            c.name, c.phone, c.bus_licence_no, c.feedback);
    fclose(fp);
}

void viewBuses() {
    FILE *fp = fopen("buses.txt", "r");
    struct Bus b;
    printf("\n--- BUS DETAILS ---\n");
    while(fread(&b, sizeof(b), 1, fp)) {
        printf("Bus Licence: %s | Seats: %d\nStops & Distances:\n", b.licence_no, b.seats);
        for(int i=0; i<b.stop_count; i++) {
            printf("%d. %s", i+1, b.stops[i]);
            if(i < b.stop_count-1) printf(" -> %.2f km -> ", b.distances[i]);
        }
        printf("\n-----------------------------\n");
    }
    fclose(fp);
}
