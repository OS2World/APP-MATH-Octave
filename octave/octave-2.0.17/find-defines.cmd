/* Searches for all Makros */
/* (c) K. Gebhardt, 1998 */

'@echo off'

'setlocal'
'set deldir='



'rm -f grep.res grep.res~'



'cd liboctave'
'grep -e "#if" *.h >> ../grep.res'
'grep -e "#if" *.c* >> ../grep.res'
'grep -e "HAVE_" *.h >> ../grep.res'
'grep -e "HAVE_" *.c* >> ../grep.res'
'cd ..'

'cd src'
'grep -e "#if" *.h >> ../grep.res'
'grep -e "#if" *.c* >> ../grep.res'
'grep -e "HAVE_" *.h >> ../grep.res'
'grep -e "HAVE_" *.c* >> ../grep.res'
'cd ..'



'cut -d: -f2 grep.res >grep.res~'
'h:\bin\sort grep.res~ | uniq >grep.res'

'sed -e "s/#[ ]*ifndef//g" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "s/#[ ]*ifdef//g" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "s/#[ ]*if//g" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "s/#[ ]*elif//g" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "s/#[ ]*else//g" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "s/#[ ]*endif//g" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "s/#[ ]*define//g" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "s/\/\*.*\*\///g" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "s/[()!&|\\=<>]*//g" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "s/defined//g" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "/octave_.*_h/d" grep.res >grep.res~'
'mv grep.res~ grep.res'

rc = stream('grep.res','c','open read');
rc = stream('grep.res~','c','open write');

do while(chars('grep.res')>0)
  c = charin('grep.res');
  if (c < ' ') then rc = charout('grep.res~',x2c('0A'));
  else              rc = charout('grep.res~',c);
end

rc = stream('grep.res','c','close');
rc = stream('grep.res~','c','close');
'mv grep.res~ grep.res'

'sed -e "s/ //g" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "/^$/d" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "/^[_01248]/d" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "/^yy/d" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "/^SIG/d" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "/^F2C/d" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "/^F77/d" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "/^CC/d" grep.res >grep.res~'
'mv grep.res~ grep.res'

'sed -e "/^YY/d" grep.res >grep.res~'
'h:\bin\sort grep.res~ | uniq >grep.res'
'rm -f grep.res~'

'endlocal'

exit
