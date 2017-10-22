function reangle = trapl(A5,x,y,z)
%A5 = [0,-1,0,0;0,0,1,0;-1,0,0,0;0,0,0,1];
global scom robot
[r,t] = cootran(x,y);%输入x,y
A6 = A5*transl(z,-r,0);
%p1 = robot.fkine(init_ang)*transl(-0.1,0.15,0);%运动学正解求16cm时正上方位姿矩阵
init_ang = robot.ikunc(A6);%逆解求机械臂转动角度
init_ang(1) = init_ang(1)+t;
reangle  = robot.fkine(init_ang);
init_ang_du = init_ang*180/pi%弧度化为角度
robot.plot(init_ang);%绘制机械臂
angle = control(r,init_ang_du);%把初始机械臂的舵机旋转角度转换为180-900，并修正旋转方向
angle=round(angle)
robot_arm_tr(scom,angle)%执行旋转命令