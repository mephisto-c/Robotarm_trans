%%
%»úĞµ±ÛÒÆ¶¯
function robot_arm_tr(scom,ang_aim)

write_my(scom,ang_aim(4),2,1);
pause(0.1);
write_my(scom,ang_aim(3),3,3);
pause(0.1);
write_my(scom,ang_aim(2),4,3);
pause(0.1);
write_my(scom,ang_aim(1),5,3);
pause(0.1);
write_my(scom,ang_aim(1),5,3);
pause(0.2);
write_my(scom,ang_aim(2),4,3);
pause(0.2);
write_my(scom,ang_aim(3),3,3);
pause(0.2);
write_my(scom,ang_aim(4),2,3);
pause(0.2);