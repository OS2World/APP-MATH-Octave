# written by Stefan Mueller stm@fgan.de 1996 
function simage(x)
# name of postscriptviewer
  ghostview = "ghostview"; 
# parameter
  filename = "sImg";                     # name of eps-file
  title = "ImageTitle";                  # title of eps-image
  imagePosX = 55;                        # x-offset of image in inch/72
  imagePosY = 200;                       # y-offset of image in inch/72
  imageWidth = 400;                      # image width in inch/72
  imageHeight = 400;                     # image height in inch/72
  xStartValue = "0";                     # first value of x-axis
  xEndValue = sprintf("%d",columns(x));  # last value of x-axis
  yStartValue = "0";                     # first value of y-axis
  yEndValue = sprintf("%d",rows(x));     # last value of y-axis
  labelSouth="x-Label";                  # text below x-axis in south of image
  labelWest = "y-Label";                 # text beside y-axis in west of image
  labelNorth = "noLabel";                # text above x-axis in north of image
  labelEast = "noLabel";                 # text beside y-axis in east of image
  labelLegend = "legend";                # text beside legend 
# legendType = "classify";               # classify legend
  legendType = "linear";                 # linear legend
  legendStartValue = "0.0";              # first value of legend
  legendEndValue =sprintf("%d",rows(colormap));  #last value of legend

# make scaled image in eps format
  eps_file = sprintf ("%s.eps",filename);
  makesimage (x,
              filename,imagePosX,imagePosY,
              imageWidth,imageHeight,
              xStartValue,xEndValue,
              yStartValue,yEndValue,
              legendStartValue,legendEndValue,
              title,labelLegend,legendType,
              labelWest,labelSouth,labelNorth,labelEast);

# Start the gsview
  eps_file = sprintf ("%s.eps",filename);
  gsview = sprintf ("(%s %s) &",ghostview,eps_file);
  system(gsview);
endfunction
