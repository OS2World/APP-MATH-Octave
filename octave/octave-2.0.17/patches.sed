/^Only in .*$/d
/^Binary files .*$/d

/^diff -c.*lex.cc .*lex.cc$/,/^diff -c.*lex.l .*lex.l$/{
  /^diff -c.*lex.l .*lex.l$/p
  d
}

/^diff -c.*parse.cc .*parse.cc$/,/^diff -c.*parse.y .*parse.y$/{
  /^diff -c.*parse.y .*parse.y$/p
  d
}
