clc
clear
close all
%% »æÖÆ»úÆ÷ÈË
m=143.5;
alpha=[pi/2 pi pi/2 pi/2 -pi/2 0];
a=[150 600 100 0 0 0];
d=[0 0 0 -615 0 100+m];
theta=[0 pi/2 0 0 pi 0];
L{1} = Link([alpha(1) a(1) 0 d(1)]);
L{2} = Link([alpha(2) a(2) pi/2 d(2)]);
L{3}= Link([alpha(3) a(3) 0 d(3)]); 
L{4} = Link([alpha(4) a(4) 0 d(4)]);
L{5} = Link([alpha(5) a(5) -pi d(5)]);
L{6} = Link([alpha(6) a(6) 0 d(6)]);
JRB = robot(L,'JRB');
JRB.name = 'JRB';
plot(JRB,theta);
drivebot(JRB);