# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected (IGNORE_EXIT_CODES => 1, [<<'EOF']);
(syn-unlink) begin
(syn-unlink) creat "deleteme"
(syn-unlink) open "deleteme"
(syn-unlink) unlink "deleteme"
(syn-unlink) write "deleteme"
(syn-unlink) seek "deleteme" to 0
(syn-unlink) read "deleteme"
(syn-unlink) close "deleteme"
(syn-unlink) end
EOF
pass;
