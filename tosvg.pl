#!/usr/bin/perl

$BORDER=$ENV{'BORDER'};

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
die 'Premature end of input' if($read < $size*$size);

print '<?xml version="1.0" standalone="no" ?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
    "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg"
    width="', 20 + 30*$size, '" height="', 20 + 30*$size, '" version="1.1">
<desc>Sudoku puzzle</desc>';

print '<rect x="5" y="5" width="', 10+30*$size, '" height="', 10+30*$size, '"
    fill="white" stroke="black" stroke-width="3" />' if($BORDER);

# Draw block backgrounds
for($r=0; $r<$dim; ++$r)
{
    for($c=0; $c<$dim; ++$c)
    {
        print '<rect x="', 10+30*$dim*$c,'" y="', 10+30*$dim*$r,'" width="', 30*$dim, '" height="', 30*$dim, '" fill="', ((($r + $c)%2) ? 'white' : '#e8e8e8') ,'" />';
    }
}

# Draw block seperators
for($n=1; $n<$dim; ++$n)
{
    print '<line x1="', 10,'" y1="', 10+30*$dim*$n,'" x2="', 10+30*$size,'" y2="', 10+30*$dim*$n,'" stroke="black" stroke-width="3" />';
    print '<line x1="', 10+30*$dim*$n,'" y1="', 10,'" x2="', 10+30*$dim*$n,'" y2="', 10+30*$size,'" stroke="black" stroke-width="3" />';
}

# Draw cells
for($r=0; $r<$size; ++$r)
{
    for($c=0; $c<$size; ++$c)
    {
        print '<rect x="', 10+30*$c,'" y="', 10+30*$r,'" width="30" height="30" fill="none" stroke="black" stroke-width="1" />';
    }
}

# Draw givens
for($r=0; $r<$size; ++$r)
{
    for($c=0; $c<$size; ++$c)
    {
        next if($arr[$size*$r+$c] eq '.');
        print '<text x="', 17.5 + 30*$c, '" y="', 35 + 30*$r, '" fill="black" font-family="sans-serif" font-size="20">', $arr[$size*$r+$c],'</text>';
    }
}

print '</svg>';
