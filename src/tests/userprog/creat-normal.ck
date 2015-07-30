# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(creat-normal) begin
(creat-normal) creat quux.dat
(creat-normal) end
creat-normal: exit(0)
EOF
pass;
