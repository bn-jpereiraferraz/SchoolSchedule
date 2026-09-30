#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "schedulerepository.h"
#include <QProcess>



class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDateSelected(const QDate &date);
    void onAddClassClicked();
    void onAddExamClicked();
    void onContextMenu(const QPoint &pos);

    // Context menu handlers
    void onCancelClassSession(int classId);
    void onDeleteClass(int classId);
    void onEditClass(int classId);
    void onEditEvent(int eventId);
    void onDeleteEvent(int eventId);

    //Repository response handlers
    void onRepositoryError(const QString& operation, const QString& error);
    void onDataLoaded();

private:
    void setupUI();
    QVBoxLayout* createLeftPanel();
    QVBoxLayout* createRightPanel();
    void connectSignals();

    void startServer();
    void setupRepository();
    void initializeRepository();

    // UI population helpers
    int addClassesToList(const QDate& date, const Schedule* schedule);
    int addEventsToList(const QDate& date, const Schedule* schedule);

    // Context menu handlers
    void showClassContextMenu(const QPoint& pos, int classId);
    void showEventContextMenu(const QPoint& pos, int eventId);

    QMap<QListWidgetItem*, int> itemToClassId;
    QMap<QListWidgetItem*, int> itemToEventId;
    QCalendarWidget *calendar;
    QListWidget *eventList;
    QPushButton *addClassButton;
    QPushButton *addExamButton;
    QLabel *selectedDateLabel;
    QProcess *serverProcess;

    ScheduleRepository *repository = nullptr;

    void updateEventListForDate(const QDate &date);
};

#endif // MAINWINDOW_H
