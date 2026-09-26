%% create_eeprom_blocks.m
% Automates the creation and configuration of industry-standard M24C04 EEPROM
% C Function blocks (no enable port, standardized 32-byte array interface).

function create_eeprom_blocks()
    disp('=== Setting up Industry-Standard ST M24C04 EEPROM Blocks ===');
    
    model_name = 'EEPROM_M24C04_Demo';
    
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
    
    % -------------------------------------------------------------
    % 1. Create EEPROM_Write C Function Block (No enable port)
    % -------------------------------------------------------------
    write_blk = [model_name, '/EEPROM_Write'];
    add_block('simulink/User-Defined Functions/C Function', write_blk, ...
        'Position', [300, 100, 480, 220]);
    
    set_param(write_blk, 'CustomCodeSettingLocation', 'BlockSettings');
    set_param(write_blk, 'CustomCodeIsMultiInstantiable', 'on');
    set_param(write_blk, 'SimCustomHeaderFile', 'm24c04.h');
    set_param(write_blk, 'SimCustomSourceFile', 'm24c04.c');
    set_param(write_blk, 'CustomHeaderFile', 'm24c04.h');
    set_param(write_blk, 'CustomSourceFile', 'm24c04.c');
    
    spec_w = get_param(write_blk, 'SymbolSpec');
    
    s1 = spec_w.addSymbol('addr');
    s1.Scope = 'Input';
    s1.Type = 'uint16';
    s1.Size = '1';
    
    s2 = spec_w.addSymbol('data_in');
    s2.Scope = 'Input';
    s2.Type = 'uint8';
    s2.Size = '32';
    
    s3 = spec_w.addSymbol('len');
    s3.Scope = 'Input';
    s3.Type = 'uint16';
    s3.Size = '1';
    
    s4 = spec_w.addSymbol('status');
    s4.Scope = 'Output';
    s4.Type = 'uint8';
    s4.Size = '1';
    
    write_code = sprintf([...
        '/* ST M24C04 EEPROM Write Operation */\n' ...
        'status = M24C04_Write(addr, data_in, len);\n']);
    set_param(write_blk, 'OutputCode', write_code);
    
    % -------------------------------------------------------------
    % 2. Create EEPROM_Read C Function Block (No enable port)
    % -------------------------------------------------------------
    read_blk = [model_name, '/EEPROM_Read'];
    add_block('simulink/User-Defined Functions/C Function', read_blk, ...
        'Position', [300, 300, 480, 420]);
    
    set_param(read_blk, 'CustomCodeSettingLocation', 'BlockSettings');
    set_param(read_blk, 'CustomCodeIsMultiInstantiable', 'on');
    set_param(read_blk, 'SimCustomHeaderFile', 'm24c04.h');
    set_param(read_blk, 'SimCustomSourceFile', 'm24c04.c');
    set_param(read_blk, 'CustomHeaderFile', 'm24c04.h');
    set_param(read_blk, 'CustomSourceFile', 'm24c04.c');
    
    spec_r = get_param(read_blk, 'SymbolSpec');
    
    sr1 = spec_r.addSymbol('addr');
    sr1.Scope = 'Input';
    sr1.Type = 'uint16';
    sr1.Size = '1';
    
    sr2 = spec_r.addSymbol('len');
    sr2.Scope = 'Input';
    sr2.Type = 'uint16';
    sr2.Size = '1';
    
    sr3 = spec_r.addSymbol('data_out');
    sr3.Scope = 'Output';
    sr3.Type = 'uint8';
    sr3.Size = '32';
    
    sr4 = spec_r.addSymbol('status');
    sr4.Scope = 'Output';
    sr4.Type = 'uint8';
    sr4.Size = '1';
    
    read_code = sprintf([...
        '/* ST M24C04 EEPROM Read Operation */\n' ...
        'status = M24C04_Read(addr, data_out, len);\n']);
    set_param(read_blk, 'OutputCode', read_code);

    % -------------------------------------------------------------
    % 3. Add Test Inputs and Display Blocks
    % -------------------------------------------------------------
    % Inputs for Write Block
    add_block('simulink/Sources/Constant', [model_name, '/Write_Address'], ...
        'Position', [80, 105, 180, 125], 'Value', 'uint16(16)', 'OutDataTypeStr', 'uint16');
    add_block('simulink/Sources/Constant', [model_name, '/Write_Data'], ...
        'Position', [80, 145, 180, 165], 'Value', 'uint8(1:32)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Write_Length'], ...
        'Position', [80, 185, 180, 205], 'Value', 'uint16(32)', 'OutDataTypeStr', 'uint16');
    
    % Connect Write Block
    add_line(model_name, 'Write_Address/1', 'EEPROM_Write/1');
    add_line(model_name, 'Write_Data/1', 'EEPROM_Write/2');
    add_line(model_name, 'Write_Length/1', 'EEPROM_Write/3');
    
    % Write Output Status Display
    add_block('simulink/Sinks/Display', [model_name, '/Write_Status'], ...
        'Position', [550, 150, 650, 175]);
    add_line(model_name, 'EEPROM_Write/1', 'Write_Status/1');
    
    % Inputs for Read Block
    add_block('simulink/Sources/Constant', [model_name, '/Read_Address'], ...
        'Position', [80, 315, 180, 335], 'Value', 'uint16(16)', 'OutDataTypeStr', 'uint16');
    add_block('simulink/Sources/Constant', [model_name, '/Read_Length'], ...
        'Position', [80, 375, 180, 395], 'Value', 'uint16(32)', 'OutDataTypeStr', 'uint16');
    
    % Connect Read Block
    add_line(model_name, 'Read_Address/1', 'EEPROM_Read/1');
    add_line(model_name, 'Read_Length/1', 'EEPROM_Read/2');
    
    % Read Output Displays
    add_block('simulink/Sinks/Display', [model_name, '/Read_Data_Display'], ...
        'Position', [550, 310, 750, 360]);
    add_block('simulink/Sinks/Display', [model_name, '/Read_Status'], ...
        'Position', [550, 380, 650, 405]);
    
    add_line(model_name, 'EEPROM_Read/1', 'Read_Data_Display/1');
    add_line(model_name, 'EEPROM_Read/2', 'Read_Status/1');
    
    % Save demo model
    save_system(model_name);
    disp(['Demo model successfully created: ', model_name, '.slx']);
    
    % -------------------------------------------------------------
    % 4. Update EEPROM_SmartWheels.slx with the new blocks
    % -------------------------------------------------------------
    target_model = 'EEPROM_SmartWheels';
    if bdIsLoaded(target_model)
        close_system(target_model, 0);
    end
    load_system(target_model);
    
    % Find any existing EEPROM_Write (in root or subsystem)
    existing_write = find_system(target_model, 'Name', 'EEPROM_Write');
    for i = 1:length(existing_write)
        delete_block(existing_write{i});
    end
    
    % Find any existing EEPROM_Read
    existing_read = find_system(target_model, 'Name', 'EEPROM_Read');
    for i = 1:length(existing_read)
        delete_block(existing_read{i});
    end
    
    % Copy new industry-standard blocks into EEPROM_SmartWheels
    add_block([model_name, '/EEPROM_Write'], [target_model, '/EEPROM_Write'], ...
        'Position', [450, 20, 630, 140]);
    add_block([model_name, '/EEPROM_Read'], [target_model, '/EEPROM_Read'], ...
        'Position', [450, 160, 630, 280]);
    
    save_system(target_model);
    disp(['Updated ', target_model, '.slx with industry-standard EEPROM_Write & EEPROM_Read blocks!']);
    
    close_system(model_name, 0);
    close_system(target_model, 0);
    disp('=== Setup Complete ===');
end
