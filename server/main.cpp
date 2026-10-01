#include "schoolcalendarserver.h"
#include "constants.h"

using namespace ServerConstants;

int main() {
    SchoolCalendarServer server(SERVER_HOST, SERVER_PORT, API_KEY, DATA_FILE);
    server.start();
    return 0;
}
