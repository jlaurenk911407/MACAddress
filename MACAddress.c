/* fetch physical address (MAC) and ip address */
#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <tchar.h>
#include <lm.h>
#include "mex.h"
    
/* the gateway function */
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    IPGlobalProperties computerProperties = IPGlobalProperties.GetIPGlobalProperties();
    NetworkInterface[] nics = NetworkInterface.GetAllNetworkInterfaces();
    if (nics == null || nics.Length < 1)
    {
        mexErrMsgTxt("  No network interfaces found.");
        return;
    }

    foreach (NetworkInterface adapter in nics)
    {
        PhysicalAddress address = adapter.GetPhysicalAddress();
        byte[] bytes = address.GetAddressBytes();
        for(int i = 0; i< bytes.Length; i++)
        {
            // Display the physical address in hexadecimal.
            mexPrintf("%2s:", bytes[i].ToString("X2"));
            // Insert a hyphen after each byte, unless we are at the end of the 
            // address.
            if (i != bytes.Length -1)
            {
                 mexPrintf("-");
            }
        }
    }
}
  