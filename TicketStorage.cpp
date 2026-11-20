#include "TicketStorage.h"
#include <fstream>
#include <iterator>
#include <limits>
#include <stdexcept>

// For review, I wanted to make sure that I hit all the points that were mentioned for the cpp file. I wasnt sure if you wanted me to make changes or set any of the helpers to the h file as well 

// TicketStorage.cpp uses the TicketStorage.h file for its saverTickets and loadTickets functions.
// it has helper functions to parse CSV rows, handle quoted fields, and validate ticket data.
    // error processing was implamented to try and prevent any invalid or null data from continuing.
// the file saves and loads tickets both to and from CVS files via the h file.  
    // The saveTickets function writes tickets to a CSV file, ensuring that existing tickets remain untouched if an error occurs. 
        // function identifies discription, priority, id, and arrival sequence.
        // function opens a CSV file. 
        // function writes to the file.
        // function closes the file.
        // error handling was implamented throughout the data verification and file operations. 
    // the loadTickets function reads tickets from a CSV file, validating the data and appending valid tickets to the provided records vector.
        // function opens a CSV file.
        // function reads the file content.
        // function identifiies if there is a data match.
        // function stores data in a temp vector to prevent overwrite on error.
        // error handling was implamented throughout the data verification and file operations.

// I tried to make sure there was good error handling for the file and feel like i hit as much as i could, sorry if i went overboard with it.

namespace { 

// helper function to escape CSV fields
std::string csvField(const std::string& value) {
    bool needsQuotes = value.find_first_of(",\"\r\n") != std::string::npos;
    if (!needsQuotes) {
        return value; 
    }

    // Escape double quotes & wrap the field in quotes
    std::string quoted;
    quoted.reserve(value.size() + 2);
    quoted.push_back('"');
    for (char character : value) {
        if (character == '"') {
            quoted.push_back('"');
        }
        quoted.push_back(character);
    }
    quoted.push_back('"');
    return quoted; 
}

// Parse CSV row into fields & handling quoted fields and commas
bool parseCsvRow(const std::string& line,
                 std::vector<std::string>& fields,
                 std::string& error) {
    fields.clear();
    std::string field;
    bool inQuotes = false;
    
    // iteration through each character in the line
    for (std::size_t i = 0; i < line.size(); ++i) {
        char character = line[i];
        if (inQuotes) {
            if (character == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field.push_back('"');
                    ++i;
                } else {
                    if (i + 1 < line.size() && line[i + 1] != ',') {
                        error = "Invalid CSV quoting"; 
                        return false; 
                    }
                    inQuotes = false; 
                }
            } else {
                field.push_back(character);
            }
        } else if (character == '"') {
            if (!field.empty()) {
                error = "Invalid CSV quoting"; 
                return false; 
            }
            inQuotes = true; 
        } else if (character == ',') {
            fields.push_back(field);
            field.clear();
        } else {
            field.push_back(character);
        }
    }

    if (inQuotes) {
        error = "Invalid CSV quoting";
        return false; 
    }

    fields.push_back(field);
    return true; 
}

// parse string into a long long integer & returning false on failure
bool parseLongLong(const std::string& text, long long& value) {
    if (text.empty()) {
        return false; 
    }

    try {
        std::size_t consumed = 0;
        value = std::stoll(text, &consumed);
        return consumed == text.size();
    } catch (const std::exception&) {
        return false; 
    }
}

// parse string into an integer & returning false on failure
bool parseInt(const std::string& text, int& value) {
    if (text.empty()) {
        return false; 
    }

    try {
        std::size_t consumed = 0;
        long long parsed = std::stoll(text, &consumed);
        if (consumed != text.size() ||
            parsed < std::numeric_limits<int>::min() ||
            parsed > std::numeric_limits<int>::max()) {
            return false; 
        }
        value = static_cast<int>(parsed);
        return true; 
    } catch (const std::exception&) {
        return false; 
    }
}

// check the line is a valid CSV header
bool isHeader(const std::string& line) {
    std::vector<std::string> fields;
    std::string error;
    return parseCsvRow(line, fields, error) && 
           fields.size() == 4 &&
           fields[0] == "ID" &&
           fields[1] == "Description" &&
           fields[2] == "Priority" &&
           fields[3] == "ArrivalSequence";
}

// Check if a vector matches the expected CSV header
bool isHeaderFields(const std::vector<std::string>& fields) {
    return fields.size() == 4 && // Check if the fields match the expected header
           fields[0] == "ID" &&
           fields[1] == "Description" &&
           fields[2] == "Priority" &&
           fields[3] == "ArrivalSequence";
}

