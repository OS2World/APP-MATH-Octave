# written by Stefan Mueller stm@fgan.de 1996 
function makesimage (X,
                     filename,imagePosX,imagePosY,
                     imageWidth,imageHeight,
                     xStartValue,xEndValue,
                     yStartValue,yEndValue,
                     legendStartValue,legendEndValue,
                     title,labelLegend,legendType,
                     labelWest,labelSouth,labelNorth,labelEast)
  map = colormap;
  frameNorth=60;
  frameSouth=40;
  frameWest=55;
  frameEast=135;
  if (strcmp(legendType,"linear"))
    legendType="le";
  else
    legendType="ll";
  endif
  oct_file = sprintf ("%s.img",filename);
  error_file = "error.tmp";

# Save image in octave image file format
  eval (['save -ascii ', oct_file, ' map X']);
  ppmtops = sprintf("pnmtops 2> %s",error_file);

# Save octave image file format in eps format 
  img_eps_file = sprintf ("%s.img.eps",filename);
  octtopnm = sprintf ("imgtopnm %s", oct_file);
  mkepsfile = sprintf ("scaleimg -ix %d -iy %d -iw %d -ih %d -x0 %s -xn %s -y0 %s -yn %s -lw %s -ls %s -ln %s -le %s -lt %s -fn %d -fs %d -fw %d -fe %d > %s",
  imagePosX,imagePosY,imageWidth,imageHeight,xStartValue,xEndValue,
  yStartValue,yEndValue,labelWest,labelSouth,labelNorth,labelEast,title,
  frameNorth,frameSouth,frameWest,frameEast,img_eps_file);
  octtops = [ octtopnm, " | ", ppmtops , " | " , mkepsfile];
# octtops = [ octtopnm, " | ", ppmtops , " >mist.ps" ];
  shell_cmd (octtops);

# Save colormap in eps format 
  frameNorth=10;
  frameSouth=10;
  frameWest=10;
  frameEast=60;
  imagePosX=imagePosX+imageWidth+60;
  imageWidth=20;
  map_eps_file = sprintf ("%s.map.eps",filename);
  cmaptopnm = sprintf ("imgtopnm -c %s", oct_file);
  mkepsfile = sprintf ("scaleimg -ix %d -iy %d -iw %d -ih %d -y0 %s -yn %s -fn %d -fs %d -fw %d -fe %d -%s %s > %s",
  imagePosX,imagePosY,imageWidth,imageHeight,
  legendStartValue,legendEndValue,
  frameNorth,frameSouth,frameWest,frameEast,
  legendType,labelLegend,map_eps_file);
  cmaptops = [ cmaptopnm, " | ", ppmtops , " | " , mkepsfile];
  shell_cmd (cmaptops);


# make  two to one eps file
  eps_file = sprintf ("%s.eps",filename);
  merge = sprintf ("cat %s %s >%s",img_eps_file,
    map_eps_file,eps_file);
  shell_cmd (merge);
  merge = sprintf ("echo showpage >>%s",eps_file);
  shell_cmd (merge);

# make clean
  rm = sprintf ("rm -f %s %s %s %s",oct_file,error_file,
       map_eps_file,img_eps_file);
  shell_cmd (rm);

endfunction
