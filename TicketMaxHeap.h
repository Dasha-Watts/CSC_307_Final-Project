#ifndef TICKET_MAX_HEAP_H
#define TICKET_MAX_HEAP_H

#include "Ticket.h"
#include <optional>
#include <string>
#include <vector>

class TicketMaxHeap {
private:
    std::vector<Ticket> heap;
    long long nextId;
    long long nextArrival;

    bool higherPriority(Ticket a, Ticket b) const;
    int findIndex(long long id) const;
    void siftUp(int index);
    void siftDown(int index);

public:
    TicketMaxHeap();

    long long insertTicket(std::string description, int priority);

    std::optional<Ticket> findTicket(long long id) const;
    std::optional<Ticket> peekNext() const;
    std::vector<Ticket> getAllTickets() const;

    bool updateTicket(long long id,
                      std::string description,
                      int priority);

    bool removeTicket(long long id);
    bool buildHeap(std::vector<Ticket> records);
};

#endif
