%%机械臂初始化程序
function robot_arm_Init(scom)
%scom=serial('com5'); 
%scom.BaudRate = 9600;
%fopen(scom);
%a = scom;
write_my(scom,400,0,3);
pause(0.1);
write_my(scom,350,1,3);
pause(0.1);
write_my(scom,500,2,3);
pause(0.1);
write_my(scom,600,3,3);
pause(0.1);
write_my(scom,270,4,3);
pause(0.1);
write_my(scom,600,5,3);
pause(0.1);


