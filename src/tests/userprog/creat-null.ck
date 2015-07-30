# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(creat-null) begin
creat-null: exit(-1)
EOF
pass;
