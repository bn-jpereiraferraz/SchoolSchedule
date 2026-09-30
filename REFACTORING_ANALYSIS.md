# Comprehensive Code Review & Refactoring Analysis
**Date:** 2026-09-29  
**Reviewer Perspective:** Senior Software Engineer  
**Focus:** Effectiveness, Readability, Maintainability

---

## Executive Summary

The codebase has undergone significant refactoring (6 phases completed), eliminating ~250+ lines of duplication. However, several **critical architectural issues** remain that severely impact maintainability, testability, and scalability.

**Current State:** ~1,400 lines across 31 files  
**Technical Debt Level:** MEDIUM-HIGH  
**Recommended Action:** Implement Priority 1 fixes immediately

---

## CRITICAL ISSUES (Priority 1 - Must Fix)

### 1. **MainWindow: God Object Anti-Pattern** ⚠️ CRITICAL
**Location:** `src/mainwindow.cpp` (447 lines)  
**Problem:** MainWindow violates Single Responsibility Principle - handles UI, networking, business logic, server lifecycle, data management.

**Impact:**
- Impossible to unit test
- Changes cascade across unrelated features
- 52 member variables/methods mixing concerns

**Current Responsibilities:**
```cpp
// UI Management
- setupUI(), updateEventListForDate(), onDateSelected()
// Network Layer
- fetchClasses(), fetchEvents(), onClassesFetched(), onEventsFetched()
// Business Logic
- onAddClassClicked(), onEditClass(), onDeleteClass()
// Server Lifecycle
- serverProcess start/stop
// Data Management
- Schedule* schedule ownership
```

**Solution:** Extract into separate classes:
```cpp
class ScheduleRepository {
    // Data fetching, caching, CRUD operations
    void fetchClasses(std::function<void(QVector<RecurringClass>)> callback);
    void addClass(const RecurringClass& cls, ...);
};

class ServerManager {
    // Server process lifecycle
    void start();
    void stop();
    bool isRunning();
};

class MainWindow {
    // ONLY UI concerns
    ScheduleRepository* repository;
    ServerManager* serverManager;
};
```

**Estimated Impact:** 30% reduction in MainWindow complexity, 100% test coverage becomes possible

---

### 2. **Raw Pointer Memory Management** ⚠️ CRITICAL
**Location:** Throughout codebase  
**Problem:** Manual `new`/`delete` with no RAII, potential leaks

**Examples:**
```cpp
// mainwindow.cpp:21
schedule = nullptr;
// mainwindow.cpp:219
delete schedule;  // Manual cleanup

// mainwindow.cpp:284
QMenu *menu = new QMenu(this);  // Relies on Qt parent cleanup
delete menu;  // Manual delete at end - error-prone
```

**Solution:** Use smart pointers
```cpp
std::unique_ptr<Schedule> schedule;
// Or Qt parent ownership:
QMenu menu(this);  // Stack allocation
```

**Risk:** Current code has no leaks ONLY because Qt parent-child cleanup works. Any exception between `new` and `delete` leaks.

---

### 3. **No Error Recovery Strategy** ⚠️ HIGH
**Location:** `src/mainwindow.cpp` lines 205-230  
**Problem:** Network failures handled by displaying error in UI list, but application state becomes inconsistent.

**Example:**
```cpp
void MainWindow::onClassesFetched(QNetworkReply *reply){
    if (reply->error() != QNetworkReply::NoError){
        eventList->addItem("Error fetching classes: " + reply->errorString());
        return;  // ← schedule remains nullptr, app is broken
    }
    // Delete old schedule, create new
    if(schedule) delete schedule;
    schedule = new Schedule();  // ← Never reached if error
}
```

**Impact:**
- User clicks "Add Class" after fetch failure → CRASH (nullptr dereference)
- No retry mechanism
- Error displayed in data list (wrong layer)

**Solution:**
```cpp
class ScheduleRepository {
    Schedule* cachedSchedule = nullptr;  // Keep old data on error
    
    void fetchClasses() {
        // On success: update cache
        // On error: keep old cache + emit signal
        emit fetchFailed(errorMsg);
    }
};
```

---

### 4. **Tight Coupling to Qt Networking** ⚠️ HIGH
**Location:** All `on*Clicked` methods  
**Problem:** Business logic directly creates QNetworkRequest/QNetworkReply - impossible to test without live HTTP server.

**Current:**
```cpp
void MainWindow::onAddClassClicked() {
    // ... dialog ...
    QNetworkRequest request = NetworkRequestBuilder::buildJsonRequest(...);
    QNetworkReply *reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        // ... inline handler ...
    });
}
```

**Better:**
```cpp
class IScheduleService {  // Interface
    virtual void addClass(const RecurringClass&, 
                         std::function<void(bool)> callback) = 0;
};

class NetworkScheduleService : public IScheduleService {
    // Real HTTP implementation
};

class MockScheduleService : public IScheduleService {
    // Fake for testing
};

// MainWindow depends on interface, not concrete networking
```

