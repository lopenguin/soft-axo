% Turns raw quaternion/Euler angle data into a plot!
function plotAngle(datafile)

data = readtable(datafile);
s = size(data);
if (s(2) == 5)
    % Quaternion!
    data.Properties.VariableNames = {'Time', 'q0', 'q1', 'q2', 'q3'};
elseif (s(2) == 4)
    % Euler angles
    data.Properties.VariableNames = {'Time', 'x', 'y', 'z'};
end

stackedplot(data, 'XVariable', 'Time')

end