#ifndef IDGENERATOR_H
#define IDGENERATOR_H

#include "../shared/scheduledata.h"

class IdGenerator {
public:
    IdGenerator();

    void initializeFromSchedule(const Schedule& schedule);
    int generateClassId();
    int generateEventId();

private:
    void findHighestClassId(const Schedule& schedule);
    void findHighestEventId(const Schedule& schedule);

    int nextClassId;
    int nextEventId;
};

#endif // IDGENERATOR_H
