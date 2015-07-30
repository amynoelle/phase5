# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(creat-exists) begin
(creat-exists) creat quux.dat
(creat-exists) creat warble.dat
(creat-exists) try to re-creat quux.dat
(creat-exists) creat baffle.dat
(creat-exists) try to re-creat quux.dat
(creat-exists) end
creat-exists: exit(0)
EOF
pass;
