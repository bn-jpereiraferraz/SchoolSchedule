#include "filterstate.h"

FilterState::FilterState()
    : currentFilterType(All), currentSearchText("") {
}

void FilterState::setFilterType(FilterType type) {
    currentFilterType = type;
}

void FilterState::setSearchText(const QString& text) {
    currentSearchText = text;
}

FilterState::FilterType FilterState::getFilterType() const {
    return currentFilterType;
}

QString FilterState::getSearchText() const {
    return currentSearchText;
}

bool FilterState::matchesTypeFilter(bool isClass) const {
    if (currentFilterType == All) {
        return true;
    }
    if (currentFilterType == ClassesOnly) {
        return isClass;
    }
    if (currentFilterType == ExamsOnly) {
        return !isClass;
    }
    return false;
}

bool FilterState::matchesSearchText(const QString& itemName) const {
    if (currentSearchText.isEmpty()) {
        return true;
    }
    return itemName.contains(currentSearchText, Qt::CaseInsensitive);
}
