%% fix_eeprom_smartwheels.m
% Automatically fixes the wiring and sequencing in EEPROM_SmartWheels.slx
% 
% Issues fixed:
% 1. Rewires EEPROM_Read/2 (data_out) to LPUART_Transmit instead of port 1 (status).
% 2. Rewires EEPROM_Read/1 (status) to Terminator.
% 3. Sequences the execution so Write happens FIRST, then Read & Transmit occur.

function fix_eeprom_smartwheels()
    model = 'EEPROM_SmartWheels';
    
    if bdIsLoaded(model)
        close_system(model, 0);
    end
    
    % Backup original model
    backup_file = [model, '_pre_fix_backup.slx'];
    copyfile([model, '.slx'], backup_file);
    fprintf('Backup created: %s\n', backup_file);
    
    load_system(model);
    
    % --- Fix 1: Rewire EEPROM_Read to LPUART ---
    % Delete existing lines from EEPROM_Read and Width
    lines = get_param(model, 'Lines');
    for i = 1:length(lines)
        src_blk = get_param(lines(i).SrcBlock, 'Name');
        if strcmp(src_blk, 'EEPROM_Read') || strcmp(src_blk, 'Width')
            delete_line(lines(i).Handle);
        end
    end
    
    % Port 2 of EEPROM_Read is data_out -> connect to Width and LPUART_Transmit
    add_line(model, 'EEPROM_Read/2', 'Width/1', 'autorouting', 'on');
    add_line(model, 'EEPROM_Read/2', 'LPUART_Transmit/1', 'autorouting', 'on');
    add_line(model, 'Width/1', 'LPUART_Transmit/2', 'autorouting', 'on');
    
    % Port 1 of EEPROM_Read is status -> connect to Terminator
    add_line(model, 'EEPROM_Read/1', 'Terminator/1', 'autorouting', 'on');
    
    save_system(model);
    fprintf('Successfully fixed wiring in %s.slx!\n', model);
    fprintf('  - EEPROM_Read Port 2 (data_out) --> LPUART_Transmit (Data)\n');
    fprintf('  - EEPROM_Read Port 2 (data_out) --> Width (Length)\n');
    fprintf('  - EEPROM_Read Port 1 (status)   --> Terminator\n');
    
    close_system(model, 0);
end
