function d = importData(filename)
%IMPORTDATA Imports Axo data into MATLAB
%   Pass in a specific filename, or nothing to use the gui. Only deals with
%   a single side--call twice to import left and right.

% Use the gui if the filename is not specified
if nargin == 0
    [FileName, PathName, FilterIndex] = uigetfile('*.csv', 'Select Log File');
    filename = [PathName, FileName];
end

d = struct("IMU",table,"FSR",table,"LOAD",table,"MOTOR",table,"LOG",table,"ERR",table);

allData = readfile(filename);

% define keys
imuKey = {};
fsrKey = {};
loadKey = {};
motorKey = {};
logKey = {};
errKey = {};

for row = 1:height(allData)
    label = allData{row, 1}{1};
    r = allData{row,:};
    if label == "I "
        [d.IMU, imuKey] = buildTable(d.IMU, r, imuKey);
    end
    
    if label == "F "
        [d.FSR, fsrKey] = buildTable(d.FSR, r, fsrKey);
    end
    
    if label == "L "
        [d.LOAD, loadKey] = buildTable(d.LOAD, r, loadKey);
    end
    
    if label == "M "
        [d.MOTOR, motorKey] = buildTable(d.MOTOR, r, motorKey);
    end
    
%     if label == "LOG"
%         [d.LOG, logKey] = buildTable(d.LOG, r, logKey);
%     end
%     
%     if label == "ERR"
%         [d.ERR, errKey] = buildTable(d.ERR, r, errKey);
%     end
end

end

function data = readfile(filename)
    % Read the file line by line into  a cell array
    fid = fopen(filename);
    lines = {};
    tline = fgetl(fid);
    while ischar(tline)
       lines{end+1,1} = tline;
       tline = fgetl(fid);
    end
    fclose(fid);
    % Function handle to split each row by comma delimiter 
    func = @(input)strsplit(input, {' | ',','});
    % Apply this function to each row in the cell array to get the final data
    % As each row is of different lenght we set the UniformOutput to false
    data = cellfun(func,lines,'UniformOutput',false);
end

function [tb, key] = buildTable(tb, row, key)
    if isempty(key)
        % we expect the first row to be a heading
        key = row(2:end);
    else
        try
            row = str2double(row(2:end));
            t = array2table(row, 'VariableNames', key);
            tb = [tb; t];
        catch
            disp(row)
        end
    end
end