%第一个机器人模型  
%6个旋转关节  
%xmfbit  
clear L1 L2 L3 L4 L5 L6 R qz qr t q qdd qd  
close all  
clc  
  
  
L1=Link([0,0,20,0,0]);  
L2=Link([0,0,20,0,0]);  
L3=Link([0,0,20,pi/2,0]);  
L4=Link([0,0,20,0,0]);  
L5=Link([0,0,20,0,0]);  
L6=Link([0,0,20,0,0]);  
disp('模型构建完成,参数如下：');  
R=SerialLink([L1,L2,L3,L4,L5,L6])  
qz=[0,0,0,0,0,0];           %初始位置关节点参数  
  
  
%做机械臂初始状态的图  
figure  
R.plot(qz)                         
title('机械臂的初始位姿');  
  
  
  
  
%轨迹求解  
qr=[0,pi/2,0,pi/2,0,0];       %终了位置6个关节点的参数  
t=[0:0.5:10]';  
disp('转动过程中的关节点参数：');  
[q,qd,qdd]=jtraj(qz,qr,t);  
disp('位移参数：');  
q  
disp('速度参数：');  
qd  
disp('加速度参数：');  
qdd  
  
  
%画动态图  
figure  
R.plot(q)  
title('机械臂运动动态图');  
  
  
%以第四个关节点为例画位移，速度，加速度参数变化图  
figure  
%提取矩阵第四列，即为第四个关节点的位移，速度，加速度参数  
q4=q(:,4);  
qd4=qd(:,4);  
qdd4=qdd(:,4);  
subplot(2,2,1)  
plot(t,q4)  
title('位移变化');  
subplot(2,2,2)  
plot(t,qd4)  
title('速度变化');  
subplot(2,2,3)  
plot(t,qdd4)  
title('加速度变化');  