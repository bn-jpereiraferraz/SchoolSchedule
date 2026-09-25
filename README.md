# School Calendar - Client-Server Architecture Documentation

## Project Overview

A cross-platform school calendar application implementing client-server architecture with REST API. The system allows users to manage recurring weekly classes and one-time events (exams, tests) with persistent storage and network synchronization.

**Target Platforms:** Linux, macOS  
**Language:** C++17  
**UI Framework:** Qt6  
**Network Protocol:** HTTP/REST  
**Data Format:** JSON

---

## Architecture

### Three-Tier Structure

```
Client (Qt GUI)  <--HTTP/JSON-->  Server (REST API)
       \                              /
        \                            /
         \                          /
          Shared Library (Data Model)
```

### Components

**1. Shared Library (libSchoolCalendarShared.a)**
- Contains data model definitions
- Used by both client and server
- Ensures consistent data structures across both components
- Files: `shared/scheduledata.h`, `shared/scheduledata.cpp`

**2. Qt GUI Client (SchoolCalendar)**
- Graphical user interface for calendar management
- Makes HTTP requests to server for data operations
- Displays calendar view and event lists
- Files: `src/main.cpp`, `src/mainwindow.h`, `src/mainwindow.cpp`, dialogs

**3. REST API Server (SchoolCalendarServer)**
- HTTP server listening on localhost:8080
- Processes API requests (GET, POST, DELETE)
- Manages persistent storage via JSON file
- Files: `server/main.cpp`

---

## Data Model

### RecurringClass
Represents a weekly repeating class.

**Fields:**
- `name` (QString) - Class name
- `dayOfWeek` (int) - Day of week (1=Monday, 7=Sunday)
- `startTime` (QTime) - Start time
- `endTime` (QTime) - End time
- `room` (QString) - Room location
- `teacher` (QString) - Teacher name
- `cancelledDates` (QSet<QDate>) - Set of dates when class is cancelled

**Methods:**
- `bool isCancelledOn(const QDate &date)` - Check if cancelled on specific date
- `void cancelOn(const QDate &date)` - Mark as cancelled
- `void uncancelOn(const QDate &date)` - Remove cancellation
- `QJsonObject toJson()` - Serialize to JSON
- `static RecurringClass fromJson(const QJsonObject &json)` - Deserialize from JSON

### OneTimeEvent
Represents a single-occurrence event (exam, test).

**Fields:**
- `name` (QString) - Event name
- `date` (QDate) - Event date
- `startTime` (QTime) - Start time
- `endTime` (QTime) - End time (optional)
- `location` (QString) - Event location
- `notes` (QString) - Additional notes

**Methods:**
- `QJsonObject toJson()` - Serialize to JSON
- `static OneTimeEvent fromJson(const QJsonObject &json)` - Deserialize from JSON

### Schedule
Main container managing all classes and events.

**Fields:**
- `recurringClasses` (QVector<RecurringClass>) - All weekly classes
- `oneTimeEvents` (QVector<OneTimeEvent>) - All one-time events

**Methods:**
- `void addRecurringClass(const RecurringClass &cls)` - Add recurring class
- `void addOneTimeEvent(const OneTimeEvent &evt)` - Add one-time event
- `void removeRecurringClass(int index)` - Remove class by index
- `void removeOneTimeEvent(int index)` - Remove event by index
- `QVector<QString> getEventsForDate(const QDate &date)` - Get formatted event list for date
- `QVector<const RecurringClass*> getClassesForDate(const QDate &date)` - Get classes for date
- `QVector<const OneTimeEvent*> getOneTimeEventsForDate(const QDate &date)` - Get events for date
- `bool saveToFile(const QString &filename)` - Persist to JSON file
- `bool loadFromFile(const QString &filename)` - Load from JSON file

---

## REST API Specification

### Base URL
```
http://localhost:8080/api
```

### Authentication
All requests require API key in header:
```
X-API-Key: your-secret-key
```

### Endpoints

#### Classes

**GET /api/classes**
- Returns all recurring classes
- Response: `{"classes": [...]}`

**POST /api/classes**
- Add new recurring class
- Request body: RecurringClass JSON object
- Response: `{"success": true, "id": <class_id>}`

**DELETE /api/classes/{id}**
- Delete recurring class by ID
- Response: `{"success": true}`

**POST /api/classes/{id}/cancel**
- Cancel specific occurrence of recurring class
- Request body: `{"date": "YYYY-MM-DD"}`
- Response: `{"success": true}`

#### Events

**GET /api/events**
- Returns all one-time events
- Response: `{"events": [...]}`

**POST /api/events**
- Add new one-time event
- Request body: OneTimeEvent JSON object
- Response: `{"success": true, "id": <event_id>}`

**DELETE /api/events/{id}**
- Delete one-time event by ID
- Response: `{"success": true}`

#### Schedule

**GET /api/schedule?date=YYYY-MM-DD**
- Get all events for specific date
- Response: `{"date": "YYYY-MM-DD", "events": [...]}`

---

## File Structure

