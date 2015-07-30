# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(write-normal) begin
(write-normal) creat "test.txt"
(write-normal) open "test.txt"
(write-normal) end
write-normal: exit(0)
EOF
pass;
