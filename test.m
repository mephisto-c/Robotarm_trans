clear
clc
L1 = Link('d',0.066 , 'a',0, 'alpha', -pi/2);%'a',0.014 
L2 = Link('d', 0, 'a', 0.105, 'alpha', 0);
L3 = Link('d', 0, 'a', 0.145, 'alpha', 0);
L4 = Link('d',0, 'a',0.073 , 'alpha',0);
L5 = Link('d',0 , 'a',0.066, 'alpha', pi/2);
robot1=SerialLink([L1,L2,L3,L4,L5]);
%theta=[0,0,0,0,0];

init_ang=[0 0 0 pi/2 0];%末端向下
p1=robot1.fkine(init_ang);
robot1.plot(init_ang);
%%
%targ_ang=[pi/4,-pi/3,pi/5,pi/2];
%step=40;
%[q,qd,qdd] = jtraj(init_ang, targ_ang, step);
%subplot(3,2,[1,3]);
%robot1.plot(q); 
%p2 = robot1.fkine(targ_ang);
%q2=robot1.ikunc(targ_ang)*180/pi;  %逆运动学
%p1=robot1.fkine(targ_ang);
%robot1.plot(targ_ang);
