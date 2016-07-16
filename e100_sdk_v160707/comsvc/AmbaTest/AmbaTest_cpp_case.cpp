#include <iostream>
/* Standard Template Library (STL) */
#include <vector>
#include <algorithm>

extern "C" {
#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaPrintk.h>
}

using namespace std;

#define myPrint(pFmt, ...) \
    AmbaPrintk(0, 0, 0, AMBA_PRINTK_MESSAGE_LEVEL,  \
               AMBA_PRINTK_CONSOLE_LEVEL, RED,   \
               pFmt, ##__VA_ARGS__)


#include <string>

void function1(void)
{
    AmbaPrint("string var using test 1  \n");
    AmbaKAL_TaskSleep(1000);
    int testType;

    string testTypeStr = "TEST"; // ---------------- memcpy() will crush here
    AmbaPrint("string var using test 2  \n");
    AmbaKAL_TaskSleep(1000);

    if (testTypeStr == "TEST") {
        testType = 0x11;
    } else {
        testType = 0x22;
    }

    AmbaPrint("string var using test 3  \n");
}


#define printk    AmbaPrint
#define printk_co AmbaPrintColor

void function2(unsigned long x)
{
    switch (x) {
    case 0xC0000015:
        char *endptr;

        char b6[] = "9.9500000476837e-001";
        printk("b6=%lf\n", strtod(b6, &endptr)); // ---------------------OK
        AmbaKAL_TaskSleep(1000);

        char b7[] = "9.95000004768371e-001";
        printk("b7=%lf\n", strtod(b7, &endptr)); // ---------------------OK
        AmbaKAL_TaskSleep(1000);

        char b8[] = "9.950000047683715e-001";
        printk("b8=%lf\n", strtod(b8, &endptr)); // ---------------- memcpy() will crush here
        AmbaKAL_TaskSleep(1000);
//        char c[] = "9.9500000476837158e-001";
        printk("test here1");
        AmbaKAL_TaskSleep(1000);
        break;
    }
}



class myClass
{
    public:
        myClass(int);
        double tst_exception(int a, int b);

    private:
        int i;
};

myClass::myClass(int x) : i(x) {}

double myClass::tst_exception(int a, int b)
{
    if (b == 0) {
#ifdef __EXCEPTIONS
        throw 13;
#else
        return 0;
#endif
    }

    return (a / b);
}

void test_stl_vector(void)
{
    int myArray[] = { 30, 12, 55, 31, 98, 11, 41, 80, 66, 21 };

    vector<int> myV(myArray, myArray + 10);


    myPrint("Push 17\r\n");
    myV.push_back(17);

    myPrint("Size: %d\r\n", myV.size());

    // Sort
    myPrint("Sort\r\n");
    sort(myV.begin(), myV.end());

    for (vector<int>::iterator it = myV.begin();
         it != myV.end();
         it++) {
        myPrint("%d", *it);
    }
    myPrint("\r\n");

    int search = 31;

    vector<int>::iterator it =
        find(myV.begin(), myV.end(), search);

    myPrint("Find %d...\r\n", search);
    if (it != myV.end()) {
        myPrint("Got it!\r\n");
    } else {
        myPrint("Can not find!\r\n");
    }

    myPrint("Pop last\r\n");
    myV.pop_back();

    // Reverse
    myPrint("Reverse sort\r\n");
    reverse(myV.begin(), myV.end());

    for (vector<int>::iterator it = myV.begin(); it != myV.end();
         it++) {
        myPrint("%d", *it);
    }
    myPrint("\r\n");
}

extern "C" int cpp_test(void);

int cpp_test(void)
{
    myPrint("%s ....\r\n", __func__);

    function1();
    function2(0xC0000015);

    myClass my_class(5);

    // dynamic
    myClass* my_class_ptr = new myClass(5);
    double d;

    __try {
        d = my_class_ptr->tst_exception(2, 0);
#ifdef __EXCEPTIONS
    } catch (int c) {
        myPrint("Exception int: %d ...\r\n", c);
    } catch (std::exception& e) {
        myPrint("Exception std ...\r\n");
#endif
    }

    delete my_class_ptr;

    // Test STL
    test_stl_vector();

    return 0;
}

