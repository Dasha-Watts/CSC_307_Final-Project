#include "TicketMaxHeap.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <unordered_set>
#include <utility>

namespace {

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

// Higher priority comes first. For equal priorities, the earlier
// arrival comes first.
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
    // Return the heap's internal array order.
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
    // Validate every record before changing the current heap.
    std::unordered_set<long long> usedIds;
    std::unordered_set<long long> usedArrivals;
    long long largestId = 0;
    long long largestArrival = 0;

    for (const Ticket& ticket : records) {
        if (!isValidTicket(ticket) ||
            !usedIds.insert(ticket.id).second ||
            !usedArrivals.insert(ticket.arrivalSequence).second) {
            return false;
        }

        largestId = std::max(largestId, ticket.id);
        largestArrival = std::max(largestArrival, ticket.arrivalSequence);
    }

    if (largestId == std::numeric_limits<long long>::max() ||
        largestArrival == std::numeric_limits<long long>::max()) {
        return false;
    }

    heap = std::move(records);

    // Restore heap order from the last parent up to the root.
    for (int i = static_cast<int>(heap.size()) / 2 - 1; i >= 0; --i) {
        siftDown(i);
    }

    nextId = largestId + 1;
    nextArrival = largestArrival + 1;
    return true;
}
