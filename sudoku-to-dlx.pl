#!/usr/bin/perl

# Read input
$dim = 3;
$read = 0;
while($read < $dim*$dim*$dim*$dim && ($line = <>))
{
    foreach $chr (split //, $line) {
        $arr[$read++] = $chr if $chr =~ /[1-9A-F0.]/;
        $dim = 4 if $chr =~ /[A-F0]/;
    }
}
$size = $dim*$dim;
die 'Invalid dimension' if($dim != 3);
die 'Premature end of input' if($read < $size*$size);

@input = (729, 324);
for($p = 0; $p < $size*$size; ++$p)
{
    for($d = 1; $d <= 9; ++$d)
    {
        @row = ();
        for($n = 0; $n < 324; ++$n)
        {
            push @row, 0;
        }

        if($arr[$p] eq '.' || $arr[$p] == $d)
        {
            $r = int($p/9);
            $c = $p%9;
            $b = 3*int($r/3) + int($c/3);
            
            $row[0 + $p] = $row[81 + 9*$r + $d-1] = $row[162 + 9*$c + $d-1] = $row[243 + 9*$b + $d-1] = 1;
        }

        push @input, @row;

    }
}
print join(' ', @input);
exit;
open(PIPE, '/bin/sh -e "echo '.join(' ', @input).' | ./dlx" |');
while(<PIPE>) {
    print;
}
close(PIPE);
