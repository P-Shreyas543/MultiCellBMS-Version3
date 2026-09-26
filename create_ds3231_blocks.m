%% create_ds3231_blocks.m
% Automates the creation of clean, modular, and user-friendly Maxim DS3231 RTC
% C Function blocks with SEPARATE Time and Date blocks (no demuxing needed).

function create_ds3231_blocks()
    disp('=== Setting up User-Friendly Maxim DS3231 RTC Blocks ===');
    
    model_name = 'RTC_DS3231_Demo';
    
    % Close if already open
    if bdIsLoaded(model_name)
        close_system(model_name, 0);
    end
    
    % Create new demo model
    new_system(model_name);
    load_system(model_name);
    
    % Configure Model Configuration Parameters for Custom Code
    set_param(model_name, 'SimCustomHeaderCode', '#include "ds3231.h"');
    set_param(model_name, 'SimCustomSourceCode', '#include "ds3231.c"');
    set_param(model_name, 'CustomHeaderCode', '#include "ds3231.h"');
    set_param(model_name, 'CustomSourceCode', '#include "ds3231.c"');
    set_param(model_name, 'StopTime', '10.0');
    set_param(model_name, 'FixedStep', '1.0');
    set_param(model_name, 'SolverType', 'Fixed-step');
    
    % Helper function to set custom code paths on blocks
    function apply_block_config(b_path)
        set_param(b_path, 'CustomCodeSettingLocation', 'BlockSettings');
        set_param(b_path, 'CustomCodeIsMultiInstantiable', 'on');
        set_param(b_path, 'SimCustomHeaderFile', 'ds3231.h');
        set_param(b_path, 'SimCustomSourceFile', 'ds3231.c');
        set_param(b_path, 'CustomHeaderFile', 'ds3231.h');
        set_param(b_path, 'CustomSourceFile', 'ds3231.c');
    end

    % =============================================================
    % 1. BLOCK: RTC_GetTime (Separate Hours, Minutes, Seconds)
    % =============================================================
    b_get_time = [model_name, '/RTC_GetTime'];
    add_block('simulink/User-Defined Functions/C Function', b_get_time, ...
        'Position', [260, 60, 440, 180]);
    apply_block_config(b_get_time);
    
    spec_gt = get_param(b_get_time, 'SymbolSpec');
    
    s = spec_gt.addSymbol('hours');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_gt.addSymbol('minutes');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_gt.addSymbol('seconds');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_gt.addSymbol('status');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    set_param(b_get_time, 'OutputCode', sprintf([...
        '/* Read Time: Hours (0-23), Minutes (0-59), Seconds (0-59) */\n' ...
        'status = DS3231_GetTimeScalars(&hours, &minutes, &seconds);\n']));

    % =============================================================
    % 2. BLOCK: RTC_GetDate (Separate Year, Month, Date, Day)
    % =============================================================
    b_get_date = [model_name, '/RTC_GetDate'];
    add_block('simulink/User-Defined Functions/C Function', b_get_date, ...
        'Position', [260, 240, 440, 380]);
    apply_block_config(b_get_date);
    
    spec_gd = get_param(b_get_date, 'SymbolSpec');
    
    s = spec_gd.addSymbol('date');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_gd.addSymbol('month');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_gd.addSymbol('year');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_gd.addSymbol('day_of_week');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_gd.addSymbol('status');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    set_param(b_get_date, 'OutputCode', sprintf([...
        '/* Read Date: Date (1-31), Month (1-12), Year (0-99), DayOfWeek (1-7) */\n' ...
        'status = DS3231_GetDateScalars(&year, &month, &date, &day_of_week);\n']));

    % =============================================================
    % 3. BLOCK: RTC_GetTemperature (Internal TCXO sensor)
    % =============================================================
    b_temp = [model_name, '/RTC_GetTemperature'];
    add_block('simulink/User-Defined Functions/C Function', b_temp, ...
        'Position', [260, 430, 440, 520]);
    apply_block_config(b_temp);
    
    spec_tp = get_param(b_temp, 'SymbolSpec');
    
    s = spec_tp.addSymbol('temp_degC');
    s.Scope = 'Output'; s.Type = 'single'; s.Size = '1';
    
    s = spec_tp.addSymbol('status');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    set_param(b_temp, 'OutputCode', sprintf([...
        '/* Read Temperature: 0.25 degC resolution */\n' ...
        'status = DS3231_GetTemperature(&temp_degC);\n']));

    % =============================================================
    % 4. BLOCK: RTC_SetTime (Hours, Minutes, Seconds inputs)
    % =============================================================
    b_set_time = [model_name, '/RTC_SetTime'];
    add_block('simulink/User-Defined Functions/C Function', b_set_time, ...
        'Position', [260, 570, 440, 690]);
    apply_block_config(b_set_time);
    
    spec_st = get_param(b_set_time, 'SymbolSpec');
    
    s = spec_st.addSymbol('hours');
    s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_st.addSymbol('minutes');
    s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_st.addSymbol('seconds');
    s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_st.addSymbol('status');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    set_param(b_set_time, 'OutputCode', sprintf([...
        '/* Write Time to DS3231 */\n' ...
        'status = DS3231_SetTimeScalars(hours, minutes, seconds);\n']));

    % =============================================================
    % 5. BLOCK: RTC_SetDate (Year, Month, Date, Day inputs)
    % =============================================================
    b_set_date = [model_name, '/RTC_SetDate'];
    add_block('simulink/User-Defined Functions/C Function', b_set_date, ...
        'Position', [260, 730, 440, 870]);
    apply_block_config(b_set_date);
    
    spec_sd = get_param(b_set_date, 'SymbolSpec');
    
    s = spec_sd.addSymbol('date');
    s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_sd.addSymbol('month');
    s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_sd.addSymbol('year');
    s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_sd.addSymbol('day_of_week');
    s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    
    s = spec_sd.addSymbol('status');
    s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    
    set_param(b_set_date, 'OutputCode', sprintf([...
        '/* Write Date to DS3231 */\n' ...
        'status = DS3231_SetDateScalars(year, month, date, day_of_week);\n']));

    % =============================================================
    % 6. BLOCK: RTC_SetI2CInstance (Configurable I2C instance)
    % =============================================================
    b_inst = [model_name, '/RTC_SetI2CInstance'];
    add_block('simulink/User-Defined Functions/C Function', b_inst, ...
        'Position', [260, 910, 440, 970]);
    apply_block_config(b_inst);
    
    spec_in = get_param(b_inst, 'SymbolSpec');
    s = spec_in.addSymbol('instance');
    s.Scope = 'Input'; s.Type = 'uint32'; s.Size = '1';
    
    set_param(b_inst, 'OutputCode', sprintf([...
        '/* Set LPI2C Hardware Instance (0 = LPI2C0, 1 = LPI2C1) */\n' ...
        'DS3231_SetI2CInstance(instance);\n']));

    % =============================================================
    % 7. Add Display Blocks for RTC_GetTime
    % =============================================================
    add_block('simulink/Sinks/Display', [model_name, '/Hours'], ...
        'Position', [530, 60, 600, 85]);
    add_block('simulink/Sinks/Display', [model_name, '/Minutes'], ...
        'Position', [530, 90, 600, 115]);
    add_block('simulink/Sinks/Display', [model_name, '/Seconds'], ...
        'Position', [530, 120, 600, 145]);
    add_block('simulink/Sinks/Display', [model_name, '/Time_Status'], ...
        'Position', [530, 150, 600, 175]);
    
    add_line(model_name, 'RTC_GetTime/1', 'Hours/1');
    add_line(model_name, 'RTC_GetTime/2', 'Minutes/1');
    add_line(model_name, 'RTC_GetTime/3', 'Seconds/1');
    add_line(model_name, 'RTC_GetTime/4', 'Time_Status/1');

    % =============================================================
    % 8. Add Display Blocks for RTC_GetDate
    % =============================================================
    add_block('simulink/Sinks/Display', [model_name, '/Date'], ...
        'Position', [530, 240, 600, 265]);
    add_block('simulink/Sinks/Display', [model_name, '/Month'], ...
        'Position', [530, 270, 600, 295]);
    add_block('simulink/Sinks/Display', [model_name, '/Year_20xx'], ...
        'Position', [530, 300, 600, 325]);
    add_block('simulink/Sinks/Display', [model_name, '/DayOfWeek'], ...
        'Position', [530, 330, 600, 355]);
    add_block('simulink/Sinks/Display', [model_name, '/Date_Status'], ...
        'Position', [530, 360, 600, 385]);
    
    add_line(model_name, 'RTC_GetDate/1', 'Date/1');
    add_line(model_name, 'RTC_GetDate/2', 'Month/1');
    add_line(model_name, 'RTC_GetDate/3', 'Year_20xx/1');
    add_line(model_name, 'RTC_GetDate/4', 'DayOfWeek/1');
    add_line(model_name, 'RTC_GetDate/5', 'Date_Status/1');

    % =============================================================
    % 9. Add Display Blocks for Temperature
    % =============================================================
    add_block('simulink/Sinks/Display', [model_name, '/Die_Temperature_C'], ...
        'Position', [530, 440, 620, 470]);
    add_block('simulink/Sinks/Display', [model_name, '/Temp_Status'], ...
        'Position', [530, 485, 620, 510]);
    
    add_line(model_name, 'RTC_GetTemperature/1', 'Die_Temperature_C/1');
    add_line(model_name, 'RTC_GetTemperature/2', 'Temp_Status/1');

    % =============================================================
    % 10. Add Inputs for SetTime & SetDate & Instance
    % =============================================================
    add_block('simulink/Sources/Constant', [model_name, '/Set_HH'], ...
        'Position', [70, 575, 150, 600], 'Value', 'uint8(14)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Set_MM'], ...
        'Position', [70, 610, 150, 635], 'Value', 'uint8(30)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Set_SS'], ...
        'Position', [70, 645, 150, 670], 'Value', 'uint8(0)', 'OutDataTypeStr', 'uint8');
    
    add_line(model_name, 'Set_HH/1', 'RTC_SetTime/1');
    add_line(model_name, 'Set_MM/1', 'RTC_SetTime/2');
    add_line(model_name, 'Set_SS/1', 'RTC_SetTime/3');
    
    add_block('simulink/Sinks/Display', [model_name, '/Set_Time_Status'], ...
        'Position', [530, 620, 600, 645]);
    add_line(model_name, 'RTC_SetTime/1', 'Set_Time_Status/1');

    % Set Date Constants
    add_block('simulink/Sources/Constant', [model_name, '/Set_Date_DD'], ...
        'Position', [70, 735, 150, 760], 'Value', 'uint8(26)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Set_Date_MM'], ...
        'Position', [70, 770, 150, 795], 'Value', 'uint8(9)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Set_Date_YY'], ...
        'Position', [70, 805, 150, 830], 'Value', 'uint8(26)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Set_Date_Day'], ...
        'Position', [70, 840, 150, 865], 'Value', 'uint8(7)', 'OutDataTypeStr', 'uint8');
    
    add_line(model_name, 'Set_Date_DD/1', 'RTC_SetDate/1');
    add_line(model_name, 'Set_Date_MM/1', 'RTC_SetDate/2');
    add_line(model_name, 'Set_Date_YY/1', 'RTC_SetDate/3');
    add_line(model_name, 'Set_Date_Day/1', 'RTC_SetDate/4');
    
    add_block('simulink/Sinks/Display', [model_name, '/Set_Date_Status'], ...
        'Position', [530, 790, 600, 815]);
    add_line(model_name, 'RTC_SetDate/1', 'Set_Date_Status/1');

    % Set Instance Constant (default 0 for LPI2C0)
    add_block('simulink/Sources/Constant', [model_name, '/Instance_0_or_1'], ...
        'Position', [70, 925, 150, 955], 'Value', 'uint32(0)', 'OutDataTypeStr', 'uint32');
    add_line(model_name, 'Instance_0_or_1/1', 'RTC_SetI2CInstance/1');

    % Save and close
    save_system(model_name);
    disp(['User-friendly demo model successfully created: ', model_name, '.slx']);
    close_system(model_name, 0);
    disp('=== Setup Complete ===');
end
