# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(wait-for-two) begin
child-quiet: exit(81)
child-quiet: exit(81)
(wait-for-two) wait(exec()) = 81
(wait-for-two) wait(exec()) = 81
(wait-for-two) end
wait-for-two: exit(0)
EOF
pass;
