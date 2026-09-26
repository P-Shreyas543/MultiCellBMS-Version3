%% startup.m
% MultiCellBMS-Version3 Project Initialization Script
% 
% Automatically adds all project directories (drivers, models, scripts, docs)
% to the MATLAB path so all models and C Function blocks locate headers and sources.

function startup()
    fprintf('=======================================================\n');
    fprintf('  Initializing MultiCell BMS Algorithm Development LAB\n');
    fprintf('  Project: MultiCellBMS-Version3\n');
    fprintf('=======================================================\n');
    
    project_root = fileparts(mfilename('fullpath'));
    
    % Subfolders to add to path
    folders = {
        fullfile(project_root, 'drivers', 'eeprom_m24c04');
        fullfile(project_root, 'drivers', 'rtc_ds3231');
        fullfile(project_root, 'models');
        fullfile(project_root, 'scripts');
        fullfile(project_root, 'docs');
    };
    
    for i = 1:length(folders)
        if exist(folders{i}, 'dir')
            addpath(folders{i});
            fprintf('  [+] Path added: %s\n', folders{i});
        end
    end
    
    fprintf('Environment initialized successfully.\n');
    fprintf('=======================================================\n');
end
