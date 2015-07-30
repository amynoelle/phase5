# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(creat-bad-ptr) begin
creat-bad-ptr: exit(-1)
EOF
pass;
