#!/usr/bin/perl

@empty_grid = ();
for($p = 0; $p < 81; ++$p)
{
    push @empty_grid, '.';
}

while(<>)
{
    @grid = @empty_grid;
    foreach $row (split(/\s/)) {
        $grid[int($row/9)] = 1 + ($row%9);
    }
    print @grid, "\n";
}