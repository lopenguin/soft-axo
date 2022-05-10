NAME = "e.csv";
IDX = 1;

data = csvread(NAME);
y = [];
x = [];
for i = 1:size(data)
    if (abs(data(i, IDX)) < 10)
        y = [y, data(i, IDX)];
        x = [x, i];
    end
end
fprintf("%d\n",size(data))
plot(x,y);