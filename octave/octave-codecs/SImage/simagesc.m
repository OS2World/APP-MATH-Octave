#written by Stefan Mueller stm@fgan.de 1996
function x = simagesc (x)

  [ high, wide ] = size (x);
  maxval = max (max (x));
  minval = min (min (x));
  if (maxval == minval)
    x = ones (high, wide);
  else
    x = fix ((x - minval) / (maxval - minval) * (length (colormap) - 1)) + 1;
  endif
  simage (x);

endfunction
