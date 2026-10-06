#ifndef EVENTLISTBUILDER_H
#define EVENTLISTBUILDER_H

#include <QListWidget>
#include <QVector>
#include "scheduledata.h"
#include "filterstate.h"

class EventListBuilder {
public:
    EventListBuilder(QListWidget* listWidget,
                     QMap<QListWidgetItem*, int>& classIdMap,
                     QMap<QListWidgetItem*, int>& eventIdMap);

    void clearList();
    int addFilteredClasses(const QVector<const RecurringClass*>& classes,
                          const FilterState& filter);
    int addFilteredEvents(const QVector<const OneTimeEvent*>& events,
                         const FilterState& filter);
    void showEmptyMessage();

private:
    QListWidgetItem* createStyledClassItem(const RecurringClass& cls);
    QListWidgetItem* createStyledEventItem(const OneTimeEvent& evt);
    void applyItemStyle(QListWidgetItem* item, const QString& backgroundColor, const QString& textColor);

    QListWidget* listWidget;
    QMap<QListWidgetItem*, int>& itemToClassId;
    QMap<QListWidgetItem*, int>& itemToEventId;
};

#endif // EVENTLISTBUILDER_H
