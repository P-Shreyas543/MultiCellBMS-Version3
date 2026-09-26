function dataTable = read_sd_card(source, varargin)
% READ_SD_CARD Read, decode, and export raw sectors from an SD card or binary dump.
%
%   dataTable = read_sd_card()
%   dataTable = read_sd_card(source)
%   dataTable = read_sd_card(source, 'ExportCSV', 'trip_log.csv', 'Plot', true)
%
% Inputs:
%   source - (Optional) Path to a binary dump file (e.g., 'sd_dump.bin'),
%            or a Windows PhysicalDrive string (e.g., '\\.\PhysicalDrive1'),
%            or 'demo' to generate and decode a simulated BMS trip log.
%            Default: 'demo' if no file specified.
%
% Name-Value Parameters:
%   'MaxSectors' - Maximum number of sectors to read (Default: 100).
%   'ExportCSV'  - Path to output CSV file (Default: 'BMS_Trip_Log_Export.csv').
%   'Plot'       - Boolean flag to generate telemetry plots (Default: true).
%
% Output:
%   dataTable    - MATLAB table containing parsed BMS telemetry records.
%
% Example:
%   % 1. Run immediate demo decoding
%   T = read_sd_card('demo');
%
%   % 2. Read from a dump file saved from hardware or Win32DiskImager
%   T = read_sd_card('bms_sd_dump.bin', 'ExportCSV', 'telemetry.csv');
%
%   % 3. Read directly from a connected USB SD card reader on Windows (Run MATLAB as Admin)
%   T = read_sd_card('\\.\PhysicalDrive1', 'MaxSectors', 50);

    p = inputParser;
    addOptional(p, 'source', 'demo', @(x) ischar(x) || isstring(x));
    addParameter(p, 'MaxSectors', 100, @isnumeric);
    addParameter(p, 'ExportCSV', 'BMS_Trip_Log_Export.csv', @(x) ischar(x) || isstring(x));
    addParameter(p, 'Plot', true, @islogical);
    parse(p, source, varargin{:});

    src = char(p.Results.source);
    maxSectors = p.Results.MaxSectors;
    csvFile = char(p.Results.ExportCSV);
    doPlot = p.Results.Plot;

    SECTOR_SIZE = 512;
    RECORD_SIZE = 32;
    RECORDS_PER_SECTOR = SECTOR_SIZE / RECORD_SIZE; % 16

    fprintf('=======================================================\n');
    fprintf('  MultiCell BMS - Raw SD Card Reader & Telemetry Decoder\n');
    fprintf('=======================================================\n');

    rawBytes = [];

    % Case 1: Demo / Test Mode
    if strcmpi(src, 'demo')
        fprintf('[+] Source: DEMO MODE (Generating synthetic BMS blackbox log)...\n');
        rawBytes = generate_demo_sd_image(8); % 8 sectors
    else
        fprintf('[+] Source: %s\n', src);
        if startsWith(src, '\\.\PhysicalDrive', 'IgnoreCase', true)
            fprintf('[!] Direct Physical Drive Access requested: %s\n', src);
            fprintf('    (Note: Direct raw drive access requires Administrator privileges on Windows)\n');
        end

        fid = fopen(src, 'r', 'l');
        if fid == -1
            error('read_sd_card:FileNotFound', ...
                ['Could not open source "%s".\n' ...
                 'If reading a physical drive, ensure MATLAB is run as Administrator.\n' ...
                 'Tip: You can test with read_sd_card(''demo'') anytime.'], src);
        end
        cleanFid = onCleanup(@() fclose(fid));
        
        bytesToRead = maxSectors * SECTOR_SIZE;
        rawBytes = fread(fid, bytesToRead, 'uint8=>uint8');
        fprintf('[+] Read %d bytes (%d sectors) from source.\n', length(rawBytes), floor(length(rawBytes) / SECTOR_SIZE));
    end

    totalSectors = floor(length(rawBytes) / SECTOR_SIZE);
    if totalSectors < 1
        error('read_sd_card:InvalidData', 'Data source contains less than one 512-byte sector.');
    end

    % -------------------------------------------------------------
    % Check Sector 0 Header
    % -------------------------------------------------------------
    sector0 = rawBytes(1:SECTOR_SIZE);
    headerAscii = char(sector0(1:min(64, SECTOR_SIZE))');
    fprintf('[+] Sector 0 Header Inspection: %s\n', strtrim(headerAscii(headerAscii >= 32 & headerAscii <= 126)));

    % -------------------------------------------------------------
    % Parse Telemetry Records (Starting from Sector 1 onwards)
    % -------------------------------------------------------------
    records = [];
    recordCount = 0;

    for s = 1:(totalSectors - 1)
        secStart = s * SECTOR_SIZE + 1;
        secEnd = secStart + SECTOR_SIZE - 1;
        sectorData = rawBytes(secStart:secEnd);

        for r = 0:(RECORDS_PER_SECTOR - 1)
            offset = r * RECORD_SIZE + 1;
            chunk = sectorData(offset:(offset + RECORD_SIZE - 1));

            % Check for blank / unwritten slot (all 0x00 or all 0xFF)
            if all(chunk == 0) || all(chunk == 255)
                continue;
            end

            % Decode 32-byte BMS Telemetry packet
            % Bytes 0..3: Unix timestamp (uint32)
            timestamp = typecast(chunk(1:4), 'uint32');
            
            % Bytes 4..5: Pack Voltage in mV (uint16)
            pack_v_mv = typecast(chunk(5:6), 'uint16');
            pack_v = double(pack_v_mv) / 1000.0;

            % Bytes 6..7: Pack Current in centi-Amps (int16)
            pack_i_ca = typecast(chunk(7:8), 'int16');
            pack_i = double(pack_i_ca) / 100.0;

            % Bytes 8..9: State of Charge (SOC) in permille (uint16)
            soc_permille = typecast(chunk(9:10), 'uint16');
            soc_pct = double(soc_permille) / 10.0;

            % Byte 10: Max Cell Temperature (int8)
            t_max = typecast(chunk(11), 'int8');

            % Byte 11: Min Cell Temperature (int8)
            t_min = typecast(chunk(12), 'int8');

            % Bytes 12..13: Max Cell Voltage in mV (uint16)
            cell_v_max_mv = typecast(chunk(13:14), 'uint16');
            cell_v_max = double(cell_v_max_mv) / 1000.0;

            % Bytes 14..15: Min Cell Voltage in mV (uint16)
            cell_v_min_mv = typecast(chunk(15:16), 'uint16');
            cell_v_min = double(cell_v_min_mv) / 1000.0;

            % Bytes 16..17: Fault Word (uint16)
            fault_word = typecast(chunk(17:18), 'uint16');

            % Bytes 18..19: Cycle Count (uint16)
            cycles = typecast(chunk(19:20), 'uint16');

            recordCount = recordCount + 1;
            records(recordCount).RecordID = recordCount; %#ok<AGROW>
            records(recordCount).SectorID = s;
            records(recordCount).SlotInSector = r;
            records(recordCount).Timestamp = double(timestamp);
            records(recordCount).PackVoltage_V = pack_v;
            records(recordCount).PackCurrent_A = pack_i;
            records(recordCount).SOC_Percent = soc_pct;
            records(recordCount).TempMax_C = double(t_max);
            records(recordCount).TempMin_C = double(t_min);
            records(recordCount).CellVMax_V = cell_v_max;
            records(recordCount).CellVMin_V = cell_v_min;
            records(recordCount).DeltaCell_mV = (cell_v_max - cell_v_min) * 1000.0;
            records(recordCount).FaultWord = sprintf('0x%04X', fault_word);
            records(recordCount).CycleCount = double(cycles);
        end
    end

    if recordCount == 0
        warning('read_sd_card:NoRecords', 'No valid telemetry records found in examined sectors.');
        dataTable = table();
        return;
    end

    dataTable = struct2table(records);
    fprintf('[+] Decoded %d BMS telemetry records across %d sectors.\n', recordCount, totalSectors);

    % Display first few rows
    disp(head(dataTable, min(5, recordCount)));

    % -------------------------------------------------------------
    % Export to CSV
    % -------------------------------------------------------------
    if ~isempty(csvFile)
        writetable(dataTable, csvFile);
        fprintf('[+] Successfully exported telemetry log to: %s\n', fullfile(pwd, csvFile));
    end

    % -------------------------------------------------------------
    % Optional Telemetry Plots
    % -------------------------------------------------------------
    if doPlot && recordCount > 1
        try
            fig = figure('Name', 'BMS SD Card Blackbox Telemetry', 'Color', 'w', ...
                         'Position', [150, 150, 950, 650]);
            
            t = (1:recordCount)';
            if range(dataTable.Timestamp) > 0
                t = (dataTable.Timestamp - dataTable.Timestamp(1));
                xLabelText = 'Time Elapsed (seconds)';
            else
                xLabelText = 'Sample Record Index';
            end

            % Subplot 1: Pack Voltage & Current
            subplot(3, 1, 1);
            yyaxis left
            plot(t, dataTable.PackVoltage_V, 'b-', 'LineWidth', 1.5);
            ylabel('Pack Voltage (V)');
            grid on;
            yyaxis right
            plot(t, dataTable.PackCurrent_A, 'r--', 'LineWidth', 1.2);
            ylabel('Pack Current (A)');
            title('BMS SD Card Telemetry: Pack Voltage & Current');
            xlabel(xLabelText);

            % Subplot 2: Cell Voltages & Imbalance
            subplot(3, 1, 2);
            plot(t, dataTable.CellVMax_V, 'g-', 'LineWidth', 1.2); hold on;
            plot(t, dataTable.CellVMin_V, 'm-', 'LineWidth', 1.2);
            ylabel('Cell Voltage (V)');
            legend('Max Cell', 'Min Cell', 'Location', 'best');
            grid on;
            title('Cell Balance & Spread');
            xlabel(xLabelText);

            % Subplot 3: Temperatures & SOC
            subplot(3, 1, 3);
            yyaxis left
            plot(t, dataTable.TempMax_C, 'Color', [0.85 0.33 0.1], 'LineWidth', 1.5); hold on;
            plot(t, dataTable.TempMin_C, 'Color', [0.93 0.69 0.13], 'LineWidth', 1.2);
            ylabel('Temperature (C)');
            yyaxis right
            plot(t, dataTable.SOC_Percent, 'k-', 'LineWidth', 1.5);
            ylabel('SOC (%)');
            grid on;
            legend('T_{max}', 'T_{min}', 'SOC', 'Location', 'best');
            title('Thermal & State of Charge Monitoring');
            xlabel(xLabelText);

            drawnow;
            fprintf('[+] Generated interactive BMS telemetry plot.\n');
        catch plotErr
            fprintf('[!] Note: Could not generate figure: %s\n', plotErr.message);
        end
    end
    fprintf('=======================================================\n');
end

% Helper function to synthesize a realistic SD card image for demo/testing
function raw = generate_demo_sd_image(numSectors)
    SECTOR_SIZE = 512;
    RECORD_SIZE = 32;
    raw = zeros(numSectors * SECTOR_SIZE, 1, 'uint8');

    % Sector 0 Header
    hdr = 'MULTICELL_BMS_V3:TRIP_RECORDER_BLOCK_0';
    raw(1:length(hdr)) = uint8(hdr);

    baseTimestamp = uint32(1727337600); % Base Unix timestamp
    baseVoltageMV = uint16(48200);      % 48.2 V

    recIdx = 0;
    for s = 1:(numSectors - 1)
        for r = 0:(SECTOR_SIZE / RECORD_SIZE - 1)
            recIdx = recIdx + 1;
            offset = s * SECTOR_SIZE + r * RECORD_SIZE + 1;

            ts = baseTimestamp + uint32(recIdx);
            v_mv = uint16(double(baseVoltageMV) - 200.0 * sin(recIdx / 5.0));
            i_ca = int16(-2500 + 500 * sin(recIdx / 3.0)); % ~ -25 A
            soc_perm = uint16(950 - recIdx);               % 95.0% discharging
            t_max = int8(32 + floor(recIdx / 10));
            t_min = int8(27 + floor(recIdx / 15));
            cv_max = uint16(4180 - floor(recIdx / 2));
            cv_min = uint16(4140 - floor(recIdx / 2));
            faults = uint16(0);
            cycles = uint16(128);

            chunk = zeros(RECORD_SIZE, 1, 'uint8');
            chunk(1:4) = typecast(ts, 'uint8');
            chunk(5:6) = typecast(v_mv, 'uint8');
            chunk(7:8) = typecast(i_ca, 'uint8');
            chunk(9:10) = typecast(soc_perm, 'uint8');
            chunk(11) = typecast(t_max, 'uint8');
            chunk(12) = typecast(t_min, 'uint8');
            chunk(13:14) = typecast(cv_max, 'uint8');
            chunk(15:16) = typecast(cv_min, 'uint8');
            chunk(17:18) = typecast(faults, 'uint8');
            chunk(19:20) = typecast(cycles, 'uint8');

            raw(offset:(offset + RECORD_SIZE - 1)) = chunk;
        end
    end
end
