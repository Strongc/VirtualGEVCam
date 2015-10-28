// -*- coding: gb2312-dos -*-
#include "VirtualDevice.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    VirtualDevice VtlDev(".\\InData", "virtual-camera.zip", "VirtualDevice.ini");

    if (VtlDev.Init() != MV_OK)
    {
        cout << "VirtualDevice init error" << endl;
        return -1;
    }

    if (VtlDev.Starting() != MV_OK)
    {
        cout << "VirtualDevice starting error" << endl;
        return -1;
    }

    VtlDev.DeInit();

    return 0;
}
