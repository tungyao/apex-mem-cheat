#include "d3dguix.h"

int main() {
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) Start, NULL, 0, NULL);
    while (1) {
        cout << "Input 1 close:" << endl;
        int a = 0;
        cin >> a;
        if (a == 1) {
            return 0;
        }
    }

    return 0;
}