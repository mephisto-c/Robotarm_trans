clear;
clc;
L1 = Link('d', 0, 'a', 0, 'alpha', pi/2);    %Link 类函数
L2 = Link('d', 0, 'a', 0.5, 'alpha', 0,'offset',pi/2);
L3 = Link('d', 0, 'a', 0, 'alpha', pi/2,'offset',pi/4);
L4 = Link('d', 1, 'a', 0, 'alpha', -pi/2);
L5 = Link('d', 0, 'a', 0, 'alpha', pi/2);
L6 = Link('d', 1, 'a', 0, 'alpha', 0);
b=isrevolute(L1);  %Link 类函数
robot=SerialLink([L1,L2,L3,L4,L5,L6]);   %SerialLink 类函数
robot.name='带球形腕的拟人臂';     %SerialLink 属性值
robot.manuf='飘零过客';     %SerialLink 属性值
robot.display();  %Link 类函数
theta=[0 0 0 0 0 0];
robot.plot(theta);   %SerialLink 类函数

theta1=[pi/4,-pi/3,pi/6,pi/4,-pi/3,pi/6];
p0=robot.fkine(theta);
p1=robot.fkine(theta1);
%s=robot.A([4 5 6],theta);
cchain=robot.trchain;
q=robot.getpos();

%q2=robot.ikine(p1);  %逆运动学
q2=robot.ikunc(p1)*180/pi;  %逆运动学
j0=robot.jacob0(q2);    %雅可比矩阵