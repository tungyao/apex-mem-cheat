#include "d3dguix.h"

void TestLoop() {
    while (1) {
        int b[20][4] = {0};
        for (int i = 0; i < 20; i++) {
            const int x[4] = {rand() % 1000, rand() % 400, 50, 100};
            Sleep(1);
            memcpy(b[i], x, sizeof(x));
        }
        Box_T box;
        memcpy(box.data, b, sizeof(b));
        pushBox(box);
        cout << "22222\n" << endl;
        Sleep(100);
    }
}

int main() {
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) Start, NULL, 0, NULL);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) TestLoop, NULL, 0, NULL);
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
