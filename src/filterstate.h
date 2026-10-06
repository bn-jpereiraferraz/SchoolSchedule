#ifndef FILTERSTATE_H
#define FILTERSTATE_H

#include <QString>

class FilterState {
public:
    enum FilterType {All = 0, ClassesOnly = 1, ExamsOnly = 2};

    FilterState();

    void setFilterType(FilterType type);
    void setSearchText(const QString& text);

    FilterType getFilterType() const;
    QString getSearchText() const;

    bool matchesTypeFilter(bool isClass) const;
    bool matchesSearchText(const QString& itemName) const;

private:
    FilterType currentFilterType;
    QString currentSearchText;
};

#endif // FILTERSTATE_H
