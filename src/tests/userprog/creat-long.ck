# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(creat-long) begin
(creat-long) creat("x..."): 0
(creat-long) end
creat-long: exit(0)
EOF
pass;
