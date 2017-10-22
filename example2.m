L{1} = link([pi/2 0.1175 0 0 0]);
L{2} = link([0 0.59 0 0 0]);
L{3} = link([pi/2 0.1675 0 0 0]);
L{4} = link([-pi/2 0 0.671 0]);
L{5} = link([pi/2 0 0 0 0]);
L{6} = link([0 0 0 0 0]);
JRB = robot(L,'JRB');
JRB.name = 'JRB';
theta = [0 pi/2 0 0 0 0];
plot(JRB,theta);
drivebot(JRB);

q0 =[0 90 0 0 0 0]*pi/180;% 起始点位置
q3 =[-10.33 71.17 -7.69 -5.15 -42 0]*pi/180;
q4 =[10.1 71.16 -7.68 5 -42.1 0]*pi/180;
%机器人在点0、3时的位姿矩阵，运用MAT?LAB机器人工具箱函数fkine()求解：
T0 = fkine(JRB,q0);% 点0位姿矩阵
T3 = fkine(JRB,q3);% 点3位姿矩阵机器人在点1，2，4处的位姿矩阵通过平移变换得到：
T4 = transl(.165,0,-0.095)*T3;% 点4处位姿矩阵

t1 = 0:0.02:2; %0~2s ，采样时间20ms
Q0_3 = ctraj(T0,T3,length(t1)); % 点0到点3
Q03 = ikine(JRB,Q0_3,q0);
plot(JRB,Q03)
Q3_4 = ctraj(T3,T4,length(t1)); % 点0到点3
Q04 = ikine(JRB,Q3_4,q3);