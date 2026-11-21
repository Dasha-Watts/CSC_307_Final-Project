#include "TicketMaxHeap.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <unordered_set>
#include <utility>


// TicketMaxHeap.cpp uses TicketMaxHeap.h to manage tickets using a Max Heap.
// the heap organizes tickets by priority and arrival sequence.
// helper functions confirms ticket information and maintain heap order.
// the higherPriority function compares tickets to determine which comes first.
     // High priority comes before Medium and Low priority.
     // Older tickets come first when priorities are equal.
// the findIndex function searches for a ticket using its ID.
// the siftUp function moves higher priority tickets toward the top.
// the siftDown function moves lower priority tickets toward the bottom.
// the insertTicket function confirms and adds new tickets to the heap.
     // assigns ticket IDs and arrival sequences.
     // uses siftUp to maintain the correct heap order.
// the findTicket function searches for and returns a ticket by ID.
// the peekNext function returns the highest priority ticket without removing it.
// the getAllTickets function returns all tickets currently stored in the heap.
     // tickets are returned in heap order, not necessarily sorted order.
// the updateTicket function changes an existing ticket's description or priority.
     // uses siftUp or siftDown to restore the heap order.
// the removeTicket function searches for and removes a ticket by ID.
     // replaces the removed ticket with the last ticket when necessary.
     // restores the heap order after removal.
// the buildHeap function creates or rebuilds the heap from existing tickets.
     // confirms ticket information and checks for duplicate IDs.
     // uses siftDown to organize tickets by priority.
// error handling prevents invalid ticket information from being added or updated.
// functions return success or failure results depending on the operation.
// the Max Heap ensures the highest priority ticket is handled first.



namespace {

// A ticket description must contain at least one non-whitespace character.
bool isBlank(const std::string& text) {
    return text.empty() ||
           std::all_of(text.begin(), text.end(), [](unsigned char ch) {
               return std::isspace(ch) != 0;
           });
}

bool isValidTicket(const Ticket& ticket) {
    return ticket.id > 0 &&
           ticket.arrivalSequence > 0 &&
           ticket.priority >= 1 && ticket.priority <= 3 &&
           !isBlank(ticket.description);
}

} // namespace

TicketMaxHeap::TicketMaxHeap()
    : nextId(1), nextArrival(1) {
}

// Higher priority numbers come first. For equal priorities, the ticket
// that arrived earlier (smaller arrivalSequence) comes first.
bool TicketMaxHeap::higherPriority(Ticket a, Ticket b) const {
    if (a.priority != b.priority) {
        return a.priority > b.priority;
    }
    return a.arrivalSequence < b.arrivalSequence;
}

int TicketMaxHeap::findIndex(long long id) const {
    for (std::size_t i = 0; i < heap.size(); ++i) {
        if (heap[i].id == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void TicketMaxHeap::siftUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (!higherPriority(heap[index], heap[parent])) {
            break;
        }
        std::swap(heap[index], heap[parent]);
        index = parent;
    }
}

void TicketMaxHeap::siftDown(int index) {
    const int count = static_cast<int>(heap.size());

    while (true) {
        int highest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < count && higherPriority(heap[left], heap[highest])) {
            highest = left;
        }
        if (right < count && higherPriority(heap[right], heap[highest])) {
            highest = right;
        }
        if (highest == index) {
            break;
        }

        std::swap(heap[index], heap[highest]);
        index = highest;
    }
}

long long TicketMaxHeap::insertTicket(std::string description, int priority) {
    if (isBlank(description) || priority < 1 || priority > 3 ||
        nextId <= 0 || nextArrival <= 0 ||
        nextId == std::numeric_limits<long long>::max() ||
        nextArrival == std::numeric_limits<long long>::max()) {
        return -1;
    }

    Ticket ticket{nextId, std::move(description), priority, nextArrival};
    heap.push_back(ticket);
    siftUp(static_cast<int>(heap.size()) - 1);

    ++nextId;
    ++nextArrival;
    return ticket.id;
}

std::optional<Ticket> TicketMaxHeap::findTicket(long long id) const {
    int index = findIndex(id);
    if (index == -1) {
        return std::nullopt;
    }
    return heap[index];
}

std::optional<Ticket> TicketMaxHeap::peekNext() const {
    if (heap.empty()) {
        return std::nullopt;
    }
    return heap.front();
}

std::vector<Ticket> TicketMaxHeap::getAllTickets() const {
    // This returns the heap's internal array order, not a sorted order.
    return heap;
}

bool TicketMaxHeap::updateTicket(long long id,
                                 std::string description,
                                 int priority) {
    if (isBlank(description) || priority < 1 || priority > 3) {
        return false;
    }

    int index = findIndex(id);
    if (index == -1) {
        return false;
    }

    heap[index].description = std::move(description);
    heap[index].priority = priority;

    // The priority may have increased or decreased, so restore the heap
    // property in the correct direction.
    if (index > 0 && higherPriority(heap[index], heap[(index - 1) / 2])) {
        siftUp(index);
    } else {
        siftDown(index);
    }
    return true;
}

bool TicketMaxHeap::removeTicket(long long id) {
    int index = findIndex(id);
    if (index == -1) {
        return false;
    }

    int lastIndex = static_cast<int>(heap.size()) - 1;
    if (index == lastIndex) {
        heap.pop_back();
        return true;
    }

    heap[index] = std::move(heap.back());
    heap.pop_back();

    if (index > 0 && higherPriority(heap[index], heap[(index - 1) / 2])) {
        siftUp(index);
    } else {
        siftDown(index);
    }
    return true;
}

bool TicketMaxHeap::buildHeap(std::vector<Ticket> records) {
    // Confirms the complete input before changing the current heap.
    std::unordered_set<long long> usedIds;
    long long largestId = 0;
    long long largestArrival = 0;

    for (const Ticket& ticket : records) {
        if (!isValidTicket(ticket) || !usedIds.insert(ticket.id).second) {
            return false;
        }
        largestId = std::max(largestId, ticket.id);
        largestArrival = std::max(largestArrival, ticket.arrivalSequence);
    }

    // Keep the existing heap unchanged if future generated values would
    // overflow. Empty input resets the heap and its generated counters.
    if (largestId == std::numeric_limits<long long>::max() ||
        largestArrival == std::numeric_limits<long long>::max()) {
        return false;
    }

    heap = std::move(records);

    // Bottom-up heap construction is O(n).
    for (int i = static_cast<int>(heap.size()) / 2 - 1; i >= 0; --i) {
        siftDown(i);
    }

    nextId = largestId + 1;
    nextArrival = largestArrival + 1;
    return true;
}
