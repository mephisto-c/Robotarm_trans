clear;
clc;
L1 = Link('d', 55, 'a', 0, 'alpha', pi);    %Link �ຯ��
L2 = Link('d', 0, 'a',14 , 'alpha', -pi/2);
L3 = Link('d', 0, 'a', 106, 'alpha', 0);
L4 = Link('d', 0, 'a', 149, 'alpha', 0);
L5 = Link('d',0, 'a',73 , 'alpha',pi/2);
L6 = Link('d',0, 'a',106 , 'alpha',pi/2);
%b=isrevolute(L1);  %Link �ຯ��
robot=SerialLink([L1,L2,L3,L4,L5,L6]);   %SerialLink �ຯ��
robot.name='������������˱�';     %SerialLink ����ֵ
robot.manuf='Ʈ�����';     %SerialLink ����ֵ
robot.display();  %Link �ຯ��
theta=[0 0 0 0];
robot.plot(theta);   %SerialLink �ຯ��