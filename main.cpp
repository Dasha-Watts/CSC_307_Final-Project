#include "TicketMaxHeap.h"
#include "TicketStorage.h"

#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {

const std::string filename = "tickets.csv";
constexpr long long maxId = std::numeric_limits<long long>::max();

bool readNumber(const std::string& prompt, long long minimum,
                long long maximum, long long& value) {
    std::string line;

    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line)) {
            return false;
        }

        std::istringstream input(line);
        long long number;
        char extra;

        if ((input >> number) && !(input >> extra) &&
            number >= minimum && number <= maximum) {
            value = number;
            return true;
        }

        std::cout << "Enter a whole number from "
                  << minimum << " to " << maximum << ".\n";
    }
}

bool readDescription(const std::string& prompt, std::string& description) {
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, description)) {
            return false;
        }

        if (description.find_first_not_of(" \t\r\n\v\f") !=
            std::string::npos) {
            return true;
        }

        std::cout << "Description cannot be empty.\n";
    }
}

bool readPriority(long long& priority) {
    return readNumber("Priority (1=Low, 2=Medium, 3=High): ",
                      1, 3, priority);
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

void displayTicket(const Ticket& ticket) {
    const char* priorityName = "Unknown";

    switch (ticket.priority) {
        case 1: priorityName = "Low"; break;
        case 2: priorityName = "Medium"; break;
        case 3: priorityName = "High"; break;
    }

    std::cout << "\nTicket ID: " << ticket.id
              << "\nDescription: " << ticket.description
              << "\nPriority: " << priorityName
              << " (" << ticket.priority << ')'
              << "\nArrival sequence: " << ticket.arrivalSequence
              << '\n';
}

bool saveCurrentTickets(const TicketMaxHeap& heap) {
    std::string error;

    if (!TicketStorage::saveTickets(filename, heap.getAllTickets(), error)) {
        std::cout << "Could not save tickets: " << error << '\n';
        return false;
    }

    std::cout << "Tickets saved to " << filename << ".\n";
    return true;
}

int inputClosed(bool unsavedChanges) {
    std::cout << "\nInput closed. Exiting.\n";
    if (unsavedChanges) {
        std::cout << "Unsaved changes were not written to "
                  << filename << ".\n";
    }
    return 0;
}

}  // namespace

int main() {
    TicketMaxHeap heap;
    bool unsavedChanges = false;

    while (true) {
        displayMenu();

        long long choice;
        if (!readNumber("Choose an option: ", 1, 9, choice)) {
            return inputClosed(unsavedChanges);
        }

        switch (choice) {
            case 1: {
                std::string description;
                long long priority;

                if (!readDescription("Description: ", description) ||
                    !readPriority(priority)) {
                    return inputClosed(unsavedChanges);
                }

                const long long id =
                    heap.insertTicket(description, static_cast<int>(priority));

                if (id > 0) {
                    unsavedChanges = true;
                    std::cout << "Ticket added. ID: " << id << '\n';
                } else {
                    std::cout << "Could not add the ticket.\n";
                }
                break;
            }

            case 2: {
                const auto ticket = heap.peekNext();

                if (ticket) {
                    displayTicket(*ticket);
                } else {
                    std::cout << "No tickets are waiting.\n";
                }
                break;
            }

            case 3: {
                long long id;
                if (!readNumber("Ticket ID: ", 1, maxId, id)) {
                    return inputClosed(unsavedChanges);
                }

                const auto ticket = heap.findTicket(id);
                if (ticket) {
                    displayTicket(*ticket);
                } else {
                    std::cout << "Ticket not found.\n";
                }
                break;
            }

            case 4: {
                const auto tickets = heap.getAllTickets();

                if (tickets.empty()) {
                    std::cout << "No tickets are waiting.\n";
                } else {
                    std::cout << "Tickets in heap order:\n";
                    for (const auto& ticket : tickets) {
                        displayTicket(ticket);
                    }
                }
                break;
            }

            case 5: {
                long long id;
                if (!readNumber("Ticket ID: ", 1, maxId, id)) {
                    return inputClosed(unsavedChanges);
                }

                const auto ticket = heap.findTicket(id);
                if (!ticket) {
                    std::cout << "Ticket not found.\n";
                    break;
                }

                displayTicket(*ticket);

                long long updateChoice;
                if (!readNumber("Update 1=Description, 2=Priority, 3=Both: ",
                                1, 3, updateChoice)) {
                    return inputClosed(unsavedChanges);
                }

                std::string description = ticket->description;
                long long priority = ticket->priority;

                if ((updateChoice == 1 || updateChoice == 3) &&
                    !readDescription("New description: ", description)) {
                    return inputClosed(unsavedChanges);
                }
                if ((updateChoice == 2 || updateChoice == 3) &&
                    !readPriority(priority)) {
                    return inputClosed(unsavedChanges);
                }

                if (heap.updateTicket(id, description, static_cast<int>(priority))) {
                    unsavedChanges = true;
                    std::cout << "Ticket updated.\n";
                } else {
                    std::cout << "Could not update the ticket.\n";
                }
                break;
            }

            case 6: {
                long long id;
                if (!readNumber("Ticket ID to remove: ", 1, maxId, id)) {
                    return inputClosed(unsavedChanges);
                }

                const auto ticket = heap.findTicket(id);
                if (!ticket) {
                    std::cout << "Ticket not found.\n";
                    break;
                }

                displayTicket(*ticket);

                long long confirm;
                if (!readNumber("Remove this ticket? 1=Yes, 2=No: ",
                                1, 2, confirm)) {
                    return inputClosed(unsavedChanges);
                }

                if (confirm == 2) {
                    std::cout << "Removal canceled.\n";
                    break;
                }

                if (heap.removeTicket(id)) {
                    unsavedChanges = true;
                    std::cout << "Ticket removed.\n";
                } else {
                    std::cout << "Ticket not found.\n";
                }
                break;
            }

            case 7: {
                if (saveCurrentTickets(heap)) {
                    unsavedChanges = false;
                }
                break;
            }

            case 8: {
                if (unsavedChanges) {
                    long long confirm;
                    if (!readNumber(
                            "Replace unsaved tickets with saved tickets? "
                            "1=Yes, 2=No: ", 1, 2, confirm)) {
                        return inputClosed(unsavedChanges);
                    }

                    if (confirm == 2) {
                        std::cout << "Load canceled.\n";
                        break;
                    }
                }

                std::vector<Ticket> records;
                std::string error;

                if (!TicketStorage::loadTickets(filename, records, error)) {
                    std::cout << "Could not load tickets: " << error << '\n';
                    break;
                }

                if (!heap.buildHeap(records)) {
                    std::cout << "Could not load tickets: invalid ticket data.\n";
                    break;
                }

                unsavedChanges = false;
                std::cout << "Tickets loaded from " << filename << ".\n";
                break;
            }

            case 9: {
                if (unsavedChanges) {
                    long long exitChoice;
                    if (!readNumber(
                            "Unsaved changes: 1=Save and exit, "
                            "2=Exit without saving, 3=Cancel: ",
                            1, 3, exitChoice)) {
                        return inputClosed(unsavedChanges);
                    }

                    if (exitChoice == 3) {
                        break;
                    }
                    if (exitChoice == 1 && !saveCurrentTickets(heap)) {
                        break;
                    }
                }

                std::cout << "Goodbye!\n";
                return 0;
            }
        }
    }
}
