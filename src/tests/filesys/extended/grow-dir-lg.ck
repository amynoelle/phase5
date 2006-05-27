# -*- perl -*-
use strict;
use warnings;
use tests::tests;
use tests::random;
check_expected (IGNORE_EXIT_CODES => 1, [<<'EOF']);
(grow-dir-lg) begin
(grow-dir-lg) mkdir /x
(grow-dir-lg) creating and checking "/x/file0"
(grow-dir-lg) creating and checking "/x/file1"
(grow-dir-lg) creating and checking "/x/file2"
(grow-dir-lg) creating and checking "/x/file3"
(grow-dir-lg) creating and checking "/x/file4"
(grow-dir-lg) creating and checking "/x/file5"
(grow-dir-lg) creating and checking "/x/file6"
(grow-dir-lg) creating and checking "/x/file7"
(grow-dir-lg) creating and checking "/x/file8"
(grow-dir-lg) creating and checking "/x/file9"
(grow-dir-lg) creating and checking "/x/file10"
(grow-dir-lg) creating and checking "/x/file11"
(grow-dir-lg) creating and checking "/x/file12"
(grow-dir-lg) creating and checking "/x/file13"
(grow-dir-lg) creating and checking "/x/file14"
(grow-dir-lg) creating and checking "/x/file15"
(grow-dir-lg) creating and checking "/x/file16"
(grow-dir-lg) creating and checking "/x/file17"
(grow-dir-lg) creating and checking "/x/file18"
(grow-dir-lg) creating and checking "/x/file19"
(grow-dir-lg) creating and checking "/x/file20"
(grow-dir-lg) creating and checking "/x/file21"
(grow-dir-lg) creating and checking "/x/file22"
(grow-dir-lg) creating and checking "/x/file23"
(grow-dir-lg) creating and checking "/x/file24"
(grow-dir-lg) creating and checking "/x/file25"
(grow-dir-lg) creating and checking "/x/file26"
(grow-dir-lg) creating and checking "/x/file27"
(grow-dir-lg) creating and checking "/x/file28"
(grow-dir-lg) creating and checking "/x/file29"
(grow-dir-lg) creating and checking "/x/file30"
(grow-dir-lg) creating and checking "/x/file31"
(grow-dir-lg) creating and checking "/x/file32"
(grow-dir-lg) creating and checking "/x/file33"
(grow-dir-lg) creating and checking "/x/file34"
(grow-dir-lg) creating and checking "/x/file35"
(grow-dir-lg) creating and checking "/x/file36"
(grow-dir-lg) creating and checking "/x/file37"
(grow-dir-lg) creating and checking "/x/file38"
(grow-dir-lg) creating and checking "/x/file39"
(grow-dir-lg) creating and checking "/x/file40"
(grow-dir-lg) creating and checking "/x/file41"
(grow-dir-lg) creating and checking "/x/file42"
(grow-dir-lg) creating and checking "/x/file43"
(grow-dir-lg) creating and checking "/x/file44"
(grow-dir-lg) creating and checking "/x/file45"
(grow-dir-lg) creating and checking "/x/file46"
(grow-dir-lg) creating and checking "/x/file47"
(grow-dir-lg) creating and checking "/x/file48"
(grow-dir-lg) creating and checking "/x/file49"
(grow-dir-lg) end
EOF
my ($fs);
$fs->{'x'}{"file$_"} = [random_bytes (512)] foreach 0...49;
check_archive ($fs);
pass;