```
SchoolCalendar/
├── CMakeLists.txt              # Build configuration
├── README.md                   # Project overview
├── DOCUMENTATION.md            # This file
├── .gitignore                  # Git ignore rules
├── shared/                     # Shared data model
│   ├── scheduledata.h          # Data structure definitions
│   └── scheduledata.cpp        # Data structure implementations
├── src/                        # Qt GUI Client
│   ├── main.cpp                # Client entry point
│   ├── mainwindow.h            # Main window header
│   ├── mainwindow.cpp          # Main window implementation
│   ├── addclassdialog.h        # Add class dialog (TODO)
│   ├── addclassdialog.cpp
│   ├── addeventdialog.h        # Add event dialog (TODO)
│   └── addeventdialog.cpp
├── server/                     # REST API Server
│   ├── main.cpp                # Server entry point
│   ├── httplib.h               # HTTP server library (single-header)
│   └── json.hpp                # JSON library (single-header)
├── data/                       # Runtime data directory
│   └── schedule.json           # Persistent storage (created at runtime)
└── build/                      # Build output
    ├── SchoolCalendar          # GUI client executable
    ├── SchoolCalendarServer    # API server executable
    └── libSchoolCalendarShared.a  # Shared library
```

---

## Build Instructions

### Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt install build-essential cmake qt6-base-dev qt6-tools-dev
```

**macOS:**
```bash
brew install cmake qt@6
```

### Build Steps

```bash
cd SchoolCalendar
mkdir build
cd build
cmake ..
make
```

### Build Targets

- `SchoolCalendar` - GUI client
- `SchoolCalendarServer` - REST API server
- `SchoolCalendarShared` - Shared library (static)

---

## Running the Application

### Start Server

```bash
cd build
./SchoolCalendarServer
```

Server starts on `http://localhost:8080`

### Start Client

```bash
cd build
./SchoolCalendar
```

### Test Server Manually

```bash
# Check server status
curl http://localhost:8080/

# Get all classes (requires implementation)
curl -H "X-API-Key: your-secret-key" http://localhost:8080/api/classes

# Add a class (requires implementation)
curl -X POST -H "X-API-Key: your-secret-key" \
  -H "Content-Type: application/json" \
  -d '{"name":"Math","dayOfWeek":1,"startTime":"09:00","endTime":"10:30"}' \
  http://localhost:8080/api/classes
```

---

## Network Architecture

### Communication Flow

1. User interacts with GUI (clicks "Add Class")
2. Client constructs HTTP POST request with JSON payload
3. Client sends request to server at localhost:8080
4. Server receives request, validates API key
5. Server parses JSON, creates data model object
6. Server updates in-memory schedule
7. Server persists changes to schedule.json
8. Server sends HTTP response (success/failure)
9. Client parses response, updates UI

### Protocol Details

**Transport:** TCP  
**Application Layer:** HTTP/1.1  
**Data Format:** JSON  
**Authentication:** API Key (HTTP header)  
**Port:** 8080 (configurable)  
**Interface:** Loopback (127.0.0.1)

---

## Security Considerations

### Current Implementation
- Server binds to localhost only (127.0.0.1)
- Not accessible from network
- API key authentication (basic)

### Production Deployment Considerations
- Use HTTPS/TLS for encrypted communication
- Implement proper authentication (OAuth, JWT)
- Add rate limiting to prevent abuse
- Validate and sanitize all inputs
- Use firewall rules to restrict access
- Implement audit logging
- Consider database instead of JSON file

---

## Development Status

### Completed
- Data model (RecurringClass, OneTimeEvent, Schedule)
- JSON serialization/deserialization
- Project structure (client-server separation)
- Build system (CMake configuration)
- Basic server skeleton

### In Progress
- REST API endpoint implementation
- Client HTTP communication layer

### Pending
- Add Class dialog (GUI)
- Add Event dialog (GUI)
- Event display in calendar view
- Class cancellation UI
- API authentication implementation
- Error handling and validation
- Unit tests

---

## Dependencies

### Runtime Dependencies
- Qt6 Core (6.4+)
- Qt6 Widgets (6.4+)
- Qt6 Network (6.4+)
- pthread (POSIX threads)

### Build Dependencies
- CMake (3.16+)
- C++17 compatible compiler (GCC 7+, Clang 5+)

### Third-Party Libraries (Header-Only, Included)
- cpp-httplib (HTTP server) - https://github.com/yhirose/cpp-httplib
- nlohmann/json (JSON parsing) - https://github.com/nlohmann/json

---

## Firewall Relevance

This project demonstrates concepts relevant to firewall software development:

**Port Management:**
- Server binds to specific port (8080)
- Understanding of socket binding and listening
- Port conflict handling

**Protocol Analysis:**
- HTTP request/response parsing
- Header inspection (API key)
- Payload validation

**Access Control:**
- IP-based restrictions (localhost only)
- Authentication mechanisms
- Request filtering

**Network Traffic:**
- TCP connection handling
- Client-server communication patterns
- Request routing and endpoint mapping

**Security:**
- Input validation
- Authentication/authorization
- Attack surface analysis
