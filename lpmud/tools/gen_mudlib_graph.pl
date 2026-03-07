#!/usr/bin/env perl
use strict;
use warnings;
use File::Find;
use File::Basename qw(dirname);

my $root = "mudlib";
my $out  = shift @ARGV || "mudlib_rooms_castles.dot";

my @files;
find(
    sub {
        return unless -f $_;
        return unless $_ =~ /\.(?:c|i)$/;
        push @files, $File::Find::name;
    },
    $root
);

my %nodes;
my %edges;

sub strip_ext {
    my ($path) = @_;
    $path =~ s{^\Q$root/\E}{};
    $path =~ s/\.(?:c|i)$//;
    return $path;
}

sub normalize_path {
    my ($src, $dst) = @_;
    return undef unless defined $dst;
    $dst =~ s/^\s+|\s+$//g;
    $dst =~ s/^\/+//;
    $dst =~ s/\.(?:c|i)$//;
    return undef if $dst eq "";

    my $path;
    if ($dst =~ m{^(?:room|players)/}) {
        $path = $dst;
    } elsif ($dst =~ m{^\.\.?/}) {
        my @parts = split m{/}, dirname($src);
        for my $seg (split m{/}, $dst) {
            next if $seg eq "" || $seg eq ".";
            if ($seg eq "..") {
                pop @parts if @parts;
            } else {
                push @parts, $seg;
            }
        }
        $path = join("/", @parts);
    } elsif ($dst =~ m{/}) {
        $path = $dst;
    } else {
        my $src_dir = dirname($src);
        $path = ($src_dir eq ".") ? $dst : "$src_dir/$dst";
    }

    my @norm;
    for my $seg (split m{/+}, $path) {
        next if $seg eq "" || $seg eq ".";
        if ($seg eq "..") {
            pop @norm if @norm;
        } else {
            push @norm, $seg;
        }
    }
    return join("/", @norm);
}

sub add_edge {
    my ($src, $dst, $dir) = @_;
    return unless defined $src && defined $dst;
    return unless $src =~ m{^(?:room|players)/};
    return unless $dst =~ m{^(?:room|players)/};
    $dir = "" unless defined $dir;
    $dir =~ s/^\s+|\s+$//g;

    $nodes{$src} = 1;
    $nodes{$dst} = 1;
    $edges{"$src\0$dst\0$dir"} = 1;
}

for my $file (@files) {
    my $src = strip_ext($file);
    next unless $src =~ m{^(?:room|players)/};

    open my $fh, "<", $file or next;
    local $/;
    my $txt = <$fh>;
    close $fh;

    while ($txt =~ /dest_dir\s*=\s*\(\{(.*?)\}\)\s*;/sg) {
        my $block = $1;
        my @q = ($block =~ /"((?:\\.|[^"\\])*)"/g);
        for (my $i = 0; $i + 1 < @q; $i += 2) {
            my $dst = normalize_path($src, $q[$i]);
            my $dir = $q[$i + 1];
            add_edge($src, $dst, $dir);
        }
    }

    while ($txt =~ /move_player\s*\(\s*"((?:\\.|[^"\\])*)#((?:\\.|[^"\\])*)"/g) {
        my $dst = normalize_path($src, $2);
        add_edge($src, $dst, $1);
    }

    while ($txt =~ /"move_player"\s*,\s*"((?:\\.|[^"\\])*)#((?:\\.|[^"\\])*)"/g) {
        my $dst = normalize_path($src, $2);
        add_edge($src, $dst, $1);
    }

    while ($txt =~ /"move_player"\s*,\s*"((?:\\.|[^"\\])*)"\s*\+\s*"#"\s*\+\s*"((?:\\.|[^"\\])*)"/g) {
        my $dst = normalize_path($src, $2);
        add_edge($src, $dst, $1);
    }
}

sub node_kind {
    my ($node) = @_;
    return "room"   if $node =~ m{^room/};
    return "castle" if $node =~ m{^players/[^/]+/castle(?:$|[./_].*)?};
    return "player" if $node =~ m{^players/};
    return "other";
}

sub esc {
    my ($s) = @_;
    $s =~ s/\\/\\\\/g;
    $s =~ s/"/\\"/g;
    return $s;
}

open my $out_fh, ">", $out or die "Cannot write $out: $!\n";
print {$out_fh} "digraph mudlib_rooms_castles {\n";
print {$out_fh} "  rankdir=LR;\n";
print {$out_fh} "  graph [overlap=false, splines=true, fontsize=10, fontname=\"Helvetica\"];\n";
print {$out_fh} "  node  [style=filled, fontsize=9, fontname=\"Helvetica\"];\n";
print {$out_fh} "  edge  [fontsize=8, fontname=\"Helvetica\", color=\"#555555\"];\n\n";

for my $n (sort keys %nodes) {
    my $kind = node_kind($n);
    my $attrs;
    if ($kind eq "room") {
        $attrs = "shape=box, fillcolor=\"#dbeafe\", color=\"#1d4ed8\"";
    } elsif ($kind eq "castle") {
        $attrs = "shape=doubleoctagon, fillcolor=\"#fef3c7\", color=\"#b45309\"";
    } elsif ($kind eq "player") {
        $attrs = "shape=ellipse, fillcolor=\"#e5e7eb\", color=\"#374151\"";
    } else {
        $attrs = "shape=ellipse, fillcolor=\"#f3f4f6\", color=\"#6b7280\"";
    }
    print {$out_fh} "  \"" . esc($n) . "\" [$attrs];\n";
}

print {$out_fh} "\n";
for my $k (sort keys %edges) {
    my ($src, $dst, $dir) = split /\0/, $k, 3;
    my $label = $dir ne "" ? " [label=\"" . esc($dir) . "\"]" : "";
    print {$out_fh} "  \"" . esc($src) . "\" -> \"" . esc($dst) . "\"$label;\n";
}
print {$out_fh} "}\n";
close $out_fh;

my $node_count = scalar(keys %nodes);
my $edge_count = scalar(keys %edges);
print "Wrote $out ($node_count nodes, $edge_count edges)\n";
