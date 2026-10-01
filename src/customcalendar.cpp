#include "customcalendar.h"
#include "constants.h"
#include <QPainter>
#include <QBrush>
#include <QPen>

CustomCalendar::CustomCalendar(QWidget *parent)
    : QCalendarWidget(parent){
        setGridVisible(true);
    }

void CustomCalendar::setSchedule(const Schedule* sched){
    schedule = sched;
    updateCells();
}

//Main painting orchestrator
void CustomCalendar::paintCell(QPainter *painter, const QRect &rect, QDate date)const{
    painter->save();

    paintBackground(painter, rect, date);
    QCalendarWidget::paintCell(painter, rect, date);

    if(schedule){
        paintEventIndicators(painter, rect, date);

        int count = getEventCount(date);
        if (count > 0){
            paintEventCount(painter, rect, count);
        }
    }
    painter->restore();
}

//Date Query Helpers

int CustomCalendar::getEventCount(const QDate& date) const{
    if (!schedule) return 0;

    int count = 0;
    count += schedule->getClassesForDate(date).size();
    count += schedule->getOneTimeEventsForDate(date).size();

    return count;
}

bool CustomCalendar::hasClasses(const QDate& date)const{
    if (!schedule) return false;
    return !schedule->getClassesForDate(date).isEmpty();
}

bool CustomCalendar::hasEvents(const QDate& date)const{
    if (!schedule) return false;
    return !schedule->getOneTimeEventsForDate(date).isEmpty();
}

bool CustomCalendar::isToday(const QDate& date)const{
    return date == QDate::currentDate();
}

bool CustomCalendar::isWeekend(const QDate& date) const{
    int dayOfWeek = date.dayOfWeek();
    return dayOfWeek == Qt::Saturday || dayOfWeek == Qt::Sunday;
}

bool CustomCalendar::isPast(const QDate& date) const{
    return date < QDate::currentDate();
}

//Background Painting
void CustomCalendar::paintBackground(QPainter *painter, const QRect &rect, const QDate& date)const{
    QColor bgColor = getBackgroundColor(date);
    painter->fillRect(rect, bgColor);

    if (isToday(date)){
        drawTodayBorder(painter, rect);
    }
}

QColor CustomCalendar::getBackgroundColor(const QDate& date)const{
    if (isToday(date)){
        return QColor(Constants::EventColors::CALENDAR_TODAY_BG);
    }else if (isWeekend(date)){
        return QColor(Constants::EventColors::CALENDAR_WEEKEND_BG);
    }else if(isPast(date)){
        return QColor(Constants::EventColors::CALENDAR_PAST_BG);
    }else{
        return Qt::white;
    }
}

void CustomCalendar::drawTodayBorder(QPainter *painter, const QRect &rect)const{
    painter->setPen(QPen(QColor(Constants::EventColors::CLASS_COLOR), 2));
    painter->drawRect(rect.adjusted(1, 1, -1, -1));
}

//Event Indicator Painting

void CustomCalendar::paintEventIndicators(QPainter *painter, const QRect &rect, const QDate& date)const{
    bool hasClass = hasClasses(date);
    bool hasEvent = hasEvents(date);

    if (!hasClass && !hasEvent) return;

    painter->setRenderHint(QPainter::Antialiasing);

    const int dotSize = 6;
    int dotY = calculateDotY(rect, dotSize);
    int centerX = calculateCenterX(rect, dotSize);

    if (hasClass && hasEvent){
        drawDoubleDots(painter, centerX, dotY, dotSize);
    }else if(hasClass){
        drawSingleDot(painter, centerX, dotY, QColor(Constants::EventColors::CLASS_COLOR), dotSize);
    }else if(hasEvent){
        drawSingleDot(painter, centerX, dotY, QColor(Constants::EventColors::EXAM_COLOR), dotSize);
    }
}

int CustomCalendar::calculateDotY(const QRect& rect, int dotSize)const{
    return rect.bottom() - dotSize - 2;
}

int CustomCalendar::calculateCenterX(const QRect& rect, int dotSize)const{
    return rect.center().x() - (dotSize / 2);
}

void CustomCalendar::drawSingleDot(QPainter *painter, int x, int y, const QColor& color, int size)const{
    painter->setBrush(color);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(x, y, size, size);
}

void CustomCalendar::drawDoubleDots(QPainter *painter, int centerX, int y, int size) const{
    const int spacing = 2;
    const int totalWidth = (size * 2) + spacing;
    const int startX = centerX - (totalWidth / 2) + (size / 2);

    //blue dot for classes
    drawSingleDot(painter, startX, y, QColor(Constants::EventColors::CLASS_COLOR), size);

    //red dot for events
    drawSingleDot(painter, startX + size + spacing, y, QColor(Constants::EventColors::EXAM_COLOR), size);
}

//Event Count Badge
void CustomCalendar::paintEventCount(QPainter *painter, const QRect &rect, int count)const{
    if (count <= 0)return;

    painter->setRenderHint(QPainter::Antialiasing);

    const int badgeSize = 18;
    QRect badgeRect = calculateBadgeRect(rect, badgeSize);

    drawBadgeCircle(painter, badgeRect.x(), badgeRect.y(), badgeSize);
    drawBadgeText(painter, badgeRect, count);
}

QRect CustomCalendar::calculateBadgeRect(const QRect& cellRect, int badgeSize)const{
    int x = cellRect.right() - badgeSize - 2;
    int y = cellRect.top() + 2;
    return QRect(x, y, badgeSize, badgeSize);
}

void CustomCalendar::drawBadgeCircle(QPainter *painter, int x, int y, int size)const{
    painter->setBrush(QColor(Constants::EventColors::CLASS_COLOR));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(x, y, size, size);
}

void CustomCalendar::drawBadgeText(QPainter *painter, const QRect& badgeRect, int count)const{
    painter->setPen(Qt::white);

    QFont font = painter->font();
    font.setPointSize(8);
    font.setBold(true);
    painter->setFont(font);

    painter->drawText(badgeRect, Qt::AlignCenter, QString::number(count));
}