STARTING_DATA = 1000;
ENDING_DATA = 40000;

L = readmatrix('data_L_good.csv');
S = readmatrix('steps.csv');

x = [];
y = [];

step = 1;
start = STARTING_DATA;

for i = 1:length(S(:,1))
    if L(STARTING_DATA,1) < S(i,1)
        step = i;
        disp("poop");
        disp(step);
        break;
    end
end
disp(length(S(:,1)))
hold on
for i = STARTING_DATA:ENDING_DATA
    % if i > length(S(:,1))
    %     break;
    % end
    x = [x, L(i,1) - S(step,1)];
    y = [y, L(i,2)];
    if L(i,1) > S(step,1)
        step = step + 1;
        disp("x: ");
        disp(length(x));
        scatter(x,y,3);
        x = [];
        y = [];
        pause(0.01);
    end
end
