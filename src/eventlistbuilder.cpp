#include "eventlistbuilder.h"
#include "displayformatter.h"
#include "constants.h"

using namespace Constants::EventColors;

EventListBuilder::EventListBuilder(QListWidget* listWidget,
                                   QMap<QListWidgetItem*, int>& classIdMap,
                                   QMap<QListWidgetItem*, int>& eventIdMap)
    : listWidget(listWidget), itemToClassId(classIdMap), itemToEventId(eventIdMap) {
}

void EventListBuilder::clearList() {
    listWidget->clear();
    itemToClassId.clear();
    itemToEventId.clear();
}

int EventListBuilder::addFilteredClasses(const QVector<const RecurringClass*>& classes,
                                        const FilterState& filter) {
    int count = 0;
    for (const RecurringClass* cls : classes) {
        if (!cls) continue;

        if (filter.matchesTypeFilter(true) && filter.matchesSearchText(cls->getName())) {
            QListWidgetItem* item = createStyledClassItem(*cls);
            listWidget->addItem(item);
            itemToClassId[item] = cls->getId();
            count++;
        }
    }
    return count;
}

int EventListBuilder::addFilteredEvents(const QVector<const OneTimeEvent*>& events,
                                       const FilterState& filter) {
    int count = 0;
    for (const OneTimeEvent* evt : events) {
        if (!evt) continue;

        if (filter.matchesTypeFilter(false) && filter.matchesSearchText(evt->getName())) {
            QListWidgetItem* item = createStyledEventItem(*evt);
            listWidget->addItem(item);
            itemToEventId[item] = evt->getId();
            count++;
        }
    }
    return count;
}

void EventListBuilder::showEmptyMessage() {
    QListWidgetItem* emptyItem = new QListWidgetItem("No events match your filters");
    emptyItem->setForeground(QColor("#9E9E9E"));
    emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsSelectable);
    listWidget->addItem(emptyItem);
}

QListWidgetItem* EventListBuilder::createStyledClassItem(const RecurringClass& cls) {
    QString richText = DisplayFormatter::formatClassRich(cls);
    QListWidgetItem* item = new QListWidgetItem(richText);
    applyItemStyle(item, CLASS_BG_LIGHT, CLASS_COLOR);
    return item;
}

QListWidgetItem* EventListBuilder::createStyledEventItem(const OneTimeEvent& evt) {
    QString richText = DisplayFormatter::formatEventRich(evt);
    QListWidgetItem* item = new QListWidgetItem(richText);
    applyItemStyle(item, EXAM_BG_LIGHT, EXAM_COLOR);
    return item;
}

void EventListBuilder::applyItemStyle(QListWidgetItem* item, const QString& backgroundColor, const QString& textColor) {
    item->setBackground(QColor(backgroundColor));
    item->setForeground(QColor(textColor));
}
