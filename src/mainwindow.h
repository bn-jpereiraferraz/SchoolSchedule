#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "customcalendar.h"
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "schedulerepository.h"
#include "filterstate.h"
#include "eventlistbuilder.h"
#include <QProcess>
#include <QComboBox>

// Forward declarations
class AddClassDialog;
class AddEventDialog;



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

    // Dialog-to-domain object helpers
    RecurringClass createClassFromDialog(const AddClassDialog& dialog);
    OneTimeEvent createEventFromDialog(const AddEventDialog& dialog);
    void populateDialogFromClass(AddClassDialog& dialog, const RecurringClass& cls);
    void populateDialogFromEvent(AddEventDialog& dialog, const OneTimeEvent& evt);

private:
    void setupUI();
    QVBoxLayout* createLeftPanel();
    QVBoxLayout* createRightPanel();
    void connectSignals();

    void startServer();
    void setupRepository();
    void initializeRepository();


    // Search & Filter UI creation helpers
    QLineEdit* createQuickDateJumpInput();
    QComboBox* createFilterTypeCombo();
    QLineEdit* createSearchByNameInput();
    QHBoxLayout* createSearchFilterBar();

    // Date parsing helpers
    QDate parseRelativeDate(const QString& lower, const QDate& today);
    QDate parseWeekdayDate(const QString& lower, const QDate& today);
    QDate parseFormattedDate(const QString& input, const QDate& today);
    QDate parseNaturalLanguageDate(const QString& input);

    // Filter and list management
    void rebuildFilteredEventList();
    void updateFilterState();

    // Context menu handlers
    void showClassContextMenu(const QPoint& pos, int classId);
    void showEventContextMenu(const QPoint& pos, int eventId);

    QMap<QListWidgetItem*, int> itemToClassId;
    QMap<QListWidgetItem*, int> itemToEventId;
    CustomCalendar *calendar;
    QListWidget *eventList;
    QPushButton *addClassButton;
    QPushButton *addExamButton;
    QPushButton *todayButton;
    QLabel *selectedDateLabel;
    QProcess *serverProcess;

    //Search and Filter UI
    QLineEdit *quickDateJumpInput;
    QComboBox *filterTypeComboBox;
    QLineEdit *searchByNameInput;

    //Filter and list management
    FilterState filterState;
    EventListBuilder *eventListBuilder;

    //Cache for current date's events
    QVector<const RecurringClass*> currentDateClasses;
    QVector<const OneTimeEvent*> currentDateEvents;

    ScheduleRepository *repository = nullptr;

    void updateEventListForDate(const QDate &date);
};

#endif // MAINWINDOW_H