// parse content into rows, handles quoted fields & line breaks
bool parseCsvRows(const std::string& content,
                  std::vector<std::pair<std::string, std::vector<std::string>>>& rows,
                  std::string& error) {
    rows.clear();
    std::string field;
    std::vector<std::string> fields;
    bool inQuotes = false;
    int lineNumber = 1;
    int rowLineNumber = 1;

    for (std::size_t i = 0; i < content.size(); ++i) {
        char character = content[i];

        if (inQuotes) {
            if (character == '"') {
                if (i + 1 < content.size() && content[i + 1] == '"') {
                    field.push_back('"');
                    ++i;
                } else {
                    inQuotes = false; 
                    if (i + 1 < content.size() &&
                        content[i + 1] != ',' &&
                        content[i + 1] != '\r' &&
                        content[i + 1] != '\n') {
                        error = "Invalid CSV quoting at line " +
                                std::to_string(lineNumber);
                        return false; 
                    }
                }
            } else if (character == '\r') {
                continue;
            } else if (character == '\n') {
                field.push_back('\n');
                ++lineNumber;
            } else {
                field.push_back(character);
            }
            continue; 
        }

        if (character == '"') {
            if (!field.empty()) {
                error = "Invalid CSV quoting at line " +
                        std::to_string(lineNumber);
                return false; 
            }
            inQuotes = true;
        } else if (character == ',') {
            fields.push_back(field);
            field.clear();
        } else if (character == '\r') {
            continue;
        } else if (character == '\n') {
            fields.push_back(field);
            field.clear();
            rows.emplace_back(std::to_string(rowLineNumber), fields);
            fields.clear();
            ++lineNumber;
            rowLineNumber = lineNumber;
        } else {
            field.push_back(character);
        }
    }

    if (inQuotes) {
        error = "Invalid CSV quoting at line " + std::to_string(rowLineNumber);
        return false; 
    }

    if (!field.empty() || !fields.empty()) {
        fields.push_back(field);
        rows.emplace_back(std::to_string(rowLineNumber), fields);
    }
    return true; 
}

} 

namespace TicketStorage { 
    
    // Save tickets to a CSV file & ensure existing tickets are untouched if error pops
    bool saveTickets(const std::string& filename,
                     const std::vector<Ticket>& tickets,
                     std::string& error) {
        for (const Ticket& ticket : tickets) {
            if (ticket.description.empty()) {
                error = "Cannot save ticket: description is empty";
                return false; 
            }
            if (ticket.priority < 1 || ticket.priority > 3) {
                error = "Cannot save ticket: invalid priority " +
                        std::to_string(ticket.priority);
                return false; 
            }
            if (ticket.id < 0) {
                error = "Cannot save ticket: invalid ID " +
                        std::to_string(ticket.id);
                return false; 
            }
            if (ticket.arrivalSequence < 0) {
                error = "Cannot save ticket: invalid arrival sequence " +
                        std::to_string(ticket.arrivalSequence);
                return false; 
            }
        }
        
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            error = "Failed to open file for writing: " + filename;
            return false; 
        }

        outFile << "ID,Description,Priority,ArrivalSequence\n";

        for (const auto& ticket : tickets) {
            outFile << ticket.id << ","
                    << csvField(ticket.description) << ","
                    << ticket.priority << ","
                    << ticket.arrivalSequence << "\n";
        }
        
        if (outFile.fail()) {
            error = "Error occurred while writing to file: " + filename;
            return false; 
        }
        
        outFile.close();
        if (outFile.fail()) {
            error = "Error occurred while closing file: " + filename;
            return false; 
        }
        return true; 
    }
    
    // Load tickets from a CSV file, ensuring that existing tickets remain untouched if an error occurs
    bool loadTickets(const std::string& filename,
                     std::vector<Ticket>& records,
                     std::string& error) {
        std::ifstream inFile(filename);
        if (!inFile.is_open()) {
            error = "Missing or unreadable file: " + filename; 
            return false; 
        }
        
        std::string content((std::istreambuf_iterator<char>(inFile)),
                            std::istreambuf_iterator<char>());
        if (!inFile.eof() && inFile.fail()) {
            error = "Error occurred while reading file: " + filename;
            return false; 
        }
        if (content.empty()) {
            error = "Invalid ticket data: file is empty: " + filename;
            return false; 
        }

        std::vector<std::pair<std::string, std::vector<std::string>>> rows;
        std::string csvError;
        if (!parseCsvRows(content, rows, csvError)) {
            error = csvError;
            return false; 
        }

        std::vector<Ticket> tempRecords;
        bool firstRow = true;

        for (const auto& row : rows) {
            const std::string& lineNumber = row.first;
            const std::vector<std::string>& fields = row.second;
            
            if (fields.size() == 1 && fields[0].empty()) {
                continue;
            }

            if (firstRow) {
                firstRow = false;
                if (isHeaderFields(fields)) {
                    continue;
                }
            }

            if (fields.size() != 4) {
                error = "Invalid CSV format (expected 4 columns) at line " +
                        lineNumber;
                return false; 
            }

            const std::string& idStr = fields[0];
            const std::string& description = fields[1];
            const std::string& priority = fields[2];
            const std::string& arrivalSequence = fields[3];

            if (idStr.empty() || description.empty() ||
                priority.empty() || arrivalSequence.empty()) {
                error = "Missing required ticket data at line " + lineNumber;
                return false; 
            }

            long long id = 0;
            if (!parseLongLong(idStr, id) || id < 0) {
                error = "Invalid ticket ID '" + idStr + "' at line " + lineNumber;
                return false; 
            }

            int priorityValue = 0;
            if (!parseInt(priority, priorityValue) ||
                priorityValue < 1 || priorityValue > 3) {
                error = "Invalid ticket priority '" + priority +
                        "' at line " + lineNumber;
                return false; 
            }

            long long arrivalValue = 0;
            if (!parseLongLong(arrivalSequence, arrivalValue) ||
                arrivalValue < 0) {
                error = "Invalid arrival sequence '" + arrivalSequence +
                        "' at line " + lineNumber;
                return false; 
            }

            tempRecords.emplace_back(Ticket{
                id,
                description,
                priorityValue,
                arrivalValue
            });
        }

        records.insert(records.end(), tempRecords.begin(), tempRecords.end());
        return true; 
    }

} 
