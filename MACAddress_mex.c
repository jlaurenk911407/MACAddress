/* Windows version of C code to get MAC address.
 * To compile in Matlab/Octave:
 *  mex -liphlpapi MACAddress_mex.c
 * will create MACAddress_mex.mex* for the corresponding language. 
 * The mex file is called by MACAddress.m.
 *  MACAddress_mex() returns the first MAC address as char;
 *  MACAddress_mex(1) returns all MAC address as cellstr.
 *  
 * 170617 Wrote it by Xiangrui.Li at gmail.com 
 * 171029 Remove NICs limit. Make it work for Octave
 */

#include <mex.h>
#include <winsock2.h>
#include <IPHlpApi.h>

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) 
{
    const char fmt[] = "%02X-%02X-%02X-%02X-%02X-%02X"; // Windows style
    //const char fmt[] = "%02x:%02x:%02x:%02x:%02x:%02x"; // Unix style
    unsigned char m[7];
    char ch18[18], i=0;
    mwSize nNICs = 1;
    DWORD bufLen = sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO pAdapter = (IP_ADAPTER_INFO *) mxMalloc(bufLen);    
    BOOL allMAC = FALSE;    
    if (nrhs>0) allMAC = (BOOL) mxGetScalar(prhs[0]);
    
    // if bufLen not enough, GetAdaptersInfo will update it
    while (GetAdaptersInfo(pAdapter, &bufLen) == ERROR_BUFFER_OVERFLOW) {
        mxFree(pAdapter);
        pAdapter = (IP_ADAPTER_INFO *) mxMalloc(bufLen); // increased buf
    }
    nNICs = (mwSize)((float)bufLen / sizeof(IP_ADAPTER_INFO) + 0.4);
    if (allMAC) plhs[0] = mxCreateCellMatrix(1, nNICs);
    
    for (i=0; i<nNICs && pAdapter; i++) {
        memcpy(m, pAdapter->Address, 6);
        sprintf(ch18, fmt, m[0], m[1], m[2], m[3], m[4], m[5]); 
        if (allMAC) mxSetCell(plhs[0], i, mxCreateString(ch18));
        else {plhs[0] = mxCreateString(ch18); break;}
		pAdapter = pAdapter->Next;
	}
    if (pAdapter) mxFree(pAdapter);
}
