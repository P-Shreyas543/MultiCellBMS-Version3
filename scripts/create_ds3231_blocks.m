%% create_ds3231_blocks.m
% Automates the creation of clean, modular, and 100% feature-complete Maxim DS3231 RTC
% C Function blocks in Simulink with Time/Date, Alarms 1 & 2, SQW, 32kHz, Aging, & Temp.

function create_ds3231_blocks()
    disp('=== Setting up 100% Complete Maxim DS3231 RTC Simulink Model ===');
    
    model_name = 'RTC_DS3231_Demo';
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
    % SECTION A: TIME & DATE OPERATIONS
    % =============================================================

    % 1. BLOCK: RTC_GetTime (Separate Hours, Minutes, Seconds)
    b_get_time = [model_name, '/RTC_GetTime'];
    add_block('simulink/User-Defined Functions/C Function', b_get_time, ...
        'Position', [260, 60, 440, 180]);
    apply_block_config(b_get_time);
    spec_gt = get_param(b_get_time, 'SymbolSpec');
    s = spec_gt.addSymbol('hours'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    s = spec_gt.addSymbol('minutes'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    s = spec_gt.addSymbol('seconds'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    s = spec_gt.addSymbol('status'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_get_time, 'OutputCode', sprintf([...
        '/* Read Time: Hours (0-23), Minutes (0-59), Seconds (0-59) */\n' ...
        'status = DS3231_GetTimeScalars(&hours, &minutes, &seconds);\n']));

    add_block('simulink/Sinks/Display', [model_name, '/Hours'], 'Position', [530, 60, 600, 85]);
    add_block('simulink/Sinks/Display', [model_name, '/Minutes'], 'Position', [530, 90, 600, 115]);
    add_block('simulink/Sinks/Display', [model_name, '/Seconds'], 'Position', [530, 120, 600, 145]);
    add_block('simulink/Sinks/Display', [model_name, '/Time_Status'], 'Position', [530, 150, 600, 175]);
    add_line(model_name, 'RTC_GetTime/1', 'Hours/1');
    add_line(model_name, 'RTC_GetTime/2', 'Minutes/1');
    add_line(model_name, 'RTC_GetTime/3', 'Seconds/1');
    add_line(model_name, 'RTC_GetTime/4', 'Time_Status/1');

    % 2. BLOCK: RTC_GetDate (Separate Year, Month, Date, Day)
    b_get_date = [model_name, '/RTC_GetDate'];
    add_block('simulink/User-Defined Functions/C Function', b_get_date, ...
        'Position', [260, 230, 440, 360]);
    apply_block_config(b_get_date);
    spec_gd = get_param(b_get_date, 'SymbolSpec');
    s = spec_gd.addSymbol('date'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    s = spec_gd.addSymbol('month'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    s = spec_gd.addSymbol('year'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    s = spec_gd.addSymbol('day_of_week'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    s = spec_gd.addSymbol('status'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_get_date, 'OutputCode', sprintf([...
        '/* Read Date: Date (1-31), Month (1-12), Year (0-99), DayOfWeek (1-7) */\n' ...
        'status = DS3231_GetDateScalars(&year, &month, &date, &day_of_week);\n']));

    add_block('simulink/Sinks/Display', [model_name, '/Date'], 'Position', [530, 230, 600, 255]);
    add_block('simulink/Sinks/Display', [model_name, '/Month'], 'Position', [530, 260, 600, 285]);
    add_block('simulink/Sinks/Display', [model_name, '/Year_20xx'], 'Position', [530, 290, 600, 315]);
    add_block('simulink/Sinks/Display', [model_name, '/DayOfWeek'], 'Position', [530, 320, 600, 345]);
    add_block('simulink/Sinks/Display', [model_name, '/Date_Status'], 'Position', [530, 350, 600, 375]);
    add_line(model_name, 'RTC_GetDate/1', 'Date/1');
    add_line(model_name, 'RTC_GetDate/2', 'Month/1');
    add_line(model_name, 'RTC_GetDate/3', 'Year_20xx/1');
    add_line(model_name, 'RTC_GetDate/4', 'DayOfWeek/1');
    add_line(model_name, 'RTC_GetDate/5', 'Date_Status/1');

    % 3. BLOCK: RTC_SetTime
    b_set_time = [model_name, '/RTC_SetTime'];
    add_block('simulink/User-Defined Functions/C Function', b_set_time, ...
        'Position', [260, 420, 440, 520]);
    apply_block_config(b_set_time);
    spec_st = get_param(b_set_time, 'SymbolSpec');
    s = spec_st.addSymbol('hours'); s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    s = spec_st.addSymbol('minutes'); s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    s = spec_st.addSymbol('seconds'); s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    s = spec_st.addSymbol('status'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_set_time, 'OutputCode', sprintf([...
        '/* Write Time to DS3231 */\n' ...
        'status = DS3231_SetTimeScalars(hours, minutes, seconds);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/Set_HH'], ...
        'Position', [70, 425, 150, 445], 'Value', 'uint8(14)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Set_MM'], ...
        'Position', [70, 455, 150, 475], 'Value', 'uint8(30)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Set_SS'], ...
        'Position', [70, 485, 150, 505], 'Value', 'uint8(0)', 'OutDataTypeStr', 'uint8');
    add_line(model_name, 'Set_HH/1', 'RTC_SetTime/1');
    add_line(model_name, 'Set_MM/1', 'RTC_SetTime/2');
    add_line(model_name, 'Set_SS/1', 'RTC_SetTime/3');
    add_block('simulink/Sinks/Display', [model_name, '/Set_Time_Status'], 'Position', [530, 460, 600, 485]);
    add_line(model_name, 'RTC_SetTime/1', 'Set_Time_Status/1');

    % 4. BLOCK: RTC_SetDate
    b_set_date = [model_name, '/RTC_SetDate'];
    add_block('simulink/User-Defined Functions/C Function', b_set_date, ...
        'Position', [260, 560, 440, 680]);
    apply_block_config(b_set_date);
    spec_sd = get_param(b_set_date, 'SymbolSpec');
    s = spec_sd.addSymbol('date'); s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    s = spec_sd.addSymbol('month'); s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    s = spec_sd.addSymbol('year'); s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    s = spec_sd.addSymbol('day_of_week'); s.Scope = 'Input'; s.Type = 'uint8'; s.Size = '1';
    s = spec_sd.addSymbol('status'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_set_date, 'OutputCode', sprintf([...
        '/* Write Date to DS3231 */\n' ...
        'status = DS3231_SetDateScalars(year, month, date, day_of_week);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/Set_Date_DD'], ...
        'Position', [70, 565, 150, 585], 'Value', 'uint8(26)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Set_Date_MM'], ...
        'Position', [70, 595, 150, 615], 'Value', 'uint8(9)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Set_Date_YY'], ...
        'Position', [70, 625, 150, 645], 'Value', 'uint8(26)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Set_Date_Day'], ...
        'Position', [70, 655, 150, 675], 'Value', 'uint8(7)', 'OutDataTypeStr', 'uint8');
    add_line(model_name, 'Set_Date_DD/1', 'RTC_SetDate/1');
    add_line(model_name, 'Set_Date_MM/1', 'RTC_SetDate/2');
    add_line(model_name, 'Set_Date_YY/1', 'RTC_SetDate/3');
    add_line(model_name, 'Set_Date_Day/1', 'RTC_SetDate/4');
    add_block('simulink/Sinks/Display', [model_name, '/Set_Date_Status'], 'Position', [530, 610, 600, 635]);
    add_line(model_name, 'RTC_SetDate/1', 'Set_Date_Status/1');

    % 5. BLOCK: RTC_SetI2CInstance
    b_inst = [model_name, '/RTC_SetI2CInstance'];
    add_block('simulink/User-Defined Functions/C Function', b_inst, ...
        'Position', [260, 720, 440, 770]);
    apply_block_config(b_inst);
    spec_in = get_param(b_inst, 'SymbolSpec');
    s = spec_in.addSymbol('instance'); s.Scope = 'Input'; s.Type = 'uint32'; s.Size = '1';
    set_param(b_inst, 'OutputCode', sprintf([...
        '/* Set LPI2C Hardware Instance (0 = LPI2C0, 1 = LPI2C1) */\n' ...
        'DS3231_SetI2CInstance(instance);\n']));
    add_block('simulink/Sources/Constant', [model_name, '/Instance_0_or_1'], ...
        'Position', [70, 730, 150, 760], 'Value', 'uint32(0)', 'OutDataTypeStr', 'uint32');
    add_line(model_name, 'Instance_0_or_1/1', 'RTC_SetI2CInstance/1');

    % =============================================================
    % SECTION B: HARDWARE ALARMS (ALARM 1 & ALARM 2)
    % =============================================================

    % 6. BLOCK: RTC_SetAlarm1 (Full 1-Second Resolution Alarm)
    b_a1 = [model_name, '/RTC_SetAlarm1'];
    add_block('simulink/User-Defined Functions/C Function', b_a1, ...
        'Position', [260, 820, 440, 940]);
    apply_block_config(b_a1);
    spec_a1 = get_param(b_a1, 'SymbolSpec');
    s = spec_a1.addSymbol('a1_hr');   s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_a1.addSymbol('a1_min');  s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_a1.addSymbol('a1_sec');  s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_a1.addSymbol('a1_mode'); s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_a1.addSymbol('status');  s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_a1, 'OutputCode', sprintf([...
        '/* Configure Alarm 1 and enable interrupt on INT pin */\n' ...
        'status = DS3231_SetAlarm1(a1_hr, a1_min, a1_sec, a1_mode);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/A1_Hr'], ...
        'Position', [70, 825, 150, 845], 'Value', 'uint8(14)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/A1_Min'], ...
        'Position', [70, 855, 150, 875], 'Value', 'uint8(45)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/A1_Sec'], ...
        'Position', [70, 885, 150, 905], 'Value', 'uint8(0)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/A1_Mode_Rate'], ...
        'Position', [70, 915, 150, 935], 'Value', 'uint8(12)', 'OutDataTypeStr', 'uint8'); % 0x0C = DS3231_ALARM1_MATCH_MIN_SEC
    add_line(model_name, 'A1_Hr/1', 'RTC_SetAlarm1/1');
    add_line(model_name, 'A1_Min/1', 'RTC_SetAlarm1/2');
    add_line(model_name, 'A1_Sec/1', 'RTC_SetAlarm1/3');
    add_line(model_name, 'A1_Mode_Rate/1', 'RTC_SetAlarm1/4');

    add_block('simulink/Sinks/Display', [model_name, '/Alarm1_Set_Status'], 'Position', [530, 870, 600, 895]);
    add_line(model_name, 'RTC_SetAlarm1/1', 'Alarm1_Set_Status/1');

    % 7. BLOCK: RTC_CheckAlarm1 (Polls Hardware Trigger Flag)
    b_ca1 = [model_name, '/RTC_CheckAlarm1'];
    add_block('simulink/User-Defined Functions/C Function', b_ca1, ...
        'Position', [260, 990, 440, 1070]);
    apply_block_config(b_ca1);
    spec_ca1 = get_param(b_ca1, 'SymbolSpec');
    s = spec_ca1.addSymbol('clear_flag');   s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_ca1.addSymbol('a1_triggered'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    s = spec_ca1.addSymbol('status');       s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_ca1, 'OutputCode', sprintf([...
        '/* Check Alarm 1 Trigger Flag (A1F) */\n' ...
        'bool fired = false;\n' ...
        'status = DS3231_CheckAlarm1(&fired, (clear_flag != 0));\n' ...
        'a1_triggered = (uint8_t)fired;\n']));

    add_block('simulink/Sources/Constant', [model_name, '/A1_AutoClear'], ...
        'Position', [70, 1015, 150, 1040], 'Value', 'uint8(0)', 'OutDataTypeStr', 'uint8');
    add_line(model_name, 'A1_AutoClear/1', 'RTC_CheckAlarm1/1');

    add_block('simulink/Sinks/Display', [model_name, '/Alarm1_Triggered'], 'Position', [530, 1000, 600, 1025]);
    add_block('simulink/Sinks/Display', [model_name, '/Alarm1_Chk_Status'], 'Position', [530, 1035, 600, 1060]);
    add_line(model_name, 'RTC_CheckAlarm1/1', 'Alarm1_Triggered/1');
    add_line(model_name, 'RTC_CheckAlarm1/2', 'Alarm1_Chk_Status/1');

    % 8. BLOCK: RTC_ClearAlarm1 (Clears A1F Flag to reset INT pin)
    b_cla1 = [model_name, '/RTC_ClearAlarm1'];
    add_block('simulink/User-Defined Functions/C Function', b_cla1, ...
        'Position', [260, 1110, 440, 1160]);
    apply_block_config(b_cla1);
    spec_cla1 = get_param(b_cla1, 'SymbolSpec');
    s = spec_cla1.addSymbol('status'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_cla1, 'OutputCode', sprintf([...
        '/* Clear Alarm 1 Flag */\n' ...
        'status = DS3231_ClearAlarm1();\n']));
    add_block('simulink/Sinks/Display', [model_name, '/Clear_A1_Status'], 'Position', [530, 1120, 600, 1145]);
    add_line(model_name, 'RTC_ClearAlarm1/1', 'Clear_A1_Status/1');

    % 9. BLOCK: RTC_SetAlarm2 (1-Minute Resolution Alarm)
    b_a2 = [model_name, '/RTC_SetAlarm2'];
    add_block('simulink/User-Defined Functions/C Function', b_a2, ...
        'Position', [260, 1200, 440, 1310]);
    apply_block_config(b_a2);
    spec_a2 = get_param(b_a2, 'SymbolSpec');
    s = spec_a2.addSymbol('a2_hr');   s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_a2.addSymbol('a2_min');  s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_a2.addSymbol('a2_mode'); s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_a2.addSymbol('status');  s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_a2, 'OutputCode', sprintf([...
        '/* Configure Alarm 2 (1-min resolution) */\n' ...
        'status = DS3231_SetAlarm2(a2_hr, a2_min, a2_mode);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/A2_Hr'], ...
        'Position', [70, 1210, 150, 1230], 'Value', 'uint8(8)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/A2_Min'], ...
        'Position', [70, 1240, 150, 1260], 'Value', 'uint8(0)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/A2_Mode_Rate'], ...
        'Position', [70, 1270, 150, 1290], 'Value', 'uint8(6)', 'OutDataTypeStr', 'uint8'); % 0x06 = MATCH_MIN
    add_line(model_name, 'A2_Hr/1', 'RTC_SetAlarm2/1');
    add_line(model_name, 'A2_Min/1', 'RTC_SetAlarm2/2');
    add_line(model_name, 'A2_Mode_Rate/1', 'RTC_SetAlarm2/3');

    add_block('simulink/Sinks/Display', [model_name, '/Alarm2_Set_Status'], 'Position', [530, 1245, 600, 1270]);
    add_line(model_name, 'RTC_SetAlarm2/1', 'Alarm2_Set_Status/1');

    % 10. BLOCK: RTC_CheckAlarm2
    b_ca2 = [model_name, '/RTC_CheckAlarm2'];
    add_block('simulink/User-Defined Functions/C Function', b_ca2, ...
        'Position', [260, 1350, 440, 1430]);
    apply_block_config(b_ca2);
    spec_ca2 = get_param(b_ca2, 'SymbolSpec');
    s = spec_ca2.addSymbol('clear_flag');   s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_ca2.addSymbol('a2_triggered'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    s = spec_ca2.addSymbol('status');       s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_ca2, 'OutputCode', sprintf([...
        '/* Check Alarm 2 Trigger Flag (A2F) */\n' ...
        'bool fired = false;\n' ...
        'status = DS3231_CheckAlarm2(&fired, (clear_flag != 0));\n' ...
        'a2_triggered = (uint8_t)fired;\n']));

    add_block('simulink/Sources/Constant', [model_name, '/A2_AutoClear'], ...
        'Position', [70, 1375, 150, 1400], 'Value', 'uint8(0)', 'OutDataTypeStr', 'uint8');
    add_line(model_name, 'A2_AutoClear/1', 'RTC_CheckAlarm2/1');

    add_block('simulink/Sinks/Display', [model_name, '/Alarm2_Triggered'], 'Position', [530, 1360, 600, 1385]);
    add_block('simulink/Sinks/Display', [model_name, '/Alarm2_Chk_Status'], 'Position', [530, 1395, 600, 1420]);
    add_line(model_name, 'RTC_CheckAlarm2/1', 'Alarm2_Triggered/1');
    add_line(model_name, 'RTC_CheckAlarm2/2', 'Alarm2_Chk_Status/1');

    % =============================================================
    % SECTION C: TEMPERATURE, SQUARE WAVE, 32kHz & AGING OFFSET
    % =============================================================

    % 11. BLOCK: RTC_GetTemperature (Internal TCXO sensor)
    b_temp = [model_name, '/RTC_GetTemperature'];
    add_block('simulink/User-Defined Functions/C Function', b_temp, ...
        'Position', [260, 1470, 440, 1550]);
    apply_block_config(b_temp);
    spec_tp = get_param(b_temp, 'SymbolSpec');
    s = spec_tp.addSymbol('temp_degC'); s.Scope = 'Output'; s.Type = 'single'; s.Size = '1';
    s = spec_tp.addSymbol('status');    s.Scope = 'Output'; s.Type = 'uint8';  s.Size = '1';
    set_param(b_temp, 'OutputCode', sprintf([...
        '/* Read Temperature: 0.25 degC resolution */\n' ...
        'status = DS3231_GetTemperature(&temp_degC);\n']));

    add_block('simulink/Sinks/Display', [model_name, '/Die_Temperature_C'], 'Position', [530, 1475, 620, 1505]);
    add_block('simulink/Sinks/Display', [model_name, '/Temp_Status'],        'Position', [530, 1515, 620, 1540]);
    add_line(model_name, 'RTC_GetTemperature/1', 'Die_Temperature_C/1');
    add_line(model_name, 'RTC_GetTemperature/2', 'Temp_Status/1');

    % 12. BLOCK: RTC_Config_SQW_32kHz (Square Wave & 32kHz pin control)
    b_sqw = [model_name, '/RTC_Config_SQW_32kHz'];
    add_block('simulink/User-Defined Functions/C Function', b_sqw, ...
        'Position', [260, 1590, 440, 1700]);
    apply_block_config(b_sqw);
    spec_sqw = get_param(b_sqw, 'SymbolSpec');
    s = spec_sqw.addSymbol('sqw_freq'); s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_sqw.addSymbol('sqw_en');   s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_sqw.addSymbol('en_32k');   s.Scope = 'Input';  s.Type = 'uint8'; s.Size = '1';
    s = spec_sqw.addSymbol('status');   s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_sqw, 'OutputCode', sprintf([...
        '/* Configure SQW Frequency, Enable SQW Pin, and Enable 32kHz Output */\n' ...
        'if (sqw_en != 0) {\n' ...
        '    status = DS3231_EnableSquareWave((ds3231_sqw_freq_t)sqw_freq, false);\n' ...
        '} else {\n' ...
        '    status = DS3231_DisableSquareWave();\n' ...
        '}\n' ...
        'status |= DS3231_Enable32kHzOutput(en_32k != 0);\n']));

    add_block('simulink/Sources/Constant', [model_name, '/SQW_Freq_Select'], ...
        'Position', [70, 1595, 150, 1615], 'Value', 'uint8(0)', 'OutDataTypeStr', 'uint8'); % 0 = 1Hz
    add_block('simulink/Sources/Constant', [model_name, '/SQW_Enable_Pin'], ...
        'Position', [70, 1630, 150, 1650], 'Value', 'uint8(1)', 'OutDataTypeStr', 'uint8');
    add_block('simulink/Sources/Constant', [model_name, '/Enable_32kHz_Pin'], ...
        'Position', [70, 1665, 150, 1685], 'Value', 'uint8(1)', 'OutDataTypeStr', 'uint8');
    add_line(model_name, 'SQW_Freq_Select/1', 'RTC_Config_SQW_32kHz/1');
    add_line(model_name, 'SQW_Enable_Pin/1', 'RTC_Config_SQW_32kHz/2');
    add_line(model_name, 'Enable_32kHz_Pin/1', 'RTC_Config_SQW_32kHz/3');

    add_block('simulink/Sinks/Display', [model_name, '/SQW_Config_Status'], 'Position', [530, 1635, 600, 1660]);
    add_line(model_name, 'RTC_Config_SQW_32kHz/1', 'SQW_Config_Status/1');

    % 13. BLOCK: RTC_AgingTrim_And_TempConv
    b_ag = [model_name, '/RTC_AgingTrim_And_TempConv'];
    add_block('simulink/User-Defined Functions/C Function', b_ag, ...
        'Position', [260, 1740, 440, 1830]);
    apply_block_config(b_ag);
    spec_ag = get_param(b_ag, 'SymbolSpec');
    s = spec_ag.addSymbol('aging_trim');   s.Scope = 'Input';  s.Type = 'int8';  s.Size = '1';
    s = spec_ag.addSymbol('aging_status'); s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    s = spec_ag.addSymbol('conv_status');  s.Scope = 'Output'; s.Type = 'uint8'; s.Size = '1';
    set_param(b_ag, 'OutputCode', sprintf([...
        '/* Aging Offset Calibration (-128 to +127) & Force Temp Conversion */\n' ...
        'aging_status = DS3231_SetAgingOffset(aging_trim);\n' ...
        'conv_status  = DS3231_TriggerTemperatureConversion();\n']));

    add_block('simulink/Sources/Constant', [model_name, '/Aging_Trim_PPM'], ...
        'Position', [70, 1770, 150, 1795], 'Value', 'int8(0)', 'OutDataTypeStr', 'int8');
    add_line(model_name, 'Aging_Trim_PPM/1', 'RTC_AgingTrim_And_TempConv/1');

    add_block('simulink/Sinks/Display', [model_name, '/Aging_Status'],   'Position', [530, 1755, 600, 1780]);
    add_block('simulink/Sinks/Display', [model_name, '/TempConv_Status'], 'Position', [530, 1790, 600, 1815]);
    add_line(model_name, 'RTC_AgingTrim_And_TempConv/1', 'Aging_Status/1');
    add_line(model_name, 'RTC_AgingTrim_And_TempConv/2', 'TempConv_Status/1');

    % Save and close
    save_system(model_name, save_path);
    disp(['100% Feature-complete DS3231 model saved to: ', save_path]);
    close_system(model_name, 0);
    disp('=== Setup Complete ===');
end
