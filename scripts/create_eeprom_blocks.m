%% create_eeprom_blocks.m
% Automates the creation and configuration of 100% feature-complete ST M24C04 EEPROM
% C Function blocks in Simulink with Byte/Page writes, CRC-8 safety verification,
% flash range blanking, and acknowledge polling.

function create_eeprom_blocks()
    disp('=== Setting up 100% Complete ST M24C04 EEPROM Simulink Model ===');
    
    model_name = 'EEPROM_M24C04_Demo';
    script_dir = fileparts(mfilename('fullpath'));
    save_path = fullfile(script_dir, '..', 'models', [model_name, '.slx']);
    
    % Close if already open
    if bdIsLoaded(model_name)
        close_system(model_name, 0);
    end
    
    % Create new demo model
    new_system(model_name);
    load_system(model_name);
    
    % Configure Model Configuration Parameters for Custom Code
    set_param(model_name, 'SimCustomHeaderCode', '#include "m24c04.h"');
    set_param(model_name, 'SimCustomSourceCode', '#include "m24c04.c"');
    set_param(model_name, 'CustomHeaderCode', '#include "m24c04.h"');
    set_param(model_name, 'CustomSourceCode', '#include "m24c04.c"');
    set_param(model_name, 'StopTime', '2.0');
    set_param(model_name, 'FixedStep', '0.1');
    set_param(model_name, 'SolverType', 'Fixed-step');

    function apply_block_config(b_path)
        set_param(b_path, 'CustomCodeSettingLocation', 'BlockSettings');
        set_param(b_path, 'CustomCodeIsMultiInstantiable', 'on');
        set_param(b_path, 'SimCustomHeaderFile', 'm24c04.h');
        set_param(b_path, 'SimCustomSourceFile', 'm24c04.c');
        set_param(b_path, 'CustomHeaderFile', 'm24c04.h');
        set_param(b_path, 'CustomSourceFile', 'm24c04.c');
    end

    % =============================================================
    % 1. BLOCK: EEPROM_Write (Arbitrary-Length Page-Safe Write)
    % =============================================================
    write_blk = [model_name, '/EEPROM_Write'];
    add_block('simulink/User-Defined Functions/C Function', write_blk, ...
        'Position', [300, 60, 480, 180]);
    apply_block_config(write_blk);
    spec_w = get_param(write_blk, 'SymbolSpec');
    s1 = spec_w.addSymbol('addr');    s1.Scope = 'Input';  s1.Type = 'uint16'; s1.Size = '1';
    s2 = spec_w.addSymbol('data_in'); s2.Scope = 'Input';  s2.Type = 'uint8';  s2.Size = '32';
    s3 = spec_w.addSymbol('len');     s3.Scope = 'Input';  s3.Type = 'uint16'; s3.Size = '1';
    s4 = spec_w.addSymbol('status');  s4.Scope = 'Output'; s4.Type = 'uint8';  s4.Size = '1';
    set_param(write_blk, 'OutputCode', sprintf([...
        '/* ST M24C04 Multi-Byte Safe Page Write */\n' ...
        'status = M24C04_WriteMulti(addr, data_in, len);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/Write_Address'], ...
        'Position', [80, 65, 180, 85], 'Value', 'uint16(16)', 'OutDataTypeStr', 'uint16');
    add_block('simulink/Sources/Constant', [model_name, '/Write_Data'], ...
        'Position', [80, 105, 180, 125], 'Value', 'uint8(1:32)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Write_Length'], ...
        'Position', [80, 145, 180, 165], 'Value', 'uint16(32)', 'OutDataTypeStr', 'uint16');
    add_line(model_name, 'Write_Address/1', 'EEPROM_Write/1');
    add_line(model_name, 'Write_Data/1', 'EEPROM_Write/2');
    add_line(model_name, 'Write_Length/1', 'EEPROM_Write/3');

    add_block('simulink/Sinks/Display', [model_name, '/Write_Status'], 'Position', [550, 110, 650, 135]);
    add_line(model_name, 'EEPROM_Write/1', 'Write_Status/1');

    % =============================================================
    % 2. BLOCK: EEPROM_Read (Arbitrary-Length Sequential Read)
    % =============================================================
    read_blk = [model_name, '/EEPROM_Read'];
    add_block('simulink/User-Defined Functions/C Function', read_blk, ...
        'Position', [300, 220, 480, 340]);
    apply_block_config(read_blk);
    spec_r = get_param(read_blk, 'SymbolSpec');
    sr1 = spec_r.addSymbol('addr');     sr1.Scope = 'Input';  sr1.Type = 'uint16'; sr1.Size = '1';
    sr2 = spec_r.addSymbol('len');      sr2.Scope = 'Input';  sr2.Type = 'uint16'; sr2.Size = '1';
    sr3 = spec_r.addSymbol('data_out'); sr3.Scope = 'Output'; sr3.Type = 'uint8';  sr3.Size = '32';
    sr4 = spec_r.addSymbol('status');   sr4.Scope = 'Output'; sr4.Type = 'uint8';  sr4.Size = '1';
    set_param(read_blk, 'OutputCode', sprintf([...
        '/* ST M24C04 Multi-Byte Sequential Read */\n' ...
        'status = M24C04_ReadMulti(addr, data_out, len);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/Read_Address'], ...
        'Position', [80, 235, 180, 255], 'Value', 'uint16(16)', 'OutDataTypeStr', 'uint16');
    add_block('simulink/Sources/Constant', [model_name, '/Read_Length'], ...
        'Position', [80, 295, 180, 315], 'Value', 'uint16(32)', 'OutDataTypeStr', 'uint16');
    add_line(model_name, 'Read_Address/1', 'EEPROM_Read/1');
    add_line(model_name, 'Read_Length/1', 'EEPROM_Read/2');

    add_block('simulink/Sinks/Display', [model_name, '/Read_Data_Display'], 'Position', [550, 230, 750, 280]);
    add_block('simulink/Sinks/Display', [model_name, '/Read_Status'], 'Position', [550, 300, 650, 325]);
    add_line(model_name, 'EEPROM_Read/1', 'Read_Data_Display/1');
    add_line(model_name, 'EEPROM_Read/2', 'Read_Status/1');

    % =============================================================
    % 3. BLOCK: EEPROM_Write_CRC (Safety-Critical SAE J1850 CRC-8)
    % =============================================================
    wcrc_blk = [model_name, '/EEPROM_Write_CRC'];
    add_block('simulink/User-Defined Functions/C Function', wcrc_blk, ...
        'Position', [300, 380, 480, 500]);
    apply_block_config(wcrc_blk);
    spec_wc = get_param(wcrc_blk, 'SymbolSpec');
    s = spec_wc.addSymbol('addr');         s.Scope = 'Input';  s.Type = 'uint16'; s.Size = '1';
    s = spec_wc.addSymbol('data_crc_in');  s.Scope = 'Input';  s.Type = 'uint8';  s.Size = '16';
    s = spec_wc.addSymbol('len');          s.Scope = 'Input';  s.Type = 'uint16'; s.Size = '1';
    s = spec_wc.addSymbol('status');       s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(wcrc_blk, 'OutputCode', sprintf([...
        '/* Safety Write: Appends Automotive SAE J1850 CRC-8 */\n' ...
        'status = M24C04_WriteWithCRC(addr, data_crc_in, len);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/CRC_Write_Addr'], ...
        'Position', [80, 390, 180, 410], 'Value', 'uint16(64)', 'OutDataTypeStr', 'uint16');
    add_block('simulink/Sources/Constant', [model_name, '/CRC_Write_Data'], ...
        'Position', [80, 430, 180, 450], 'Value', 'uint8(101:116)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/CRC_Write_Len'], ...
        'Position', [80, 470, 180, 490], 'Value', 'uint16(16)', 'OutDataTypeStr', 'uint16');
    add_line(model_name, 'CRC_Write_Addr/1', 'EEPROM_Write_CRC/1');
    add_line(model_name, 'CRC_Write_Data/1', 'EEPROM_Write_CRC/2');
    add_line(model_name, 'CRC_Write_Len/1', 'EEPROM_Write_CRC/3');

    add_block('simulink/Sinks/Display', [model_name, '/CRC_Write_Status'], 'Position', [550, 430, 650, 455]);
    add_line(model_name, 'EEPROM_Write_CRC/1', 'CRC_Write_Status/1');

    % =============================================================
    % 4. BLOCK: EEPROM_Read_CRC (Safety-Critical Check with CRC-8)
    % =============================================================
    rcrc_blk = [model_name, '/EEPROM_Read_CRC'];
    add_block('simulink/User-Defined Functions/C Function', rcrc_blk, ...
        'Position', [300, 540, 480, 660]);
    apply_block_config(rcrc_blk);
    spec_rc = get_param(rcrc_blk, 'SymbolSpec');
    s = spec_rc.addSymbol('addr');         s.Scope = 'Input';  s.Type = 'uint16'; s.Size = '1';
    s = spec_rc.addSymbol('len');          s.Scope = 'Input';  s.Type = 'uint16'; s.Size = '1';
    s = spec_rc.addSymbol('data_crc_out'); s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '16';
    s = spec_rc.addSymbol('crc_error');    s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(rcrc_blk, 'OutputCode', sprintf([...
        '/* Safety Read: Verifies against stored SAE J1850 CRC-8 (0=Match, 4=Mismatch) */\n' ...
        'crc_error = M24C04_ReadWithCRC(addr, data_crc_out, len);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/CRC_Read_Addr'], ...
        'Position', [80, 555, 180, 575], 'Value', 'uint16(64)', 'OutDataTypeStr', 'uint16');
    add_block('simulink/Sources/Constant', [model_name, '/CRC_Read_Len'], ...
        'Position', [80, 615, 180, 635], 'Value', 'uint16(16)', 'OutDataTypeStr', 'uint16');
    add_line(model_name, 'CRC_Read_Addr/1', 'EEPROM_Read_CRC/1');
    add_line(model_name, 'CRC_Read_Len/1', 'EEPROM_Read_CRC/2');

    add_block('simulink/Sinks/Display', [model_name, '/CRC_Data_Display'], 'Position', [550, 550, 750, 600]);
    add_block('simulink/Sinks/Display', [model_name, '/CRC_Error_Flag'], 'Position', [550, 620, 650, 645]);
    add_line(model_name, 'EEPROM_Read_CRC/1', 'CRC_Data_Display/1');
    add_line(model_name, 'EEPROM_Read_CRC/2', 'CRC_Error_Flag/1');

    % =============================================================
    % 5. BLOCK: EEPROM_EraseRange (Flash Blanking with Pattern)
    % =============================================================
    ers_blk = [model_name, '/EEPROM_EraseRange'];
    add_block('simulink/User-Defined Functions/C Function', ers_blk, ...
        'Position', [300, 700, 480, 820]);
    apply_block_config(ers_blk);
    spec_er = get_param(ers_blk, 'SymbolSpec');
    s = spec_er.addSymbol('start_addr'); s.Scope = 'Input';  s.Type = 'uint16'; s.Size = '1';
    s = spec_er.addSymbol('num_bytes');  s.Scope = 'Input';  s.Type = 'uint16'; s.Size = '1';
    s = spec_er.addSymbol('fill_byte');  s.Scope = 'Input';  s.Type = 'uint8';  s.Size = '1';
    s = spec_er.addSymbol('status');     s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(ers_blk, 'OutputCode', sprintf([...
        '/* Blank EEPROM Range with pattern (e.g. 0xFF) */\n' ...
        'status = M24C04_EraseRange(start_addr, num_bytes, fill_byte);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/Erase_Start_Addr'], ...
        'Position', [80, 710, 180, 730], 'Value', 'uint16(128)', 'OutDataTypeStr', 'uint16');
    add_block('simulink/Sources/Constant', [model_name, '/Erase_Num_Bytes'], ...
        'Position', [80, 745, 180, 765], 'Value', 'uint16(32)', 'OutDataTypeStr', 'uint16');
    add_block('simulink/Sources/Constant', [model_name, '/Erase_Fill_Byte'], ...
        'Position', [80, 780, 180, 800], 'Value', 'uint8(255)', 'OutDataTypeStr', 'uint8');
    add_line(model_name, 'Erase_Start_Addr/1', 'EEPROM_EraseRange/1');
    add_line(model_name, 'Erase_Num_Bytes/1', 'EEPROM_EraseRange/2');
    add_line(model_name, 'Erase_Fill_Byte/1', 'EEPROM_EraseRange/3');

    add_block('simulink/Sinks/Display', [model_name, '/Erase_Status'], 'Position', [550, 750, 650, 775]);
    add_line(model_name, 'EEPROM_EraseRange/1', 'Erase_Status/1');

    % =============================================================
    % 6. BLOCK: EEPROM_IsDeviceReady (Acknowledge Polling)
    % =============================================================
    rdy_blk = [model_name, '/EEPROM_IsDeviceReady'];
    add_block('simulink/User-Defined Functions/C Function', rdy_blk, ...
        'Position', [300, 860, 480, 920]);
    apply_block_config(rdy_blk);
    spec_rd = get_param(rdy_blk, 'SymbolSpec');
    s = spec_rd.addSymbol('ready_flag'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(rdy_blk, 'OutputCode', sprintf([...
        '/* Acknowledge Polling: Query Chip Ready State after Write Cycle (t_W) */\n' ...
        'ready_flag = (uint8_t)M24C04_IsDeviceReady();\n']));

    add_block('simulink/Sinks/Display', [model_name, '/Device_Ready_Flag'], 'Position', [550, 875, 650, 900]);
    add_line(model_name, 'EEPROM_IsDeviceReady/1', 'Device_Ready_Flag/1');

    % =============================================================
    % 7. BLOCK: M24C04_SetInstance (Configurable LPI2C Instance)
    % =============================================================
    inst_blk = [model_name, '/M24C04_SetInstance'];
    add_block('simulink/User-Defined Functions/C Function', inst_blk, ...
        'Position', [300, 960, 480, 1020]);
    apply_block_config(inst_blk);
    spec_i = get_param(inst_blk, 'SymbolSpec');
    si1 = spec_i.addSymbol('instance'); si1.Scope = 'Input'; si1.Type = 'uint32'; si1.Size = '1';
    set_param(inst_blk, 'OutputCode', sprintf([...
        '/* Set LPI2C Hardware Instance (0 = LPI2C0, 1 = LPI2C1) */\n' ...
        'M24C04_SetI2CInstance(instance);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/I2C_Instance'], ...
        'Position', [80, 975, 180, 1005], 'Value', 'uint32(0)', 'OutDataTypeStr', 'uint32');
    add_line(model_name, 'I2C_Instance/1', 'M24C04_SetInstance/1');

    % Save demo model
    save_system(model_name, save_path);
    disp(['100% Feature-complete EEPROM model saved to: ', save_path]);
    close_system(model_name, 0);
    disp('=== Setup Complete ===');
end
