clear
global init_ang_du
L1 = Link('d', 0.055, 'a',0.014 , 'alpha', -pi/2);
L2 = Link('d', 0, 'a', 0.106, 'alpha', 0);
L3 = Link('d', 0, 'a', 0.149, 'alpha', 0);
L4 = Link('d',0, 'a',0.16 , 'alpha',0);
robot=SerialLink([L1,L2,L3,L4]);
init_ang=[0 0 0 pi/2];
p1 = robot.fkine(init_ang)*transl(-0.16,0.1,0)
init_ang=robot.ikunc(p1);
init_ang_du = init_ang*180/pi;
robot.plot(init_ang);