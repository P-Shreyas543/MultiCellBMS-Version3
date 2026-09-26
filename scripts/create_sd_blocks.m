%% create_sd_blocks.m
% Automates the creation of 100% feature-complete SPI SD Card Reader C Function blocks
% for BMS Blackbox Trip Logging, Crash Data Recording, Card Identification, Capacity Query,
% Sector Erasing, and High-Throughput Burst Streaming.

function create_sd_blocks()
    disp('=== Setting up 100% Complete SPI SD Card Reader Simulink Model ===');
    
    model_name = 'SD_Card_SPI_Demo';
    script_dir = fileparts(mfilename('fullpath'));
    save_path = fullfile(script_dir, '..', 'models', [model_name, '.slx']);
    
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

    % =============================================================
    % 1. BLOCK: SD_Init (SPI Card Initialization)
    % =============================================================
    b_init = [model_name, '/SD_Init'];
    add_block('simulink/User-Defined Functions/C Function', b_init, ...
        'Position', [250, 40, 420, 120]);
    apply_block_config(b_init);
    spec_in = get_param(b_init, 'SymbolSpec');
    s = spec_in.addSymbol('card_type'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    s = spec_in.addSymbol('status');    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_init, 'OutputCode', sprintf([...
        '/* Initialize SD Card over SPI (CMD0 -> CMD8 -> ACMD41) */\n' ...
        'status = SD_Init();\n' ...
        'card_type = SD_GetCardType();\n']));

    add_block('simulink/Sinks/Display', [model_name, '/Card_Type'], 'Position', [500, 45, 600, 70]);
    add_block('simulink/Sinks/Display', [model_name, '/Init_Status'], 'Position', [500, 80, 600, 105]);
    add_line(model_name, 'SD_Init/1', 'Card_Type/1');
    add_line(model_name, 'SD_Init/2', 'Init_Status/1');

    % =============================================================
    % 2. BLOCK: SD_GetCapacity (CSD Register Query & Math)
    % =============================================================
    b_cap = [model_name, '/SD_GetCapacity'];
    add_block('simulink/User-Defined Functions/C Function', b_cap, ...
        'Position', [250, 160, 420, 260]);
    apply_block_config(b_cap);
    spec_cp = get_param(b_cap, 'SymbolSpec');
    s = spec_cp.addSymbol('total_sectors'); s.Scope = 'Output'; s.Type = 'uint32'; s.Size = '1';
    s = spec_cp.addSymbol('capacity_MB');   s.Scope = 'Output'; s.Type = 'uint32'; s.Size = '1';
    s = spec_cp.addSymbol('status');        s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(b_cap, 'OutputCode', sprintf([...
        '/* Query Card Capacity via CSD Register (CMD9) */\n' ...
        'status = SD_GetSectorCount(&total_sectors);\n' ...
        'capacity_MB = (uint32_t)(((uint64_t)total_sectors * 512) / (1024 * 1024));\n']));

    add_block('simulink/Sinks/Display', [model_name, '/Total_Sectors'], 'Position', [500, 165, 620, 190]);
    add_block('simulink/Sinks/Display', [model_name, '/Capacity_MB'],   'Position', [500, 200, 620, 225]);
    add_block('simulink/Sinks/Display', [model_name, '/CSD_Status'],    'Position', [500, 235, 620, 260]);
    add_line(model_name, 'SD_GetCapacity/1', 'Total_Sectors/1');
    add_line(model_name, 'SD_GetCapacity/2', 'Capacity_MB/1');
    add_line(model_name, 'SD_GetCapacity/3', 'CSD_Status/1');

    % =============================================================
    % 3. BLOCK: SD_GetCID (Card Identification: MID & Serial)
    % =============================================================
    b_cid = [model_name, '/SD_GetCID'];
    add_block('simulink/User-Defined Functions/C Function', b_cid, ...
        'Position', [250, 300, 420, 400]);
    apply_block_config(b_cid);
    spec_ci = get_param(b_cid, 'SymbolSpec');
    s = spec_ci.addSymbol('mid');           s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    s = spec_ci.addSymbol('serial_num');    s.Scope = 'Output'; s.Type = 'uint32'; s.Size = '1';
    s = spec_ci.addSymbol('status');        s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(b_cid, 'OutputCode', sprintf([...
        '/* Query Card Identification Register CID (CMD10) */\n' ...
        'sd_cid_t cid;\n' ...
        'status = SD_GetCardCID(&cid);\n' ...
        'mid = cid.manufacturer_id;\n' ...
        'serial_num = cid.serial_number;\n']));

    add_block('simulink/Sinks/Display', [model_name, '/Manufacturer_ID'], 'Position', [500, 305, 620, 330]);
    add_block('simulink/Sinks/Display', [model_name, '/Serial_Number'],   'Position', [500, 340, 620, 365]);
    add_block('simulink/Sinks/Display', [model_name, '/CID_Status'],      'Position', [500, 375, 620, 400]);
    add_line(model_name, 'SD_GetCID/1', 'Manufacturer_ID/1');
    add_line(model_name, 'SD_GetCID/2', 'Serial_Number/1');
    add_line(model_name, 'SD_GetCID/3', 'CID_Status/1');

    % =============================================================
    % 4. BLOCK: SD_GetCardStatus (Query 16-bit Status CMD13)
    % =============================================================
    b_stat = [model_name, '/SD_GetCardStatus'];
    add_block('simulink/User-Defined Functions/C Function', b_stat, ...
        'Position', [250, 440, 420, 520]);
    apply_block_config(b_stat);
    spec_st = get_param(b_stat, 'SymbolSpec');
    s = spec_st.addSymbol('card_status_reg'); s.Scope = 'Output'; s.Type = 'uint16'; s.Size = '1';
    s = spec_st.addSymbol('status');          s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(b_stat, 'OutputCode', sprintf([...
        '/* Query SD Status Register (CMD13) */\n' ...
        'status = SD_GetCardStatus(&card_status_reg);\n']));

    add_block('simulink/Sinks/Display', [model_name, '/Card_Status_Register'], 'Position', [500, 445, 620, 470]);
    add_block('simulink/Sinks/Display', [model_name, '/Status_Cmd_Result'],     'Position', [500, 485, 620, 510]);
    add_line(model_name, 'SD_GetCardStatus/1', 'Card_Status_Register/1');
    add_line(model_name, 'SD_GetCardStatus/2', 'Status_Cmd_Result/1');

    % =============================================================
    % 5. BLOCK: SD_ReadSector (Single 512-Byte Physical Sector)
    % =============================================================
    b_read = [model_name, '/SD_ReadSector'];
    add_block('simulink/User-Defined Functions/C Function', b_read, ...
        'Position', [250, 560, 420, 650]);
    apply_block_config(b_read);
    spec_rd = get_param(b_read, 'SymbolSpec');
    s = spec_rd.addSymbol('sector_id'); s.Scope = 'Input';  s.Type = 'uint32'; s.Size = '1';
    s = spec_rd.addSymbol('data_512');  s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '512';
    s = spec_rd.addSymbol('status');    s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(b_read, 'OutputCode', sprintf([...
        '/* Read Single 512-Byte Physical Sector (CMD17) */\n' ...
        'status = SD_ReadSector(sector_id, data_512);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/Read_Sector_ID'], ...
        'Position', [60, 595, 160, 620], 'Value', 'uint32(0)', 'OutDataTypeStr', 'uint32');
    add_line(model_name, 'Read_Sector_ID/1', 'SD_ReadSector/1');

    add_block('simulink/Sinks/Display', [model_name, '/Read_Status'], 'Position', [500, 615, 600, 640]);
    add_line(model_name, 'SD_ReadSector/2', 'Read_Status/1');

    % =============================================================
    % 6. BLOCK: SD_WriteSector (Single 512-Byte Physical Sector)
    % =============================================================
    b_write = [model_name, '/SD_WriteSector'];
    add_block('simulink/User-Defined Functions/C Function', b_write, ...
        'Position', [250, 690, 420, 790]);
    apply_block_config(b_write);
    spec_wr = get_param(b_write, 'SymbolSpec');
    s = spec_wr.addSymbol('sector_id'); s.Scope = 'Input';  s.Type = 'uint32'; s.Size = '1';
    s = spec_wr.addSymbol('data_512');  s.Scope = 'Input';  s.Type = 'uint8';  s.Size = '512';
    s = spec_wr.addSymbol('status');    s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(b_write, 'OutputCode', sprintf([...
        '/* Write Single 512-Byte Physical Sector (CMD24) */\n' ...
        'status = SD_WriteSector(sector_id, data_512);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/Write_Sector_ID'], ...
        'Position', [60, 700, 160, 720], 'Value', 'uint32(1)', 'OutDataTypeStr', 'uint32');
    add_block('simulink/Sources/Constant', [model_name, '/Write_Data_512'], ...
        'Position', [60, 740, 160, 765], 'Value', 'uint8(repmat(1:32, 1, 16))', 'OutDataTypeStr', 'uint8');
    add_line(model_name, 'Write_Sector_ID/1', 'SD_WriteSector/1');
    add_line(model_name, 'Write_Data_512/1', 'SD_WriteSector/2');

    add_block('simulink/Sinks/Display', [model_name, '/Write_Status'], 'Position', [500, 730, 600, 755]);
    add_line(model_name, 'SD_WriteSector/1', 'Write_Status/1');

    % =============================================================
    % 7. BLOCK: SD_ReadPayload (Compact BMS Slice)
    % =============================================================
    b_pay = [model_name, '/SD_ReadPayload'];
    add_block('simulink/User-Defined Functions/C Function', b_pay, ...
        'Position', [250, 830, 420, 950]);
    apply_block_config(b_pay);
    spec_pl = get_param(b_pay, 'SymbolSpec');
    s = spec_pl.addSymbol('sector_id');  s.Scope = 'Input';  s.Type = 'uint32'; s.Size = '1';
    s = spec_pl.addSymbol('offset');     s.Scope = 'Input';  s.Type = 'uint16'; s.Size = '1';
    s = spec_pl.addSymbol('length');     s.Scope = 'Input';  s.Type = 'uint16'; s.Size = '1';
    s = spec_pl.addSymbol('payload_32'); s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '32';
    s = spec_pl.addSymbol('status');     s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(b_pay, 'OutputCode', sprintf([...
        '/* Read Partial Slice from Sector (e.g. 32-Byte BMS Record) */\n' ...
        'status = SD_ReadPayload(sector_id, offset, payload_32, length);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/Payload_Sector_ID'], ...
        'Position', [60, 840, 160, 860], 'Value', 'uint32(0)', 'OutDataTypeStr', 'uint32');
    add_block('simulink/Sources/Constant', [model_name, '/Payload_Offset'], ...
        'Position', [60, 875, 160, 895], 'Value', 'uint16(0)', 'OutDataTypeStr', 'uint16');
    add_block('simulink/Sources/Constant', [model_name, '/Payload_Length'], ...
        'Position', [60, 910, 160, 930], 'Value', 'uint16(32)', 'OutDataTypeStr', 'uint16');
    add_line(model_name, 'Payload_Sector_ID/1', 'SD_ReadPayload/1');
    add_line(model_name, 'Payload_Offset/1', 'SD_ReadPayload/2');
    add_line(model_name, 'Payload_Length/1', 'SD_ReadPayload/3');

    add_block('simulink/Sinks/Display', [model_name, '/Payload_32B_Display'], 'Position', [500, 845, 720, 915]);
    add_block('simulink/Sinks/Display', [model_name, '/Payload_Status'],      'Position', [500, 925, 600, 950]);
    add_line(model_name, 'SD_ReadPayload/1', 'Payload_32B_Display/1');
    add_line(model_name, 'SD_ReadPayload/2', 'Payload_Status/1');

    % =============================================================
    % 8. BLOCK: SD_WriteMultiBlock (Burst Stream Write CMD25)
    % =============================================================
    b_mwr = [model_name, '/SD_WriteMultiBlock'];
    add_block('simulink/User-Defined Functions/C Function', b_mwr, ...
        'Position', [250, 990, 420, 1100]);
    apply_block_config(b_mwr);
    spec_mw = get_param(b_mwr, 'SymbolSpec');
    s = spec_mw.addSymbol('start_sector'); s.Scope = 'Input';  s.Type = 'uint32'; s.Size = '1';
    s = spec_mw.addSymbol('data_1024');    s.Scope = 'Input';  s.Type = 'uint8';  s.Size = '1024';
    s = spec_mw.addSymbol('count');        s.Scope = 'Input';  s.Type = 'uint32'; s.Size = '1';
    s = spec_mw.addSymbol('status');       s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(b_mwr, 'OutputCode', sprintf([...
        '/* Burst Multi-Sector Write (CMD25) */\n' ...
        'status = SD_WriteMultipleSectors(start_sector, data_1024, count);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/MW_Start_Sector'], ...
        'Position', [60, 1000, 160, 1020], 'Value', 'uint32(10)', 'OutDataTypeStr', 'uint32');
    add_block('simulink/Sources/Constant', [model_name, '/MW_Data_1024'], ...
        'Position', [60, 1035, 160, 1055], 'Value', 'uint8(repmat(1:64, 1, 16))', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/MW_Block_Count'], ...
        'Position', [60, 1070, 160, 1090], 'Value', 'uint32(2)', 'OutDataTypeStr', 'uint32');
    add_line(model_name, 'MW_Start_Sector/1', 'SD_WriteMultiBlock/1');
    add_line(model_name, 'MW_Data_1024/1', 'SD_WriteMultiBlock/2');
    add_line(model_name, 'MW_Block_Count/1', 'SD_WriteMultiBlock/3');

    add_block('simulink/Sinks/Display', [model_name, '/MultiWrite_Status'], 'Position', [500, 1035, 600, 1060]);
    add_line(model_name, 'SD_WriteMultiBlock/1', 'MultiWrite_Status/1');

    % =============================================================
    % 9. BLOCK: SD_ReadMultiBlock (Burst Stream Read CMD18)
    % =============================================================
    b_mrd = [model_name, '/SD_ReadMultiBlock'];
    add_block('simulink/User-Defined Functions/C Function', b_mrd, ...
        'Position', [250, 1140, 420, 1240]);
    apply_block_config(b_mrd);
    spec_mr = get_param(b_mrd, 'SymbolSpec');
    s = spec_mr.addSymbol('start_sector'); s.Scope = 'Input';  s.Type = 'uint32'; s.Size = '1';
    s = spec_mr.addSymbol('count');        s.Scope = 'Input';  s.Type = 'uint32'; s.Size = '1';
    s = spec_mr.addSymbol('data_1024');    s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1024';
    s = spec_mr.addSymbol('status');       s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(b_mrd, 'OutputCode', sprintf([...
        '/* Burst Multi-Sector Read (CMD18) */\n' ...
        'status = SD_ReadMultipleSectors(start_sector, data_1024, count);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/MR_Start_Sector'], ...
        'Position', [60, 1155, 160, 1175], 'Value', 'uint32(10)', 'OutDataTypeStr', 'uint32');
    add_block('simulink/Sources/Constant', [model_name, '/MR_Block_Count'], ...
        'Position', [60, 1195, 160, 1215], 'Value', 'uint32(2)', 'OutDataTypeStr', 'uint32');
    add_line(model_name, 'MR_Start_Sector/1', 'SD_ReadMultiBlock/1');
    add_line(model_name, 'MR_Block_Count/1', 'SD_ReadMultiBlock/2');

    add_block('simulink/Sinks/Display', [model_name, '/MultiRead_Status'], 'Position', [500, 1200, 600, 1225]);
    add_line(model_name, 'SD_ReadMultiBlock/2', 'MultiRead_Status/1');

    % =============================================================
    % 10. BLOCK: SD_EraseBlocks (Flash Range Erase CMD32/33/38)
    % =============================================================
    b_ers = [model_name, '/SD_EraseBlocks'];
    add_block('simulink/User-Defined Functions/C Function', b_ers, ...
        'Position', [250, 1280, 420, 1370]);
    apply_block_config(b_ers);
    spec_eb = get_param(b_ers, 'SymbolSpec');
    s = spec_eb.addSymbol('start_sector'); s.Scope = 'Input';  s.Type = 'uint32'; s.Size = '1';
    s = spec_eb.addSymbol('end_sector');   s.Scope = 'Input';  s.Type = 'uint32'; s.Size = '1';
    s = spec_eb.addSymbol('status');       s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(b_ers, 'OutputCode', sprintf([...
        '/* Erase Flash Sectors Range (CMD32 -> CMD33 -> CMD38) */\n' ...
        'status = SD_EraseSectors(start_sector, end_sector);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/Erase_Start_Sector'], ...
        'Position', [60, 1290, 160, 1310], 'Value', 'uint32(100)', 'OutDataTypeStr', 'uint32');
    add_block('simulink/Sources/Constant', [model_name, '/Erase_End_Sector'], ...
        'Position', [60, 1330, 160, 1350], 'Value', 'uint32(120)', 'OutDataTypeStr', 'uint32');
    add_line(model_name, 'Erase_Start_Sector/1', 'SD_EraseBlocks/1');
    add_line(model_name, 'Erase_End_Sector/1', 'SD_EraseBlocks/2');

    add_block('simulink/Sinks/Display', [model_name, '/Erase_Blocks_Status'], 'Position', [500, 1315, 600, 1340]);
    add_line(model_name, 'SD_EraseBlocks/1', 'Erase_Blocks_Status/1');

    % =============================================================
    % 11. BLOCK: SD_SetInstance (Configurable LPSPI Instance)
    % =============================================================
    b_inst = [model_name, '/SD_SetInstance'];
    add_block('simulink/User-Defined Functions/C Function', b_inst, ...
        'Position', [250, 1410, 420, 1470]);
    apply_block_config(b_inst);
    spec_is = get_param(b_inst, 'SymbolSpec');
    s = spec_is.addSymbol('instance'); s.Scope = 'Input'; s.Type = 'uint32'; s.Size = '1';
    set_param(b_inst, 'OutputCode', sprintf([...
        '/* Configure LPSPI Instance (0 = LPSPI0, 1 = LPSPI1, 2 = LPSPI2) */\n' ...
        'SD_SetSPIInstance(instance);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/LPSPI_Instance'], ...
        'Position', [60, 1425, 160, 1455], 'Value', 'uint32(0)', 'OutDataTypeStr', 'uint32');
    add_line(model_name, 'LPSPI_Instance/1', 'SD_SetInstance/1');

    % Save model into models/ directory
    save_system(model_name, save_path);
    disp(['100% Feature-complete SD Card model saved to: ', save_path]);
    close_system(model_name, 0);
    disp('=== Setup Complete ===');
end
