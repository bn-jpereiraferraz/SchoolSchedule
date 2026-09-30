#include "entityfinder.h"
#include "scheduledata.h"

// Find recurring class by ID (mutable version)
RecurringClass* EntityFinder::findClassById(Schedule& schedule, int id) {
    auto& classes = schedule.getRecurringClassesMutable();
    for (int i = 0; i < classes.size(); i++) {
        if (classes[i].getId() == id) {
            return &classes[i];
        }
    }
    return nullptr;
}

// Find recurring class by ID (const version)
const RecurringClass* EntityFinder::findClassById(const Schedule& schedule, int id) {
    const auto& classes = schedule.getRecurringClasses();
    for (const auto& cls : classes) {
        if (cls.getId() == id) {
            return &cls;
        }
    }
    return nullptr;
}

// Find one-time event by ID (mutable version)
OneTimeEvent* EntityFinder::findEventById(Schedule& schedule, int id) {
    auto& events = schedule.getOneTimeEventsMutable();
    for (int i = 0; i < events.size(); i++) {
        if (events[i].getId() == id) {
            return &events[i];
        }
    }
    return nullptr;
}

// Find one-time event by ID (const version)
const OneTimeEvent* EntityFinder::findEventById(const Schedule& schedule, int id) {
    const auto& events = schedule.getOneTimeEvents();
    for (const auto& evt : events) {
        if (evt.getId() == id) {
            return &evt;
        }
    }
    return nullptr;
}

// Get index of recurring class by ID
int EntityFinder::findClassIndexById(Schedule& schedule, int id) {
    const auto& classes = schedule.getRecurringClasses();
    for (int i = 0; i < classes.size(); i++) {
        if (classes[i].getId() == id) {
            return i;
        }
    }
    return -1;
}

// Get index of one-time event by ID
int EntityFinder::findEventIndexById(Schedule& schedule, int id) {
    const auto& events = schedule.getOneTimeEvents();
    for (int i = 0; i < events.size(); i++) {
        if (events[i].getId() == id) {
            return i;
        }
    }
    return -1;
}
