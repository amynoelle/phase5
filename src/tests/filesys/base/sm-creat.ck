# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected (IGNORE_EXIT_CODES => 1, [<<'EOF']);
(sm-creat) begin
(sm-creat) creat "blargle"
(sm-creat) open "blargle" for verification
(sm-creat) verified contents of "blargle"
(sm-creat) close "blargle"
(sm-creat) end
EOF
pass;
