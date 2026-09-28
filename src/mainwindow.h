#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>

class Schedule;  // Forward declaration

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDateSelected(const QDate &date);
    void onAddClassClicked();
    void onAddExamClicked();
    void onClassesFetched(QNetworkReply *reply);
    void onEventsFetched(QNetworkReply *reply);
    void onClassAdded (QNetworkReply *reply);

private:
    void setupUI();

    QCalendarWidget *calendar;
    QListWidget *eventList;
    QPushButton *addClassButton;
    QPushButton *addExamButton;
    QLabel *selectedDateLabel;
    Schedule *schedule;
    QString scheduleFilePath;
    QNetworkAccessManager *networkManager;
    QString serverUrl;
    QString apiKey;
    QProcess *serverProcess;

    void fetchClasses();
    void fetchEvents();
    void updateEventListForDate(const QDate &date);

    void loadSchedule();
    void saveSchedule();
};

#endif // MAINWINDOW_H
