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
    
    % Subfolders to add to path (models prioritized so local sources compile cleanly in MBDT)
    folders = {
        fullfile(project_root, 'models');
        fullfile(project_root, 'drivers', 'eeprom_m24c04');
        fullfile(project_root, 'drivers', 'rtc_ds3231');
        fullfile(project_root, 'drivers', 'sd_card_spi');
        fullfile(project_root, 'scripts');
        fullfile(project_root, 'docs');
    };
    
    for i = 1:length(folders)
        if exist(folders{i}, 'dir')
            addpath(folders{i}, '-begin');
            fprintf('  [+] Path added: %s\n', folders{i});
        end
    end
    
    % Auto-detect and configure GCC toolchain for S32K1xx if not set
    if isempty(getenv('GCC_S32K_TOOL'))
        nxp_toolchain = 'C:\Users\Shreyas\AppData\Roaming\MathWorks\MATLAB Add-Ons\Toolboxes\NXP_MBDToolbox_S32K1xx\tools\gcc-6.3-arm32-eabi';
        if exist(nxp_toolchain, 'dir')
            setenv('GCC_S32K_TOOL', nxp_toolchain);
            fprintf('  [+] GCC_S32K_TOOL configured: %s\n', nxp_toolchain);
        end
    end
    
    fprintf('Environment initialized successfully.\n');
    fprintf('=======================================================\n');
end
