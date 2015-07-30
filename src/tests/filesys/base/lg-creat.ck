# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected (IGNORE_EXIT_CODES => 1, [<<'EOF']);
(lg-creat) begin
(lg-creat) creat "blargle"
(lg-creat) open "blargle" for verification
(lg-creat) verified contents of "blargle"
(lg-creat) close "blargle"
(lg-creat) end
EOF
pass;
