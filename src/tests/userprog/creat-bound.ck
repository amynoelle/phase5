# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(creat-bound) begin
(creat-bound) creat("quux.dat"): 1
(creat-bound) end
creat-bound: exit(0)
EOF
pass;
