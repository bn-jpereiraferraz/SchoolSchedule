#ifndef ENTITYFINDER_H
#define ENTITYFINDER_H

class Schedule;
class RecurringClass;
class OneTimeEvent;

class EntityFinder {
public:
    // Find recurring class by ID (mutable version)
    static RecurringClass* findClassById(Schedule& schedule, int id);

    // Find recurring class by ID (const version)
    static const RecurringClass* findClassById(const Schedule& schedule, int id);

    // Find one-time event by ID (mutable version)
    static OneTimeEvent* findEventById(Schedule& schedule, int id);

    // Find one-time event by ID (const version)
    static const OneTimeEvent* findEventById(const Schedule& schedule, int id);

    // Get index of recurring class by ID (-1 if not found)
    static int findClassIndexById(Schedule& schedule, int id);

    // Get index of one-time event by ID (-1 if not found)
    static int findEventIndexById(Schedule& schedule, int id);
};

#endif // ENTITYFINDER_H
