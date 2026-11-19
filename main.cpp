#include "TicketMaxHeap.h"
#include "TicketStorage.h"

#include <iostream>
#include <sstream>
#include <string>

// Read a whole number within the allowed range.
bool readNumber(const std::string& prompt,
                long long minimum,
                long long maximum,
                long long& value) {
    std::string line;

    while (true) {
        std::cout << prompt;

        if (!std::getline(std::cin, line)) {
            return false;
        }

        std::istringstream input(line);
        long long number;
        char extra;

        if ((input >> number) &&
            !(input >> extra) &&
            number >= minimum &&
            number <= maximum) {
            value = number;
            return true;
        }

        std::cout << "Enter a whole number from "
                  << minimum << " to " << maximum << ".\n";
    }
}

void displayMenu() {
    std::cout << "\n=== Helpdesk Ticket Manager ===\n"
              << "Team: Tickets Titans\n"
              << "1. Add ticket\n"
              << "2. View next ticket\n"
              << "3. Search ticket by ID\n"
              << "4. View all tickets\n"
              << "5. Update ticket\n"
              << "6. Remove ticket\n"
              << "7. Save tickets\n"
              << "8. Load tickets\n"
              << "9. Exit\n";
}

int main() {
    while (true) {
        displayMenu();

        long long choice;

        if (!readNumber("Choose an option: ", 1, 9, choice)) {
            std::cout << "\nInput closed. Exiting.\n";
            return 0;
        }

        if (choice == 9) {
            std::cout << "Goodbye!\n";
            return 0;
        }

        std::cout << "This operation will be connected next.\n";
    }
}
