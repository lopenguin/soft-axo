PLOTRANGE = 5000;

% Reading
E = readmatrix('q.csv');
L = readmatrix('data_L .csv');
M = readmatrix('data_M .csv');
S = readmatrix('steps.csv');

% Set up triple graph----------------------------------------------------------
% tiledlayout(3,1);
figure('Renderer', 'painters', 'Position', [100 100 600 1000]);
% --- MOTOR ---
nexttile
scatter(M(:,1),M(:,2),1);
mAxes = gca;
title('Motor');
xlabel('Time')
ylabel('Motor angle (radians)')

% --- EULER ---
nexttile
hold on
scatter(E(:,1), E(:,2),1);
for index = 1:length(S(:,1))
    xline(S(index, 1))
end
eAxes = gca;
title('Euler');
xlabel('Time')
ylabel('Ankle angle (radians)')
hold off

% --- LOAD ---
nexttile
scatter(L(:,1),L(:,2),1);
lAxes = gca;
title('Load');
xlabel('Time')
ylabel('Force (Newtons?)')
%------------------------------------------------------------------------------

% Set up UI Slider...
uicontrol('Style','slider',...
          'Min',E(2,1),...
          'Max',E(end,1),...
          'Value',E(2,1),...
          'Position', [20 20 400 20],...
          'Callback', @(src,evt) hax(src, PLOTRANGE, eAxes, lAxes, mAxes));


function hax( src, range, ax1, ax2, ax3 )
    s = get(src, 'Value');
    ax1.XLim = [round(s) round(s + range)];
    ax1.YLimMode = 'auto';
    ax2.XLim = [round(s) round(s + range)];
    ax2.YLimMode = 'auto';
    ax3.XLim = [round(s) round(s + range)];
    ax3.YLimMode = 'auto';
end