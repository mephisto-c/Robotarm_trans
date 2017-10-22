function write_my(com,in_angel,in_sel,in_speed)
%scom=serial('com5'); 
%scom.BaudRate = 9600;
%fopen(scom);
%以上为测试程序的
char ch_angel ch_sel ch_speed data;
angel=in_angel;
sel = in_sel;
speed = in_speed;
ch_angel = num2str(angel);
ch_sel = num2str(sel);
ch_speed = num2str(speed);
data = ['steer_set_sel(',ch_angel,',',' ',ch_sel,',',' ',ch_speed,')'];
fprintf(com,'%s\r\n',data);