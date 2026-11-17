#ifndef TICKET_H
#define TICKET_H

#include <string>

// Stores the information for one support ticket.
struct Ticket {
    long long id;
    std::string description;
    int priority;                // 3 = High, 2 = Medium, 1 = Low
    long long arrivalSequence;   // Earlier arrivals have smaller values
};

#endif