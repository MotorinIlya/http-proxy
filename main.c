#include "proxy.h"

int main() {
    logg("SERVER START", GREEN);
    run_proxy();
    exit(EXIT_SUCCESS);
}