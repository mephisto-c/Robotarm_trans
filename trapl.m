function reangle = trapl(A5,x,y,z)
%A5 = [0,-1,0,0;0,0,1,0;-1,0,0,0;0,0,0,1];
global scom robot
[r,t] = cootran(x,y);%����x,y
A6 = A5*transl(z,-r,0);
%p1 = robot.fkine(init_ang)*transl(-0.1,0.15,0);%�˶�ѧ������16cmʱ���Ϸ�λ�˾���
init_ang = robot.ikunc(A6);%������е��ת���Ƕ�
init_ang(1) = init_ang(1)+t;
reangle  = robot.fkine(init_ang);
init_ang_du = init_ang*180/pi%���Ȼ�Ϊ�Ƕ�
robot.plot(init_ang);%���ƻ�е��
angle = control(r,init_ang_du);%�ѳ�ʼ��е�۵Ķ����ת�Ƕ�ת��Ϊ180-900����������ת����
angle=round(angle)
robot_arm_tr(scom,angle)%ִ����ת����