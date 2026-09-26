%% fix_mbd_makefiles.m
% Resolves the GNU Make '*** multiple target patterns. Stop.' error
% in NXP Model-Based Design Toolbox (MBDT) on Windows.
%
% Root Cause:
% When workspace paths contain spaces (e.g., 'MultiCell BMS Algorithum Develpment LAB'),
% Simulink Coder wraps custom source file target prerequisites in double quotes:
%   m24c04.o : "C:\Users\...\m24c04.c"
% GNU Make (gmake) on Windows does not support quotes in targets/prerequisites
% and interprets the drive letter colon (:) as a second target delimiter, throwing:
%   <model>.mk:609: *** multiple target patterns. Stop.
%
% This script automatically sanitizes the generated makefile by stripping quotes
% and converting paths to DOS 8.3 short names without spaces.

function fix_mbd_makefiles(varargin)
    if nargin > 0 && isstruct(varargin{1})
        % Called with buildInfo
        try
            model = varargin{1}.ModelName;
        catch
            model = bdroot;
        end
    elseif nargin > 0 && ischar(varargin{1})
        model = varargin{1};
    else
        try
            model = bdroot;
        catch
            model = 'EEPROM_SmartWheels';
        end
    end

    if isempty(model)
        model = 'EEPROM_SmartWheels';
    end

    mk_name = [model, '.mk'];
    
    % Search in model-specific build directory and current working directory
    search_dirs = {
        fullfile(pwd, [model, '_mbd_rtw']), ...
        fullfile(pwd, 'models', [model, '_mbd_rtw']), ...
        pwd, ...
        fullfile(pwd, 'models')
    };

    fixed_count = 0;

    for d = 1:length(search_dirs)
        mk_path = fullfile(search_dirs{d}, mk_name);
        if exist(mk_path, 'file')
            txt = fileread(mk_path);
            lines = splitlines(txt);
            modified = false;

            for j = 1:length(lines)
                line = lines{j};
                % Match lines like: foo.o : "C:\path\to\file.c"
                if contains(line, '.o : "')
                    toks = regexp(line, '^([a-zA-Z0-9_\.]+\.o\s*:\s*)"([^"]+)"(.*)$', 'tokens');
                    if ~isempty(toks)
                        target_prefix = toks{1}{1};
                        filepath = toks{1}{2};
                        trailing = toks{1}{3};

                        % Convert to DOS 8.3 short path using FileSystemObject
                        ps_cmd = sprintf('powershell -NoProfile -Command "(New-Object -ComObject Scripting.FileSystemObject).GetFile(''%s'').ShortPath"', filepath);
                        [st, short_p] = system(ps_cmd);
                        if (st == 0) && ~isempty(strtrim(short_p))
                            filepath = strtrim(short_p);
                        end

                        lines{j} = sprintf('%s%s%s', target_prefix, filepath, trailing);
                        modified = true;
                    end
                end
                
                % Also strip quotes in compilation rule command lines if any
                if contains(line, '$(CFLAGS) "')
                    toks = regexp(line, '^(.*)\$(CFLAGS\)\s*"([^"]+)"(.*)$', 'tokens');
                    if ~isempty(toks)
                        cmd_prefix = toks{1}{1};
                        filepath = toks{1}{2};
                        cmd_suffix = toks{1}{3};
                        
                        ps_cmd = sprintf('powershell -NoProfile -Command "(New-Object -ComObject Scripting.FileSystemObject).GetFile(''%s'').ShortPath"', filepath);
                        [st, short_p] = system(ps_cmd);
                        if (st == 0) && ~isempty(strtrim(short_p))
                            filepath = strtrim(short_p);
                        end
                        
                        lines{j} = sprintf('%s$(CFLAGS) %s%s', cmd_prefix, filepath, cmd_suffix);
                        modified = true;
                    end
                end
            end

            if modified
                fid = fopen(mk_path, 'w');
                for j = 1:length(lines)
                    fprintf(fid, '%s\n', lines{j});
                end
                fclose(fid);
                fprintf('[+] Successfully sanitized makefile: %s\n', mk_path);
                fixed_count = fixed_count + 1;
            end
        end
    end

    if fixed_count == 0
        fprintf('[-] No makefile needing sanitization found for model: %s\n', model);
    else
        fprintf('[+] Makefile sanitization complete for: %s. GNU Make can now build without errors.\n', model);
    end
end
