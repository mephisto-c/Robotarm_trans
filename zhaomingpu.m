clear;
clc;
L1 = Link('d', 55, 'a', 0, 'alpha', pi);    %Link 类函数
L2 = Link('d', 0, 'a',14 , 'alpha', -pi/2);
L3 = Link('d', 0, 'a', 106, 'alpha', 0);
L4 = Link('d', 0, 'a', 149, 'alpha', 0);
L5 = Link('d',0, 'a',73 , 'alpha',pi/2);
L6 = Link('d',0, 'a',106 , 'alpha',pi/2);
%b=isrevolute(L1);  %Link 类函数
robot=SerialLink([L1,L2,L3,L4,L5,L6]);   %SerialLink 类函数
robot.name='带球形腕的拟人臂';     %SerialLink 属性值
robot.manuf='飘零过客';     %SerialLink 属性值
robot.display();  %Link 类函数
theta=[0 0 0 0];
robot.plot(theta);   %SerialLink 类函数