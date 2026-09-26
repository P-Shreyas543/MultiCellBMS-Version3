%% create_sd_blocks.m
% Automates the creation of industry-standard SPI SD Card Reader C Function blocks
% for BMS Blackbox Trip Logging and Crash Data Recording.

function create_sd_blocks()
    disp('=== Setting up Industry-Standard SPI SD Card Reader Blocks ===');
    
    model_name = 'SD_Card_SPI_Demo';
    
    % Close if already open
    if bdIsLoaded(model_name)
        close_system(model_name, 0);
    end
    
    % Create new demo model
    new_system(model_name);
    load_system(model_name);
    
    % Configure Model Custom Code
    set_param(model_name, 'SimCustomHeaderCode', '#include "sd_spi.h"');
    set_param(model_name, 'SimCustomSourceCode', '#include "sd_spi.c"');
    set_param(model_name, 'CustomHeaderCode', '#include "sd_spi.h"');
    set_param(model_name, 'CustomSourceCode', '#include "sd_spi.c"');
    set_param(model_name, 'StopTime', '2.0');
    set_param(model_name, 'SolverType', 'Fixed-step');
    set_param(model_name, 'FixedStep', '0.1');

    function apply_block_config(b_path)
        set_param(b_path, 'CustomCodeSettingLocation', 'BlockSettings');
        set_param(b_path, 'CustomCodeIsMultiInstantiable', 'on');
        set_param(b_path, 'SimCustomHeaderFile', 'sd_spi.h');
        set_param(b_path, 'SimCustomSourceFile', 'sd_spi.c');
        set_param(b_path, 'CustomHeaderFile', 'sd_spi.h');
        set_param(b_path, 'CustomSourceFile', 'sd_spi.c');
    end

    % -------------------------------------------------------------
    % 1. Create SD_Init Block
    % -------------------------------------------------------------
    b_init = [model_name, '/SD_Init'];
    add_block('simulink/User-Defined Functions/C Function', b_init, ...
        'Position', [250, 40, 420, 130]);
    apply_block_config(b_init);
    
    spec_in = get_param(b_init, 'SymbolSpec');
    s = spec_in.addSymbol('card_type');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_in.addSymbol('status');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    set_param(b_init, 'OutputCode', sprintf([...
        '/* Initialize SD Card over SPI */\n' ...
        'status = SD_Init();\n' ...
        'card_type = SD_GetCardType();\n']));

    % -------------------------------------------------------------
    % 2. Create SD_ReadSector Block (Full 512-Byte Sector)
    % -------------------------------------------------------------
    b_read = [model_name, '/SD_ReadSector'];
    add_block('simulink/User-Defined Functions/C Function', b_read, ...
        'Position', [250, 180, 420, 280]);
    apply_block_config(b_read);
    
    spec_rd = get_param(b_read, 'SymbolSpec');
    s = spec_rd.addSymbol('sector_id');
    s.Scope = 'Input'; s.Type = 'uint32'; s.Size = '1';
    
    s = spec_rd.addSymbol('data_512');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '512';
    
    s = spec_rd.addSymbol('status');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    set_param(b_read, 'OutputCode', sprintf([...
        '/* Read Single 512-Byte Physical Sector */\n' ...
        'status = SD_ReadSector(sector_id, data_512);\n']));

    % -------------------------------------------------------------
    % 3. Create SD_WriteSector Block (Full 512-Byte Sector)
    % -------------------------------------------------------------
    b_write = [model_name, '/SD_WriteSector'];
    add_block('simulink/User-Defined Functions/C Function', b_write, ...
        'Position', [250, 330, 420, 430]);
    apply_block_config(b_write);
    
    spec_wr = get_param(b_write, 'SymbolSpec');
    s = spec_wr.addSymbol('sector_id');
    s.Scope = 'Input'; s.Type = 'uint32'; s.Size = '1';
    
    s = spec_wr.addSymbol('data_512');
    s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '512';
    
    s = spec_wr.addSymbol('status');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    set_param(b_write, 'OutputCode', sprintf([...
        '/* Write Single 512-Byte Physical Sector */\n' ...
        'status = SD_WriteSector(sector_id, data_512);\n']));

    % -------------------------------------------------------------
    % 4. Create SD_ReadPayload Block (Compact 32-Byte Telemetry)
    % -------------------------------------------------------------
    b_pay = [model_name, '/SD_ReadPayload'];
    add_block('simulink/User-Defined Functions/C Function', b_pay, ...
        'Position', [250, 480, 420, 600]);
    apply_block_config(b_pay);
    
    spec_pl = get_param(b_pay, 'SymbolSpec');
    s = spec_pl.addSymbol('sector_id');
    s.Scope = 'Input'; s.Type = 'uint32'; s.Size = '1';
    
    s = spec_pl.addSymbol('offset');
    s.Scope = 'Input'; s.Type = 'uint16'; s.Size = '1';
    
    s = spec_pl.addSymbol('length');
    s.Scope = 'Input'; s.Type = 'uint16'; s.Size = '1';
    
    s = spec_pl.addSymbol('payload_32');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '32';
    
    s = spec_pl.addSymbol('status');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    set_param(b_pay, 'OutputCode', sprintf([...
        '/* Read Partial Payload from Sector (e.g. 32-Byte BMS Record) */\n' ...
        'status = SD_ReadPayload(sector_id, offset, payload_32, length);\n']));

    % -------------------------------------------------------------
    % 5. Create SD_SetInstance Block (Configurable LPSPI Instance)
    % -------------------------------------------------------------
    b_inst = [model_name, '/SD_SetInstance'];
    add_block('simulink/User-Defined Functions/C Function', b_inst, ...
        'Position', [250, 650, 420, 710]);
    apply_block_config(b_inst);
    
    spec_is = get_param(b_inst, 'SymbolSpec');
    s = spec_is.addSymbol('instance');
    s.Scope = 'Input'; s.Type = 'uint32'; s.Size = '1';
    
    set_param(b_inst, 'OutputCode', sprintf([...
        '/* Configure LPSPI Instance (0 = LPSPI0, 1 = LPSPI1, 2 = LPSPI2) */\n' ...
        'SD_SetSPIInstance(instance);\n']));

    % -------------------------------------------------------------
    % 6. Add Test Inputs and Displays
    % -------------------------------------------------------------
    % Displays for SD_Init
    add_block('simulink/Sinks/Display', [model_name, '/Card_Type'], ...
        'Position', [500, 45, 600, 75]);
    add_block('simulink/Sinks/Display', [model_name, '/Init_Status'], ...
        'Position', [500, 85, 600, 115]);
    add_line(model_name, 'SD_Init/1', 'Card_Type/1');
    add_line(model_name, 'SD_Init/2', 'Init_Status/1');
    
    % Input & Displays for SD_ReadSector
    add_block('simulink/Sources/Constant', [model_name, '/Read_Sector_ID'], ...
        'Position', [60, 220, 160, 245], 'Value', 'uint32(0)', 'OutDataTypeStr', 'uint32');
    add_line(model_name, 'Read_Sector_ID/1', 'SD_ReadSector/1');
    
    add_block('simulink/Sinks/Display', [model_name, '/Read_Status'], ...
        'Position', [500, 240, 600, 265]);
    add_line(model_name, 'SD_ReadSector/2', 'Read_Status/1');

    % Input & Displays for SD_WriteSector
    add_block('simulink/Sources/Constant', [model_name, '/Write_Sector_ID'], ...
        'Position', [60, 340, 160, 365], 'Value', 'uint32(1)', 'OutDataTypeStr', 'uint32');
    add_block('simulink/Sources/Constant', [model_name, '/Write_Data_512'], ...
        'Position', [60, 380, 160, 405], 'Value', 'uint8(repmat(1:32, 1, 16))', 'OutDataTypeStr', 'uint8');
    add_line(model_name, 'Write_Sector_ID/1', 'SD_WriteSector/1');
    add_line(model_name, 'Write_Data_512/1', 'SD_WriteSector/2');
    
    add_block('simulink/Sinks/Display', [model_name, '/Write_Status'], ...
        'Position', [500, 370, 600, 395]);
    add_line(model_name, 'SD_WriteSector/1', 'Write_Status/1');

    % Inputs & Displays for SD_ReadPayload (Reads 32-byte header from Sector 0)
    add_block('simulink/Sources/Constant', [model_name, '/Payload_Sector_ID'], ...
        'Position', [60, 490, 160, 510], 'Value', 'uint32(0)', 'OutDataTypeStr', 'uint32');
    add_block('simulink/Sources/Constant', [model_name, '/Payload_Offset'], ...
        'Position', [60, 525, 160, 545], 'Value', 'uint16(0)', 'OutDataTypeStr', 'uint16');
    add_block('simulink/Sources/Constant', [model_name, '/Payload_Length'], ...
        'Position', [60, 560, 160, 580], 'Value', 'uint16(32)', 'OutDataTypeStr', 'uint16');
    
    add_line(model_name, 'Payload_Sector_ID/1', 'SD_ReadPayload/1');
    add_line(model_name, 'Payload_Offset/1', 'SD_ReadPayload/2');
    add_line(model_name, 'Payload_Length/1', 'SD_ReadPayload/3');
    
    add_block('simulink/Sinks/Display', [model_name, '/Payload_32B_Display'], ...
        'Position', [500, 480, 720, 550]);
    add_block('simulink/Sinks/Display', [model_name, '/Payload_Status'], ...
        'Position', [500, 565, 600, 590]);
    add_line(model_name, 'SD_ReadPayload/1', 'Payload_32B_Display/1');
    add_line(model_name, 'SD_ReadPayload/2', 'Payload_Status/1');

    % Input for LPSPI Instance (default 0 for LPSPI0)
    add_block('simulink/Sources/Constant', [model_name, '/LPSPI_Instance'], ...
        'Position', [60, 665, 160, 695], 'Value', 'uint32(0)', 'OutDataTypeStr', 'uint32');
    add_line(model_name, 'LPSPI_Instance/1', 'SD_SetInstance/1');

    % Save model into models/ directory
    save_path = fullfile(fileparts(mfilename('fullpath')), '..', 'models', [model_name, '.slx']);
    save_system(model_name, save_path);
    disp(['Demo model successfully created: ', save_path]);
    close_system(model_name, 0);
    disp('=== Setup Complete ===');
end
