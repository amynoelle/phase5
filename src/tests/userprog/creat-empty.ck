# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF', <<'EOF']);
(creat-empty) begin
(creat-empty) creat(""): 0
(creat-empty) end
creat-empty: exit(0)
EOF
(creat-empty) begin
creat-empty: exit(-1)
EOF
pass;