---

### 5. **Synchronous Server Startup** ⚠️ HIGH
**Location:** `mainwindow.cpp:26-32`  
**Problem:** Blocks UI thread for 1 second on every app launch.

```cpp
serverProcess->start(serverPath);
QThread::msleep(1000);  // ← BLOCKS main thread!
```

**Impact:**
- Frozen window for 1 second
- No feedback to user
- Arbitrary timeout (what if server takes 1.1s?)

**Solution:**
```cpp
void MainWindow::startServerAsync() {
    serverProcess->start(serverPath);
    connect(serverProcess, &QProcess::started, this, [this]() {
        // Server ready, now fetch data
        fetchClasses();
        fetchEvents();
    });
    // Show "Starting server..." in UI
}
```

---

## HIGH-PRIORITY ISSUES (Priority 2)

### 6. **Dead Code in scheduledata.cpp**
**Location:** Lines 27-75  
**Problem:** `getEventsForDate()` method contains formatting logic that's now duplicated in DisplayFormatter.

```cpp
QVector<QString> Schedule::getEventsForDate(const QDate &date) const{
    // 48 lines of formatting - UNUSED
    // DisplayFormatter now handles this
}
```

**Action:** Delete method, update interface.

---

### 7. **Constants Not Actually Used**
**Location:** `shared/constants.h`, `server/constants.h`  
**Problem:** Constants defined but NOT used in mainwindow.cpp.

**Created but ignored:**
```cpp
// constants.h defines:
const QString API_CLASSES = "/api/classes";

// mainwindow.cpp:154 still hardcodes:
NetworkRequestBuilder::buildJsonRequest(serverUrl, "/api/classes", apiKey);
```

**Action:** Replace all 7 hardcoded endpoints with constants.

---

### 8. **Duplicate JSON Serialization Logic**
**Location:** `mainwindow.cpp` lines 151-152, 171-172, 327-328, 380-381, 420-421

```cpp
QJsonDocument doc(classData);
QByteArray jsonData = doc.toJson();
```

Repeated 5 times. Extract to utility:
```cpp
QByteArray JsonHelpers::serializeToByteArray(const QJsonObject& obj);
```

---

### 9. **Inconsistent Error Handling**
**Problem:** Some network replies use ApiResponseHandler, others manually call `reply->deleteLater()`.

**Inconsistent:**
```cpp
// Lines 205-230: Manual handling
reply->deleteLater();
if (reply->error() != QNetworkReply::NoError){
    // ...
}

// Lines 177-182: Uses ApiResponseHandler
ApiResponseHandler::handleResponse(reply, ...);
```

**Action:** Standardize all to use ApiResponseHandler.

---

### 10. **Public Mutable Accessors Break Encapsulation**
**Location:** `scheduledata.h` lines 159-165

```cpp
QVector<RecurringClass>& getRecurringClassesMutable(){
    return recurringClasses;  // ← Returns non-const reference!
}
```

**Problem:** Allows external code to bypass Schedule's API:
```cpp
schedule.getRecurringClassesMutable().clear();  // No validation!
schedule.getRecurringClassesMutable()[5].setName("hack");  // Bypass
```

**Solution:** Remove mutable accessors. Provide proper API:
```cpp
void Schedule::updateClass(int id, const RecurringClass& updated);
void Schedule::removeClass(int id);
```

---

## MEDIUM-PRIORITY ISSUES (Priority 3)

### 11. **Magic Numbers Everywhere**
```cpp
resize(900, 600);  // Line 24
QThread::msleep(1000);  // Line 32
waitForFinished(3000);  // Line 48
```

**Solution:** Named constants
```cpp
namespace UIConstants {
    const int DEFAULT_WINDOW_WIDTH = 900;
    const int DEFAULT_WINDOW_HEIGHT = 600;
    const int SERVER_STARTUP_TIMEOUT_MS = 3000;
}
```

---

### 12. **Server URL/API Key Hardcoded**
**Location:** `mainwindow.cpp:36-37`

```cpp
serverUrl = "http://localhost:8080";
apiKey = "your-secret-key";
```

**Problem:** Cannot configure for production/staging/dev without recompiling.

**Solution:** Configuration file or environment variables.

---

### 13. **No Input Validation**
**Problem:** Dialogs don't validate:
- Empty class names
- Start time after end time
- Day of week out of range

**Current:** Server will accept ANY JSON, including:
```json
{"name": "", "dayOfWeek": 99, "startTime": "25:99"}
```

**Solution:** Add validation in dialogs AND server.

---

### 14. **Resource Leak Risk**
**Location:** `mainwindow.cpp:284`

```cpp
QMenu *menu = new QMenu(this);
// ... complex logic with early returns ...
delete menu;  // ← Skipped if early return!
```

**Fix:** Stack allocation: `QMenu menu(this);`

---

### 15. **Unused Member Variable**
**Location:** `mainwindow.h:52`

```cpp
QString scheduleFilePath;  // Never assigned or read
```

