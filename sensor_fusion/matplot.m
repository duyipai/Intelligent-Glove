pose = load('pose.txt');
length = size(pose, 1);
t = 0.01:0.01:length * 0.01;
subplot(2,3,1);
plot(t, pose(:, 1));
title('roll');
subplot(2,3,2);
plot(t, pose(:, 2));
title('pitch');
subplot(2,3,3);
plot(t, pose(:, 3));
title('yaw');
subplot(2,3,4);
plot(t, pose(:, 4));
title('x');
subplot(2,3,5);
plot(t, pose(:, 5));
title('y');
subplot(2,3,6);
plot(t, pose(:, 6));
title('z');
%% 
data=load('3_out.txt');
xbias = mean(data(:,1));
ybias = mean(data(:,2));
zbias = mean(data(:,3));
xd = mean(data(:,4));
yd = mean(data(:,5));
zd = mean(data(:,6));