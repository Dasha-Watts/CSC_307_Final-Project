#ifndef TICKET_STORAGE_H
#define TICKET_STORAGE_H

#include "Ticket.h"
#include <string>
#include <vector>

namespace TicketStorage {
    bool saveTickets(const std::string& filename,
                     const std::vector<Ticket>& tickets,
                     std::string& error);

    bool loadTickets(const std::string& filename,
                     std::vector<Ticket>& records,
                     std::string& error);
}

#endif
