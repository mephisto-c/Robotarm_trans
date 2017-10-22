%%
%现在最大的问题是5号机械臂旋转角度不足使得机械臂
%主函数
%%
clc
clear
global A5 scom robot;
scom=serial('com5'); 
scom.BaudRate = 9600;
fopen(scom);
L1 = Link('d',0.066 , 'a',0, 'alpha', -pi/2);%'a',0.014 
L2 = Link('d', 0, 'a', 0.105, 'alpha', 0);
L3 = Link('d', 0, 'a', 0.145, 'alpha', 0);
L4 = Link('d',0.01, 'a',0.073 , 'alpha',0);
robot=SerialLink([L1,L2,L3,L4]);
%robot_arm_Init(scom);
%init_ang = [0 0 0 pi/2];%末端向下

%%
%以下为轨迹程序

%p2 = robot.fkine(angle)*transl(0,0.15,0.15)%运动学正解求16cm时正上方位姿矩阵
%init_ang2 = robot.ikunc(p2);%逆解求机械臂转动角度
%init_ang_du2 = init_ang2*180/pi;%弧度化为角度
%subplot(3,2,2)
%robot.plot(init_ang2);%绘制机械臂
%angle2 = control(init_ang_du2);%把初始机械臂的舵机旋转角度转换为180-900，并修正旋转方向
%angle2 = round(angle2);
%robot_arm_tr(scom,angle2)%执行旋转命令
