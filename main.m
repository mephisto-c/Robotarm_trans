%%
%��������������5�Ż�е����ת�ǶȲ���ʹ�û�е��
%������
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
%init_ang = [0 0 0 pi/2];%ĩ������

%%
%����Ϊ�켣����

%p2 = robot.fkine(angle)*transl(0,0.15,0.15)%�˶�ѧ������16cmʱ���Ϸ�λ�˾���
%init_ang2 = robot.ikunc(p2);%������е��ת���Ƕ�
%init_ang_du2 = init_ang2*180/pi;%���Ȼ�Ϊ�Ƕ�
%subplot(3,2,2)
%robot.plot(init_ang2);%���ƻ�е��
%angle2 = control(init_ang_du2);%�ѳ�ʼ��е�۵Ķ����ת�Ƕ�ת��Ϊ180-900����������ת����
%angle2 = round(angle2);
%robot_arm_tr(scom,angle2)%ִ����ת����
