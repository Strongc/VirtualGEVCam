#include "VirtualGEVCam.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    VirtualGEVCam VtCam(".\\InData", "virtual-camera.zip", "VirtualDevice.ini");

    if (VtCam.Init() != MV_OK)
    {
        cout << "VirtualGEVCam init error" << endl;
        return -1;
    }

    if (VtCam.Starting() != MV_OK)
    {
        cout << "VirtualGEVCam starting error" << endl;
        return -1;
    }

    VtCam.DeInit();

    return 0;
}
