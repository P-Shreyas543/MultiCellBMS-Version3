function status = write_sd_string(text, sector_id, offset)
% WRITE_SD_STRING Store an ASCII string into the SD card and read it back.
%
%   status = write_sd_string('Hello Shareys P')
%   status = write_sd_string('Hello Shareys P', 1, 0)
%
% Inputs:
%   text      - String to write (Default: 'Hello Shareys P')
%   sector_id - Sector index (Default: 1)
%   offset    - Byte offset in sector (Default: 0)

    if nargin < 1 || isempty(text)
        text = 'Hello Shareys P';
    end
    if nargin < 2 || isempty(sector_id)
        sector_id = 1;
    end
    if nargin < 3 || isempty(offset)
        offset = 0;
    end

    fprintf('=======================================================\n');
    fprintf('  SD Card String Storage Test: "%s"\n', text);
    fprintf('=======================================================\n');

    strBytes = uint8(text);
    strLen = length(strBytes);
    
    if (offset + strLen) > 512
        error('write_sd_string:ExceedsSector', 'Text length + offset exceeds 512 bytes.');
    end

    payload32 = zeros(1, 32, 'uint8');
    copyLen = min(strLen, 32);
    payload32(1:copyLen) = strBytes(1:copyLen);

    fprintf('[+] Sector Target : Sector %d (Byte Offset %d)\n', sector_id, offset);
    fprintf('[+] Raw ASCII     : [ ');
    fprintf('%d ', payload32(1:copyLen));
    fprintf(']\n');
    fprintf('[+] Hex Dump      : [ ');
    fprintf('0x%02X ', payload32(1:copyLen));
    fprintf(']\n');

    dumpFile = 'sd_dump.bin';
    SECTOR_SIZE = 512;
    numSectors = max(8, sector_id + 1);

    if exist(dumpFile, 'file')
        fid = fopen(dumpFile, 'r+b');
        if fid == -1, fid = fopen(dumpFile, 'wb'); end
    else
        fid = fopen(dumpFile, 'w+b');
    end

    if fid ~= -1
        fseek(fid, 0, 'eof');
        curBytes = ftell(fid);
        neededBytes = numSectors * SECTOR_SIZE;
        if curBytes < neededBytes
            padding = zeros(neededBytes - curBytes, 1, 'uint8');
            fwrite(fid, padding, 'uint8');
        end

        fseek(fid, sector_id * SECTOR_SIZE + offset, 'bof');
        fwrite(fid, strBytes, 'uint8');

        fseek(fid, sector_id * SECTOR_SIZE + offset, 'bof');
        readBack = fread(fid, strLen, 'uint8=>uint8')';
        fclose(fid);

        readBackStr = char(readBack);
        fprintf('[+] Readback Test : "%s"\n', readBackStr);

        if strcmp(text, readBackStr)
            fprintf('[+] VERIFICATION  : PASSED 100%% (Stored & Verified in %s)\n', dumpFile);
            status = 0;
        else
            fprintf('[!] VERIFICATION  : FAILED (Readback mismatch)\n');
            status = 1;
        end
    else
        fprintf('[!] Error opening %s\n', dumpFile);
        status = -1;
    end
    fprintf('=======================================================\n');
end