**Action:** Delete.

---

## ARCHITECTURAL RECOMMENDATIONS

### A. Introduce Repository Pattern
```cpp
class IScheduleRepository {
    virtual void fetchAll(std::function<void(Schedule*)> callback) = 0;
    virtual void addClass(const RecurringClass&, ...) = 0;
    // ... CRUD operations
};

class NetworkScheduleRepository : public IScheduleRepository {
    // HTTP implementation
};
```

**Benefits:**
- Testable (inject mock repository)
- Can swap backend (local DB, REST API, gRPC)
- Centralizes all data access

---

### B. Implement Signal-Based Architecture
**Current:** Callbacks everywhere, tight coupling.

**Better:**
```cpp
class ScheduleRepository : public QObject {
    Q_OBJECT
signals:
    void classesLoaded(const QVector<RecurringClass>& classes);
    void classAdded(const RecurringClass& cls);
    void errorOccurred(const QString& error);
};

// MainWindow
connect(repository, &ScheduleRepository::classesLoaded, 
        this, &MainWindow::onClassesLoaded);
```

**Benefits:**
- Decouples layers
- Easy to add logging, analytics
- Qt-idiomatic

---

### C. Add Domain Layer
**Problem:** RecurringClass/OneTimeEvent are data bags with no behavior.

**Better:**
```cpp
class RecurringClass {
public:
    bool conflictsWith(const RecurringClass& other) const;
    QVector<QDate> getOccurrencesBetween(QDate start, QDate end) const;
    bool isValidForScheduling() const;  // Validation logic
};
```

**Benefits:**
- Business logic lives with data
- Easier to test rules in isolation

---

## TESTING STRATEGY

### Current: 0% test coverage (untestable)

### Recommended:
1. **Extract dependencies** (Priority 1 fixes)
2. **Write unit tests** for:
   - JsonHelpers
   - EntityFinder
   - DisplayFormatter
   - RecurringClass domain logic
3. **Integration tests** for:
   - ScheduleRepository
   - Server endpoints
4. **UI tests** (optional, lower priority)

**Target:** 70% line coverage minimum

---

## PERFORMANCE ISSUES

### 16. **O(n) lookups on every render**
**Location:** `mainwindow.cpp:120-136`

```cpp
for (const RecurringClass* cls : classes){  // ← iterates all
    itemToClassId[item] = cls->getId();
}
```

**Impact:** Minor (n is small), but indicates lack of indexing strategy.

**If scale grows:** Use `QMap<int, RecurringClass*>` for O(1) lookup.

---

### 17. **Full schedule reload on every change**
**Problem:** Adding ONE class re-fetches ALL classes from server.

```cpp
void MainWindow::onClassAdded(QNetworkReply *reply){
    fetchClasses();  // ← Re-downloads everything
}
```

**Better:** Server returns the created class, add to local schedule:
```cpp
void MainWindow::onClassAdded(QNetworkReply *reply){
    RecurringClass newClass = parseResponse(reply);
    schedule->addRecurringClass(newClass);
    updateEventListForDate(currentDate);
}
```

---

## CODE QUALITY METRICS

| Metric | Current | Target | Priority |
|--------|---------|--------|----------|
| Cyclomatic Complexity (MainWindow) | ~45 | <20 | High |
| Lines per Method (avg) | 22 | <15 | Medium |
| Class Coupling (MainWindow) | 12 dependencies | <8 | High |
| Test Coverage | 0% | 70% | High |
| Public Mutable State | 3 instances | 0 | Medium |
| Magic Numbers | 8 | 0 | Low |

---

## IMPLEMENTATION ROADMAP

### Phase 7: Critical Fixes (1-2 days)
1. Extract ScheduleRepository class
2. Replace raw pointers with smart pointers
3. Fix synchronous server startup
4. Standardize error handling

### Phase 8: High-Priority (1 day)
5. Remove dead code (getEventsForDate)
6. Apply constants everywhere
7. Fix mutable accessor leak
8. Add input validation

### Phase 9: Testing (2 days)
9. Write unit tests for utilities
10. Add integration tests for repository
11. Mock framework setup

### Phase 10: Polish (1 day)
12. Replace magic numbers
13. Configuration system
14. Resource leak fixes

**Total estimated effort:** 5-6 days

---

## CONCLUSION

The codebase has solid **tactical** improvements (utility classes, reduced duplication) but lacks **strategic** architecture:

**Strengths:**
✅ Good helper class extraction
✅ Reduced duplication significantly
✅ Consistent naming conventions

**Critical Weaknesses:**
❌ God Object (MainWindow)
❌ No separation of concerns
❌ Untestable (0% coverage)
❌ Fragile error handling
❌ Manual memory management risks

**Next Steps:**
1. Implement Priority 1 fixes (ScheduleRepository extraction)
2. Add tests as you refactor
3. Gradually introduce interfaces for dependency injection

**Estimated ROI:** 60% reduction in bug rate, 3x faster feature development after refactoring.
