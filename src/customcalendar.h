#ifndef CUSTOMCALENDAR_H
#define CUSTOMCALENDAR_H
#include <QCalendarWidget>
#include <QDate>
#include <QPainter>
#include "scheduledata.h"

class CustomCalendar : public QCalendarWidget {
    Q_OBJECT
public:
    
    explicit CustomCalendar(QWidget *parent = nullptr);
    void setSchedule(const Schedule* schedule);

protected:

    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;

private:

    const Schedule* schedule = nullptr;

    //Date query helpers
    int getEventCount(const QDate& date) const;
    bool hasClasses(const QDate& date) const;
    bool hasEvents(const QDate& date) const;
    bool isToday(const QDate& date) const;
    bool isWeekend(const QDate& date) const;
    bool isPast(const QDate& date) const;

    //Painting orchestrators
    void paintBackground(QPainter *painter, const QRect &rect, const QDate& date) const;
    void paintEventIndicators(QPainter *painter, const QRect &rect, const QDate& date)const;
    void paintEventCount(QPainter *painter, const QRect &rect, int count)const;

    //Background color helpers
    QColor getBackgroundColor(const QDate &date)const;
    void drawTodayBorder(QPainter *painter, const QRect &rect)const;

    //Dot Painting helpers
    void drawSingleDot(QPainter *painter, int x, int y, const QColor& color, int size)const;
    void drawDoubleDots(QPainter *painter, int centerX, int y, int size)const;
    int calculateDotY(const QRect& rect, int dotSize)const;
    int calculateCenterX(const QRect& rect, int dotSize)const;

    //Badge helpers
    void drawBadgeCircle(QPainter *painter, int x, int y, int size) const;
    void drawBadgeText(QPainter *painter, const QRect& badgeRect, int count)const;
    QRect calculateBadgeRect(const QRect& cellRect, int badgeSize)const;
};

#endif // CUSTOMCALENDAR_H