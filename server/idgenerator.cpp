#include "idgenerator.h"

IdGenerator::IdGenerator()
    : nextClassId(1), nextEventId(1) {
}

void IdGenerator::initializeFromSchedule(const Schedule& schedule) {
    findHighestClassId(schedule);
    findHighestEventId(schedule);
}

int IdGenerator::generateClassId() {
    return nextClassId++;
}

int IdGenerator::generateEventId() {
    return nextEventId++;
}

void IdGenerator::findHighestClassId(const Schedule& schedule) {
    for (const auto& cls : schedule.getRecurringClasses()) {
        if (cls.getId() >= nextClassId) {
            nextClassId = cls.getId() + 1;
        }
    }
}

void IdGenerator::findHighestEventId(const Schedule& schedule) {
    for (const auto& evt : schedule.getOneTimeEvents()) {
        if (evt.getId() >= nextEventId) {
            nextEventId = evt.getId() + 1;
        }
    }
}
