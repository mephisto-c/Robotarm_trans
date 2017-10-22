%%»úĞµ±Û¿ØÖÆ³ÌĞò
function angle_tr1 = control(r,angle)
angle(2) = angle(2)*-1;
angle(3) = angle(3)*-1;
angel_settle = angle/30*100;
init_asettle = [600,220,638,340];
angle_tr1 = init_asettle + angel_settle;

