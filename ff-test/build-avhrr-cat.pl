#!/usr/local/bin/perl
#
# Build an avhrr catalog. Works only for June 1998 files. Used to test the
# DODS_Time class.

while (<>) {
    chop; chop;			# Remove newline and `*' characters
    my ($sat, $year, $day, $hh, $mm, $ss) = unpack "a a2 a3 a2 a2 a2", $_;
    my $url = "http://dods.gso.uri.edu/cgi-bin/nph-dsp/avhrr/1998/6/". $_;

    print "$sat $year $day $hh $mm $ss $url\n";
}

