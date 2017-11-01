function mac = MACAddress(allMac)
% mac = MACAddress()
% 
% The default is to return one MAC address, likely for ethernet adaptor. If the
% optional input is provided and true, all MAC address are returned in cellstr.
% No internet connection is required for this to work.
% 
% Examples:
%  mac = MACAddress(); % return 1st MAC in string
% The format is like F0-4D-A2-DB-00-37 for Windows, f0:4d:a2:db:00:37 otherwise.
% 
%  macs = MACAddress(1); % return all MAC on the computer
% The output is cell even if only one MAC found.
% 
% To get numeric:
%  num = uint8(sscanf(MACAddress, '%2x%*c', 6))';

% 170510 Adapted this from RTBox code (Xiangrui.Li at gmail.com).
% 170525 Include mex for MS Windows.
% 171030 mex.c more robust. Include Octave 4 mex.

if nargin<1 || isempty(allMac), allMac = false; end % default to first MAC

try % first try mex version if it works
    mac = MACAddress_mex(allMac);
    return;
end

if ispc, fmt = '%02X-%02X-%02X-%02X-%02X-%02X'; % adopt OS format preference
else,    fmt = '%02x:%02x:%02x:%02x:%02x:%02x';
end

if ~allMac 
    try %#ok<*TRYNC> if Psychtoolbox is available, it takes almost no time
        a = Screen('computer'); 
        mac = a.MACAddress; % works for OSX/Linux for now
        if ~isempty(mac) && sum(mac=='0')<12, return; end
    end
    if exist('/sys/class/net/eth0/address', 'file') % linux
        [~, mac] = system('cat /sys/class/net/eth0/address');
        if ~isempty(mac) && sum(mac=='0')<12, return; end
    end
%     try % python uuid.getnode is fast, but 1st py.* call is slow
%         a = int64(py.uuid.getnode());
%         a = typecast(a, 'uint8'); % LE for Matlab
%         mac = sprintf(fmt, a(6:-1:1));
%         return;
%     end
end

try % java approach is faster than system command, and is OS independent
    if allMac, mac = {}; end
    ni = java.net.NetworkInterface.getNetworkInterfaces;
    while ni.hasMoreElements
        a = ni.nextElement.getHardwareAddress;
        if numel(a)~=6 || all(a==0), continue; end % not valid mac
        a = typecast(a, 'uint8'); % from int8
        a = sprintf(fmt, a);
        if allMac, mac{end+1} = a; %#ok
        else, mac = a; return; % done after finding 1st
        end
    end
    if ~isempty(mac), return; end
end

try % system command is slow, use as last resort
    if ispc
        [err, str] = system('getmac.exe');
        expr = '(?<=\s)([0-9A-F]{2}-){5}[0-9A-F]{2}(?=\s)'; % separator -
    else
        [err, str] = system('ifconfig');
        expr = '(?<=\s)([0-9a-f]{2}:){5}[0-9a-f]{2}(?=\s)'; % separator :
    end
    if err, error(str); end % unlikely to happen
    if allMac, mac = regexp(str, expr, 'match');
    else,      mac = regexp(str, expr, 'match', 'once');
    end
    if ~isempty(mac), return; end
end

% If all attemps fail, give warning, and return a random MAC
warning('MACAddress:RandomMAC', 'Returned MAC are random numbers');
a = randi(255, [1 6], 'uint8');
a(5) = bitset(a(5), 1); % set 8th bit for random MAC, likely meaningless
mac = sprintf(fmt, a);
if allMac, mac = {mac}; end
