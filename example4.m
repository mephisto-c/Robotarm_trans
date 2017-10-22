clc;
clear;
L1 = Link([pi/2 0.1175 0 0 0]);
L2 = Link([0 0.59 0 0 0]);
L3 = Link([pi/2 0.1675 0 0 0]);
L4 = Link([-pi/2 0 0.671 0]);
L5 = Link([pi/2 0 0 0 0]);
L6 = Link([0 0 0 0 0]);
JRB=SerialLink([L1,L2,L3,L4,L5,L6])
theta = [0 pi/2 0 0 0 0];
JRB.plot(theta);


q0 =[0 90 0 0 0 0]*pi/180;% 起始点位置
q3 =[-10.33 71.17 -7.69 -5.15 -42 0]*pi/180;
q4 =[10.1 71.16 -7.68 5 -42.1 0]*pi/180;
%机器人在点0、3时的位姿矩阵，运用MATLAB机器人工具箱函数fkine()求解：
T0 = JRB.fkine(q0);% 点0位姿矩阵
T3 = JRB.fkine(q3);% 点3位姿矩阵机器人在点1，2，4处的位姿矩阵通过平移变换得到：
T4 = transl(0.165,0,-0.095)*T3;% 点4处位姿矩阵

t1 = 0:0.02:2; %0~2s ，采样时间20ms
Q0_3 = ctraj(T0,T3,length(t1)); % 点0到点3
Q03 = JRB.ikine(Q0_3,q0);
JRB.plot(Q03)
Q3_4 = ctraj(T3,T4,length(t1)); % 点0到点3
Q04 = JRB.ikine(Q3_4,q3);