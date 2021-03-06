% clear
% clc
% L1 = Link('d', 0, 'a',0 , 'alpha',0);
% L2 = Link('d', 0, 'a',0.286, 'alpha',-pi/2);
% L3 = Link('d', 0, 'a', 0.700, 'alpha',-pi/2);
% L4 = Link('d',0.905, 'a',0.135 , 'alpha',pi/2);
% L5 = Link('d',0, 'a',0 , 'alpha',pi/2);
% L6 = Link('d',0, 'a',0 , 'alpha',-pi/2);
% robot=SerialLink([L1,L2,L3,L4,L5,L6]);
% init_ang=[0 0 0 0 0 0];
% %p1 = robot.fkine(init_ang)*transl(-0.16,0.1,0)
% %init_ang=robot.ikunc(p1);
% %init_ang_du = init_ang*180/pi;
% %robot.plot(init_ang);
% robot.teach(init_ang);

%%
%my robotarm
close all
clear all
clc
L1 = Link('alpha',-pi/2 , 'a',56 , 'd',45);
L2 = Link('alpha',0 , 'a',108 ,'d',0);
L3 = Link('alpha',pi/2 , 'a',-22 ,'d',0);
L4 = Link('alpha',pi/2 , 'a',0,'d',158);
L5 = Link('alpha',pi/2 , 'a',52,'d',0);
robot=SerialLink([L1,L2,L3,L4,L5]);
init_ang=[0 -pi/2 pi 0 0];
robot.teach(init_ang);