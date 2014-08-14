// Build command:
// g++  -std=gnu++0x -fPIC -g -Wall -march=armv6 -mfpu=vfp -mfloat-abi=hard -isystem /opt/vc/include/ -isystem /opt/vc/include/interface/vcos/pthreads/ -isystem /opt/vc/include/interface/vmcs_host/linux/ -I/usr/local/include -L /opt/vc/lib -lcec -lbcm_host cec-simplest.cpp -o cec-simplest

//#CXXFLAGS=-I/usr/local/include
//#LINKFLAGS=-lcec
#include <libcec/cec.h>

// cecloader.h uses std::cout _without_ including iosfwd or iostream
// Furthermore is uses cout and not std::cout
#include <iostream>
using std::cout;
using std::endl;
#include <libcec/cecloader.h>

#include "bcm_host.h"
//#LINKFLAGS=-lbcm_host

#include <algorithm>  // for std::min

// The main loop will just continue until a ctrl-C is received
#include <signal.h>
bool exit_now = false;
void handle_signal(int signal)
{
    exit_now = true;
}


//CEC::CBCecKeyPressType 
int on_keypress(void* not_used, const CEC::cec_keypress msg)
{
    std::string key;
    switch( msg.keycode )
    {
        case CEC::CEC_USER_CONTROL_CODE_SELECT: { key = "select"; break; }
        case CEC::CEC_USER_CONTROL_CODE_UP: { key = "up"; break; }
        case CEC::CEC_USER_CONTROL_CODE_DOWN: { key = "down"; break; }
        case CEC::CEC_USER_CONTROL_CODE_LEFT: { key = "left"; break; }
        case CEC::CEC_USER_CONTROL_CODE_RIGHT: { key = "right"; break; }
        default: break;
    };

    std::cout << "on_keypress: " << static_cast<int>(msg.keycode) << " " << key << std::endl;
    return 0;
}


int main(int argc, char* argv[])
{
    // Install the ctrl-C signal handler
    if( SIG_ERR == signal(SIGINT, handle_signal) )
    {
        std::cerr << "Failed to install the SIGINT signal handler\n";
        return 1;
    }

    // Initialise the graphics pipeline for the raspberry pi. Yes, this is necessary.
    bcm_host_init();

    // Set up the CEC config and specify the keypress callback function
    CEC::ICECCallbacks        cec_callbacks;
    CEC::libcec_configuration cec_config;
    cec_config.Clear();
    cec_callbacks.Clear();

    const std::string devicename("CECExample");
    devicename.copy(cec_config.strDeviceName, std::min(devicename.size(),13u) );
    
    cec_config.clientVersion       = CEC::CEC_CLIENT_VERSION_CURRENT;
    cec_config.bActivateSource     = 0;
    cec_config.callbacks           = &cec_callbacks;
    cec_config.deviceTypes.Add(CEC::CEC_DEVICE_TYPE_RECORDING_DEVICE);

    cec_callbacks.CBCecKeyPress    = &on_keypress;

    // Get a cec adapter by initialising the cec library
    CEC::ICECAdapter* cec_adapter = LibCecInitialise(&cec_config);
    if( !cec_adapter )
    { 
        std::cerr << "Failed loading libcec.so\n"; 
        return 1; 
    }

    // Try to automatically determine the CEC devices 
    CEC::cec_adapter devices[10];
    int8_t devices_found = cec_adapter->FindAdapters(devices, 10, NULL);
    if( devices_found <= 0)
    {
        std::cerr << "Could not automatically determine the cec adapter devices\n";
        UnloadLibCec(cec_adapter);
        return 1;
    }

    // Open a connection to the zeroth CEC device
    if( !cec_adapter->Open(devices[0].comm) )
    {        
        std::cerr << "Failed to open the CEC device on port " << devices[0].comm << std::endl;
        UnloadLibCec(cec_adapter);
        return 1;
    }

    // Loop until ctrl-C occurs
    while( !exit_now )
    {
        // nothing to do.  All happens in the CEC callback on another thread
        sleep(1);
    }

    // Close down and cleanup
    cec_adapter->Close();
    UnloadLibCec(cec_adapter);

    return 0;
}

